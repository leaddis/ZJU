# Copyright (C) 2023, Gaussian-Grouping
# Gaussian-Grouping research group, https://github.com/lkeab/gaussian-grouping
# All rights reserved.
#
# ------------------------------------------------------------------------
# Modified from codes in Gaussian-Splatting 
# GRAPHDECO research group, https://team.inria.fr/graphdeco

import os
import torch
from random import randint
from utils.loss_utils import l1_loss, ssim, loss_cls_3d
from gaussian_renderer import render, network_gui
import sys
from scene import Scene, GaussianModel
from utils.general_utils import safe_state
import uuid
from tqdm import tqdm
from utils.image_utils import psnr
from argparse import ArgumentParser, Namespace
from arguments import ModelParams, PipelineParams, OptimizationParams
import wandb
import json

import numpy as np
from sklearn.cluster import DBSCAN
from collections import Counter

def training(dataset, opt, pipe, testing_iterations, saving_iterations, checkpoint_iterations, checkpoint, debug_from, use_wandb):
    first_iter = 0
    prepare_output_and_logger(dataset)
    gaussians = GaussianModel(dataset.sh_degree)
    scene = Scene(dataset, gaussians)
    gaussians.training_setup(opt)
    num_classes = dataset.num_classes
    print("Num classes: ",num_classes)
    classifier = torch.nn.Conv2d(gaussians.num_objects, num_classes, kernel_size=1)
    cls_criterion = torch.nn.CrossEntropyLoss(reduction='none')
    cls_optimizer = torch.optim.Adam(classifier.parameters(), lr=5e-4)
    classifier.cuda()
    if checkpoint:
        (model_params, first_iter) = torch.load(checkpoint)
        gaussians.restore(model_params, opt)

    bg_color = [1, 1, 1] if dataset.white_background else [0, 0, 0]
    background = torch.tensor(bg_color, dtype=torch.float32, device="cuda")

    iter_start = torch.cuda.Event(enable_timing = True)
    iter_end = torch.cuda.Event(enable_timing = True)

    viewpoint_stack = None
    ema_loss_for_log = 0.0
    progress_bar = tqdm(range(first_iter, opt.iterations), desc="Training progress")
    first_iter += 1
    for iteration in range(first_iter, opt.iterations + 1):        
        if network_gui.conn == None:
            network_gui.try_connect()
        while network_gui.conn != None:
            try:
                net_image_bytes = None
                custom_cam, do_training, pipe.convert_SHs_python, pipe.compute_cov3D_python, keep_alive, scaling_modifer = network_gui.receive()
                if custom_cam != None:
                    net_image = render(custom_cam, gaussians, pipe, background, scaling_modifer)["render"]
                    net_image_bytes = memoryview((torch.clamp(net_image, min=0, max=1.0) * 255).byte().permute(1, 2, 0).contiguous().cpu().numpy())
                network_gui.send(net_image_bytes, dataset.source_path)
                if do_training and ((iteration < int(opt.iterations)) or not keep_alive):
                    break
            except Exception as e:
                network_gui.conn = None

        iter_start.record()

        gaussians.update_learning_rate(iteration)

        # Every 1000 its we increase the levels of SH up to a maximum degree
        if iteration % 1000 == 0:
            gaussians.oneupSHdegree()

        # Pick a random Camera
        if not viewpoint_stack:
            viewpoint_stack = scene.getTrainCameras().copy()
            ### load from views if training labels only

        viewpoint_cam = viewpoint_stack.pop(randint(0, len(viewpoint_stack)-1))

        # Render
        if (iteration - 1) == debug_from:
            pipe.debug = True
        render_pkg = render(viewpoint_cam, gaussians, pipe, background)
        image, viewspace_point_tensor, visibility_filter, radii, objects = render_pkg["render"], render_pkg["viewspace_points"], render_pkg["visibility_filter"], render_pkg["radii"], render_pkg["render_object"]

        # print("objects: ", objects.shape, objects)

        # Object Loss #
        # gt_obj = viewpoint_cam.objects.cuda().long()
        # logits = classifier(objects)
        # print(gt_obj.shape, logits.shape)
        # loss_obj = cls_criterion(logits.unsqueeze(0), gt_obj.unsqueeze(0)).squeeze().mean()
        # loss_obj = loss_obj / torch.log(torch.tensor(num_classes))  # normalize to (0,1)

        # Loss
        gt_image = viewpoint_cam.original_image.cuda()
        Ll1 = l1_loss(image, gt_image)

        '''
        loss_obj_3d = None
        if iteration % opt.reg3d_interval == 0:
            # regularize at certain intervals
            logits3d = classifier(gaussians._objects_dc.permute(2,0,1))
            prob_obj3d = torch.softmax(logits3d,dim=0).squeeze().permute(1,0)
            loss_obj_3d = loss_cls_3d(gaussians._xyz.squeeze().detach(), prob_obj3d, opt.reg3d_k, opt.reg3d_lambda_val, opt.reg3d_max_points, opt.reg3d_sample_size)
            loss = (1.0 - opt.lambda_dssim) * Ll1 + opt.lambda_dssim * (1.0 - ssim(image, gt_image)) + loss_obj + loss_obj_3d
        else:
            loss = (1.0 - opt.lambda_dssim) * Ll1 + opt.lambda_dssim * (1.0 - ssim(image, gt_image)) + loss_obj
        '''

        loss = (1.0 - opt.lambda_dssim) * Ll1 + opt.lambda_dssim * (1.0 - ssim(image, gt_image))
        loss.backward()
        iter_end.record()

        with torch.no_grad():
            # Progress bar
            ema_loss_for_log = 0.4 * loss.item() + 0.6 * ema_loss_for_log
            if iteration % 10 == 0:
                progress_bar.set_postfix({"Loss": f"{ema_loss_for_log:.{7}f}"})
                progress_bar.update(10)
            if iteration == opt.iterations:
                progress_bar.close()

            # Log and save
            training_report(iteration, Ll1, loss, l1_loss, iter_start.elapsed_time(iter_end), testing_iterations, scene, render, (pipe, background), use_wandb)
            if (iteration in saving_iterations):

                objects_dc = gaussians._objects_dc.detach()
                objects_dc[:,0,0] = 0
                gaussians._objects_dc = objects_dc
                
                print("\n[ITER {}] Saving Gaussians".format(iteration))
                scene.save(iteration)
                torch.save(classifier.state_dict(), os.path.join(scene.model_path, "point_cloud/iteration_{}".format(iteration),'classifier.pth'))

            # Densification
            if iteration < opt.densify_until_iter:
                # Keep track of max radii in image-space for pruning
                gaussians.max_radii2D[visibility_filter] = torch.max(gaussians.max_radii2D[visibility_filter], radii[visibility_filter])
                gaussians.add_densification_stats(viewspace_point_tensor, visibility_filter)

                if iteration > opt.densify_from_iter and iteration % opt.densification_interval == 0:
                    size_threshold = 20 if iteration > opt.opacity_reset_interval else None
                    gaussians.densify_and_prune(opt.densify_grad_threshold, 0.005, scene.cameras_extent, size_threshold)
                
                if iteration % opt.opacity_reset_interval == 0 or (dataset.white_background and iteration == opt.densify_from_iter):
                    gaussians.reset_opacity()

            # Optimizer step
            if iteration < opt.iterations:
                gaussians.optimizer.step()
                gaussians.optimizer.zero_grad(set_to_none = True)
                cls_optimizer.step()
                cls_optimizer.zero_grad()

            if (iteration in checkpoint_iterations):
                print("\n[ITER {}] Saving Checkpoint".format(iteration))
                # init here
                objects_dc = gaussians._objects_dc.detach()
                objects_dc[:,0,0] = 0
                gaussians._objects_dc = objects_dc
                torch.save((gaussians.capture(), iteration), scene.model_path + "/chkpnt" + str(iteration) + ".pth")

from scipy.spatial import ConvexHull, Delaunay

def points_inside_convex_hull(point_cloud, mask, remove_outliers=True, outlier_factor=1.0):

    masked_points = point_cloud[mask].cpu().numpy()

    if remove_outliers:
        Q1 = np.percentile(masked_points, 25, axis=0)
        Q3 = np.percentile(masked_points, 75, axis=0)
        IQR = Q3 - Q1
        outlier_mask = (masked_points < (Q1 - outlier_factor * IQR)) | (masked_points > (Q3 + outlier_factor * IQR))
        filtered_masked_points = masked_points[~np.any(outlier_mask, axis=1)]
    else:
        filtered_masked_points = masked_points

    if filtered_masked_points.shape[0] < 5:
        return mask

    delaunay = Delaunay(filtered_masked_points)

    points_inside_hull_mask = delaunay.find_simplex(point_cloud.cpu().numpy()) >= 0

    inside_hull_tensor_mask = torch.tensor(points_inside_hull_mask, device='cuda')

    return inside_hull_tensor_mask

def find_clusters(point_cloud, mask):

    vertices = point_cloud[mask].cpu().numpy()

    dbscan = DBSCAN(eps=0.1, min_samples=15)
    labels = dbscan.fit_predict(vertices)

    label_counts = Counter(labels)

    most_common_label, most_common_count = label_counts.most_common(1)[0]

    max_cluster_mask = np.zeros(point_cloud.shape[0], dtype=bool)
    max_cluster_mask[mask.cpu()] = (labels == most_common_label)

    return torch.tensor(max_cluster_mask, device='cuda')


def labelling(args):

    gaussians = GaussianModel(args.sh_degree)
    scene = Scene(args, gaussians, load_iteration=args.iteration, shuffle=False, only_gaussians=args.scanrefer)

    bg_color = [1, 1, 1] if args.white_background else [0, 0, 0]
    background = torch.tensor(bg_color, dtype=torch.float32, device="cuda")

    from scene.dataset_readers import CameraInfo
    from utils.camera_utils import cameraList_from_camInfos
    import pickle
    import cv2 

    camera_path = os.path.join(args.model_path, 'object_mask', 'cam_infos.pkl')
    with open(camera_path, 'rb') as f:
        cam_infos = pickle.load(f)

    new_cam_infos = []
    kernel = np.ones((3, 3), np.uint8)

    for id, cam_info in enumerate(cam_infos):
        img_path = os.path.join(args.model_path, 'object_mask', f'{id:05}.jpg')
        obj_mask = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        obj_mask = cv2.erode(obj_mask, kernel, iterations=4)
        new_cam_info = cam_info._replace(objects=obj_mask)
        new_cam_infos.append(new_cam_info)
    viewpoint_stack = cameraList_from_camInfos(new_cam_infos, 1.0, args)

    progress_bar = tqdm(range(1, len(viewpoint_stack)+1), desc="Labelling progress")

    votings = torch.zeros(gaussians._xyz.shape[0]).cuda()

    ##### Opacity & Voting Theshold #####
    opacity_theshold = 0.8
    voting_theshold = 0.5

    for iteration, viewpoint_cam in enumerate(viewpoint_stack):        

        # Render
        render_pkg = render(viewpoint_cam, gaussians, args, background)
        objects, max_opacity = render_pkg["render_object"], render_pkg["max_opacity"]

        votings += (max_opacity > opacity_theshold).float()

        progress_bar.update(1)

    ### labelling here
    voting_num = 0.1 * torch.max(votings)
    labelling_mask = (votings > voting_num)
    labelling_mask = labelling_mask.bool().squeeze()

    # get mask

    selected_points = gaussians._xyz[labelling_mask].detach()
    selected_points_np = selected_points.cpu().numpy()
    gaussians_np = gaussians._xyz.detach().cpu().numpy()
    Q1 = np.percentile(selected_points_np, 1, axis=0)
    Q3 = np.percentile(selected_points_np, 99, axis=0)
    inlier_mask = (gaussians_np > Q1) & (gaussians_np < Q3)
    inlier_mask = torch.tensor(np.all(inlier_mask, axis=1), dtype=torch.bool).cuda()

    mask3d = inlier_mask & labelling_mask
    
    # get gaussians in the convex hull
    mask3d_convex = points_inside_convex_hull(gaussians._xyz.detach(),mask3d,outlier_factor=1)
    mask3d = torch.logical_or(mask3d,mask3d_convex)
    print('points after convex_hull:', torch.nonzero(mask3d).size(0))
    
    mask3d = find_clusters(gaussians._xyz.detach(),mask3d)
    print('points after cluster:', torch.nonzero(mask3d).size(0))

    # init for existing ply
    if args.scanrefer:
        objects_dc = gaussians._objects_dc.detach()
        objects_dc[:,0,0] = 0
        gaussians._objects_dc = objects_dc

    mask3d = mask3d & (gaussians._objects_dc[:,0,0] == 0)

    obj_num = torch.max(gaussians._objects_dc[:,0,0]) + 1
    objects_dc = gaussians._objects_dc.detach()
    objects_dc[mask3d,0,0] = obj_num
    gaussians._objects_dc = objects_dc

    progress_bar.close()

    # Log and save
    print("\n[ITER {}] Saving Gaussians".format(scene.loaded_iter))
    scene.save(scene.loaded_iter)



def prepare_output_and_logger(args):    
    if not args.model_path:
        if os.getenv('OAR_JOB_ID'):
            unique_str=os.getenv('OAR_JOB_ID')
        else:
            unique_str = str(uuid.uuid4())
        args.model_path = os.path.join("./data/", unique_str[0:10])
        
    # Set up output folder
    print("Output folder: {}".format(args.model_path))
    os.makedirs(args.model_path, exist_ok = True)
    with open(os.path.join(args.model_path, "cfg_args"), 'w') as cfg_log_f:
        cfg_log_f.write(str(Namespace(**vars(args))))


def training_report(iteration, Ll1, loss, l1_loss, elapsed, testing_iterations, scene : Scene, renderFunc, renderArgs, use_wandb):

    if use_wandb:
        # if loss_obj_3d:
            # wandb.log({"train_loss_patches/l1_loss": Ll1.item(), "train_loss_patches/total_loss": loss.item(), "train_loss_patches/loss_obj_3d": loss_obj_3d.item(), "iter_time": elapsed, "iter": iteration})
        # else:
            # wandb.log({"train_loss_patches/l1_loss": Ll1.item(), "train_loss_patches/total_loss": loss.item(), "iter_time": elapsed, "iter": iteration})
        wandb.log({"train_loss_patches/l1_loss": Ll1.item(), "train_loss_patches/total_loss": loss.item(), "iter_time": elapsed, "iter": iteration})
    
    # Report test and samples of training set
    if iteration in testing_iterations:
        torch.cuda.empty_cache()
        validation_configs = ({'name': 'test', 'cameras' : scene.getTestCameras()}, 
                              {'name': 'train', 'cameras' : [scene.getTrainCameras()[idx % len(scene.getTrainCameras())] for idx in range(5, 30, 5)]})

        for config in validation_configs:
            if config['cameras'] and len(config['cameras']) > 0:
                l1_test = 0.0
                psnr_test = 0.0
                for idx, viewpoint in enumerate(config['cameras']):
                    image = torch.clamp(renderFunc(viewpoint, scene.gaussians, *renderArgs)["render"], 0.0, 1.0)
                    gt_image = torch.clamp(viewpoint.original_image.to("cuda"), 0.0, 1.0)
                    if use_wandb:
                        if idx < 5:
                            wandb.log({config['name'] + "_view_{}/render".format(viewpoint.image_name): [wandb.Image(image)]})
                            if iteration == testing_iterations[0]:
                                wandb.log({config['name'] + "_view_{}/ground_truth".format(viewpoint.image_name): [wandb.Image(gt_image)]})
                    l1_test += l1_loss(image, gt_image).mean().double()
                    psnr_test += psnr(image, gt_image).mean().double()
                psnr_test /= len(config['cameras'])
                l1_test /= len(config['cameras'])          
                print("\n[ITER {}] Evaluating {}: L1 {} PSNR {}".format(iteration, config['name'], l1_test, psnr_test))
                if use_wandb:
                    wandb.log({config['name'] + "/loss_viewpoint - l1_loss": l1_test, config['name'] + "/loss_viewpoint - psnr": psnr_test})
        if use_wandb:
            wandb.log({"scene/opacity_histogram": scene.gaussians.get_opacity, "total_points": scene.gaussians.get_xyz.shape[0], "iter": iteration})
        torch.cuda.empty_cache()

if __name__ == "__main__":
    # Set up command line argument parser
    parser = ArgumentParser(description="Training script parameters")
    lp = ModelParams(parser)
    op = OptimizationParams(parser)
    pp = PipelineParams(parser)
    parser.add_argument('--ip', type=str, default="127.0.0.1")
    parser.add_argument('--port', type=int, default=6009)
    parser.add_argument('--debug_from', type=int, default=-1)
    parser.add_argument('--detect_anomaly', action='store_true', default=False)
    parser.add_argument("--test_iterations", nargs="+", type=int, default=[15_000, 30_000, 60_000, 90_000])
    parser.add_argument("--save_iterations", nargs="+", type=int, default=[15_000, 30_000, 60_000, 90_000])
    parser.add_argument("--quiet", action="store_true")
    parser.add_argument("--checkpoint_iterations", nargs="+", type=int, default=[])
    parser.add_argument("--start_checkpoint", type=str, default = None)
    # Add an argument for the configuration file
    parser.add_argument("--config_file", type=str, default="config/gaussian_dataset/train.json", help="Path to the configuration file")
    parser.add_argument("--use_wandb", action='store_true', default=False, help="Use wandb to record loss value")

    parser.add_argument("--train_labels", action='store_true', default=False)
    parser.add_argument("--scanrefer", action='store_true', default=False)

    parser.add_argument("--iteration", type=int, default=-1)
    parser.add_argument("--get_gvec", action='store_true')

    args = parser.parse_args(sys.argv[1:])

    args.iterations = args.iteration
    args.position_lr_max_steps = args.iterations
    args.densify_until_iter = args.iterations
    args.save_iterations.append(args.iterations)

    args.dataset = args.source_path.split('/')[-1]
    if args.get_gvec:
        with open('config/camera_rotation.json', 'r') as file:
            camera_config = json.load(file)
        camera_config["dataset"] = args.dataset
        camera_config["gvec_rotate_euler"] = [0,0,0]
        camera_config["rotation"] = 0
        camera_config["transform"] = [0,0,0]
        with open('config/camera_rotation.json', 'w') as file:
            camera_config = json.dump(camera_config, file, indent=4)
    else:
        os.system(f'cp config/camera_rotation.json config/camera_rotation_{args.dataset}.json')

    if args.train_labels:
        args.sh_degree = (0 if args.scanrefer else 3)
        labelling(args)
        # os.system(f'python render.py -m {args.model_path} --num_classes 256 --images images --iteration 0')
        # training_labels(lp.extract(args), op.extract(args), pp.extract(args), args.test_iterations, args.save_iterations, args.checkpoint_iterations, args.start_checkpoint, args.debug_from, args.use_wandb)
        # os.system(f'python render.py -m {args.model_path} --num_classes 256 --images images --iteration {args.iterations // 100}')
    
    else:

        # Read and parse the configuration file
        try:
            with open(args.config_file, 'r') as file:
                config = json.load(file)
        except FileNotFoundError:
            print(f"Error: Configuration file '{args.config_file}' not found.")
            exit(1)
        except json.JSONDecodeError as e:
            print(f"Error: Failed to parse the JSON configuration file: {e}")
            exit(1)

        args.densify_until_iter = config.get("densify_until_iter", 15000)
        args.num_classes = config.get("num_classes", 200)
        args.reg3d_interval = config.get("reg3d_interval", 2)
        args.reg3d_k = config.get("reg3d_k", 5)
        args.reg3d_lambda_val = config.get("reg3d_lambda_val", 2)
        args.reg3d_max_points = config.get("reg3d_max_points", 300000)
        args.reg3d_sample_size = config.get("reg3d_sample_size", 1000)
        
        print("Optimizing " + args.model_path)

        if args.use_wandb:
            wandb.init(project="gaussian-splatting")
            wandb.config.args = args
            wandb.run.name = args.model_path

        # Initialize system state (RNG)
        safe_state(args.quiet)

        # Start GUI server, configure and run training
        network_gui.init(args.ip, args.port)
        torch.autograd.set_detect_anomaly(args.detect_anomaly)


        training(lp.extract(args), op.extract(args), pp.extract(args), args.test_iterations, args.save_iterations, args.checkpoint_iterations, args.start_checkpoint, args.debug_from, args.use_wandb)

    # All done
    print("\nTraining complete.")
