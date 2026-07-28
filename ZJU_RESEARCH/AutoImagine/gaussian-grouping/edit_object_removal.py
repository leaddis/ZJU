# Copyright (C) 2023, Gaussian-Grouping
# Gaussian-Grouping research group, https://github.com/lkeab/gaussian-grouping
# All rights reserved.
#
# ------------------------------------------------------------------------
# Modified from codes in Gaussian-Splatting 
# GRAPHDECO research group, https://team.inria.fr/graphdeco

import torch
from scene import Scene
import os
from tqdm import tqdm
from os import makedirs
from gaussian_renderer import render
import torchvision
from utils.general_utils import safe_state
from argparse import ArgumentParser
from arguments import ModelParams, PipelineParams, OptimizationParams, get_combined_args
from gaussian_renderer import GaussianModel
import numpy as np
from PIL import Image
import json

import cv2

from scipy.spatial import Delaunay

from sklearn.cluster import DBSCAN
from collections import Counter

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

def get_center(point_cloud, mask):

    selected_points = point_cloud[mask.bool().squeeze()]

    selected_points_np = selected_points.cpu().numpy()
    Q1 = np.percentile(selected_points_np, 1, axis=0)
    Q3 = np.percentile(selected_points_np, 99, axis=0)
    outlier_mask = (selected_points_np < Q1) | (selected_points_np > Q3)
    filtered_selected_points = torch.Tensor(selected_points_np[~np.any(outlier_mask, axis=1)]).cuda()

    min_coor, _tmp = torch.min(filtered_selected_points, dim=0)
    max_coor, _tmp = torch.max(filtered_selected_points, dim=0)

    shrink_rate = 0.02
    delta = (max_coor - min_coor) * shrink_rate
    min_coor += delta
    max_coor -= delta

    center = (min_coor + max_coor) / 2

    return center

def get_pixel_coord(viewpoint_camera, point):
    point_coord = list(point)
    point_coord.append(1)
    quad_coord = np.array([point_coord], dtype=np.float32)
    proj_coord = (np.dot(viewpoint_camera.full_proj_transform.cpu().numpy().T, quad_coord.T)).T

    proj_coord[:, :2] /= proj_coord[:, 3, np.newaxis]
    proj_coord = proj_coord[:, :2]
    pixel_coord = ((proj_coord + 1) / 2 * np.array([viewpoint_camera.image_width, viewpoint_camera.image_height])).astype(np.int32)
    return np.array(pixel_coord).squeeze()

def check_pos(mask, left_up, right_down):
    l,u = left_up
    r,d = right_down
    rect = mask[u:d+1, l:r+1]  

    white_pixels = np.sum(rect == 255)  
    total_pixels = rect.size  

    ratio = white_pixels / total_pixels  
    if ratio < 0.8:
        return 0
    else:
        rect[:] = 0
        return 1

def crop_image_to_size(image, target_width, target_height):
    original_width, original_height = image.size
    crop_left = (original_width - target_width) // 2
    crop_right = original_width - crop_left
    crop_top = (original_height - target_height) // 2
    crop_bottom = original_height - crop_top
    cropped_image = image.crop((crop_left, crop_top, crop_right, crop_bottom))
    return cropped_image


def find_clusters(vertices: np.ndarray):

    dbscan = DBSCAN(eps=0.05, min_samples=15)
    labels = dbscan.fit_predict(vertices)

    label_counts = Counter(labels)

    most_common_label, most_common_count = label_counts.most_common(1)[0]

    members = vertices[labels == most_common_label]
    centroid = np.mean(members, axis=0)

    sx = np.max(members[:, 0]) - np.min(members[:, 0])
    sy = np.max(members[:, 1]) - np.min(members[:, 1])
    sz = np.max(members[:, 2]) - np.min(members[:, 2])

    extend = (sx, sy, sz)

    return centroid, extend

def get_bb(point_cloud, mask, bb):

    selected_points = point_cloud[mask.bool().squeeze()]
    selected_points_np = selected_points.cpu().numpy()

    centroid, extend = find_clusters(selected_points_np)

    bb['size'] = [float(extend[i] / 2) for i in range(3)]
    bb['centroid'] = [float(centroid[i]) for i in range(3)]


def compute_camera_extrinsics(center, size, fov_x, fov_y, theta):
    Cx, Cy, Cz = center
    maxl = 2 * max(size)

    r_x = 2 * maxl / (2 * np.tan(fov_x / 2))
    r_y = 2 * maxl / (2 * np.tan(fov_y / 2))
    r = max(r_x, r_y)
    
    Px = Cx + r * np.cos(theta)
    Py = Cy + r * np.sin(theta)
    Pz = Cz + size[2] * 2 
    
    P_cam = np.array([Px, Py, Pz])
    d = np.array([Cx - Px, Cy - Py, Cz - Pz])
    d = d / np.linalg.norm(d)  
    
    up = np.array([0, 0, 1])
    
    right = -np.cross(up, d)
    right = right / np.linalg.norm(right)
    
    up = np.cross(d, right)
    
    R = np.vstack([right, up, d]).T
    T = -np.linalg.inv(R) @ P_cam
    
    return R, T, r

def cal_dis(point_cloud, T_init, threshold_xy = 0.1):

    points = point_cloud.cpu().numpy()
    filtered_points = points[
        (np.abs(points[:, 0] - T_init[0]) < threshold_xy) & 
        (np.abs(points[:, 1] - T_init[1]) < threshold_xy) &
        (points[:, 2] < T_init[2])
    ]

    print(len(filtered_points), np.percentile(filtered_points, 85, axis=0)[2], np.percentile(filtered_points, 90, axis=0)[2], np.percentile(filtered_points, 95, axis=0)[2])

    upper_center = np.percentile(filtered_points, 95, axis=0)
    return T_init[2] - upper_center[2]

def generate_spiral_path(T_init, radius, num_frames=180):
    
    theta = np.linspace(0, 2 * np.pi * 5, num_frames) 
    
    z = np.linspace(T_init[2], T_init[2] - radius / 2, num_frames)  
    
    r = np.sqrt(radius ** 2 - (z - (T_init[2] - radius)) ** 2) 
    r[0] += 1e-5

    x = T_init[0] + r * np.cos(theta)
    y = T_init[1] + r * np.sin(theta)
    
    return np.vstack((x, y, z)).T

def generate_circle_path(T_init, T_obj, num_frames=60):
    
    init_theta = np.arctan2(T_init[1]-T_obj[1], T_init[0]-T_obj[0])
    theta = np.linspace(0, 2 * np.pi, num_frames) + init_theta 


    r = np.sqrt((T_init[0] - T_obj[0]) ** 2 + (T_init[1] - T_obj[1]) ** 2)    
    x = T_obj[0] + r * np.cos(theta)
    y = T_obj[1] + r * np.sin(theta)
    z = np.full(x.shape, T_init[2])

    return np.vstack((x, y, z)).T


def look_at(camera_position, target_position):
    forward = target_position - camera_position
    forward = forward / np.linalg.norm(forward)
    
    up = np.array([0.0, 0.0, 1.0]) 
    
    if np.allclose(up, forward):
        return np.array([[1,0,0],[0,1,0],[0,0,1]])
    if np.allclose(-up, forward):
        return np.array([[1,0,0],[0,-1,0],[0,0,-1]])

    right = np.cross(forward, up)
    right = right / np.linalg.norm(right)
    
    up = np.cross(right, forward)
    
    rotation_matrix = np.vstack((right, -up, forward)).T
    return rotation_matrix

def find_first_intersection(ray_origin, ray_direction, points, tolerance=0.1):
    ray_direction = ray_direction / np.linalg.norm(ray_direction)

    intersections = []

    for point in points:
        vec_to_point = point - ray_origin
        projection_length = np.dot(vec_to_point, ray_direction)

        if projection_length < 0:
            continue
        projection_point = ray_origin + projection_length * ray_direction
        distance_to_ray = np.linalg.norm(point - projection_point)
        if distance_to_ray < tolerance:
            distance_from_origin = np.linalg.norm(point - ray_origin)
            intersections.append((point, distance_from_origin))
    
    if intersections:
        intersections.sort(key=lambda x: x[1]) 
        count_to_take = max(1, len(intersections) * 5 // 100) 
        return intersections[count_to_take - 1][0]
    
    else:
        print('find intersection failed!')
        return find_first_intersection(ray_origin, ray_direction, points, tolerance=tolerance + 0.05)
        raise


def editing_setup(opt, model_path, iteration, views, gaussians, pipeline, background, cameras_extent, removal_thresh, args):

    selected_obj_ids = torch.tensor(args.select_obj_id).cuda()

    parameter = args.dst_center if args.operation == "translate" else args.euler_angle if args.operation == "rotate" else ""
    print(f"Editing: {args.operation} {selected_obj_ids.item()} {parameter}")

    if selected_obj_ids == -1:
        mask3d = torch.ones(gaussians._xyz.shape[0])
        masked_center = torch.zeros(3).cuda()

    elif selected_obj_ids == -2:
        mask3d = torch.zeros(gaussians._xyz.shape[0])
        masked_center = torch.zeros(3).cuda()

    else:
        with torch.no_grad():
            mask3d = gaussians._objects_dc[:,0,0] == selected_obj_ids
            mask3d = mask3d.float()[:,None,None]

        masked_center = get_center(gaussians._xyz.detach(), mask3d)
    
    if args.operation == 'translate':
        dst_center = np.array(args.dst_center, dtype=float)

        if dst_center[2] > 98:
            import pickle
            with open('config/cam_info.pkl', 'rb') as f:
                cam_info_0 = pickle.load(f)
                fx_color = cam_info_0.FovX
                fy_color = cam_info_0.FovY
                        
            points = gaussians._xyz.detach().cpu().numpy()
            z_avg = np.average(points[:,2], axis=0)

            if args.render_coord is None:
                x_min = np.percentile(points[:,0], 1, axis=0)
                x_max = np.percentile(points[:,0], 99, axis=0)
                y_min = np.percentile(points[:,1], 1, axis=0)
                y_max = np.percentile(points[:,1], 99, axis=0)

            else:
                x_min, x_max, y_min, y_max = args.render_coord

            pixels_per_unit = 1200 / max((x_max - x_min), (y_max - y_min))
            
            z_cam = z_avg + fx_color / pixels_per_unit
            T_real = np.array([(x_min + x_max) / 2, (y_min + y_max) / 2, z_cam])
            dst_center[2] = z_avg

            if not mask3d.any():
                
                new_dst_center = find_first_intersection(T_real, dst_center-T_real, points)
                new_dst_center[2] += 0.1
                print(dst_center) 

                dataset = args.model_path.split('/')[-1]
                with open(f"data/{dataset}/coord", 'w') as file:
                    file.write(f"{new_dst_center[0]} {new_dst_center[1]} {new_dst_center[2]}\n")

                point_cloud_path = os.path.join(model_path, "point_cloud"+"/iteration_{}".format(iteration))
                gaussians.save_ply(os.path.join(point_cloud_path, "point_cloud.ply"))

                return gaussians

            else:
                points = gaussians._xyz.detach()
                obj_bb = {}
                get_bb(points, mask3d, obj_bb)
                other_points = points[~(mask3d.bool().squeeze())]
                new_dst_center = find_first_intersection(T_real, dst_center-T_real, other_points.cpu().numpy())
                new_dst_center[2] += obj_bb["size"][2] / 2
                print(obj_bb["size"], ' | ', dst_center, ' | ', new_dst_center) 

                dataset = args.model_path.split('/')[-1]
                with open(f"data/{dataset}/coord", 'w') as file:
                    file.write(f"{new_dst_center[0]} {new_dst_center[1]} {new_dst_center[2]}\n")

                for i in range(3):
                    args.dst_center[i] = new_dst_center[i]


    if args.operation == 'removal':
        gaussians.removal_setup(opt, mask3d)
    elif args.operation == 'translate':
        
        dst_center = torch.FloatTensor(args.dst_center).cuda()

        gaussians.translate_setup(opt, mask3d, masked_center, dst_center)

    elif args.operation == 'rotate':

        from scipy.spatial.transform import Rotation
        rotate_matrix = Rotation.from_euler('xyz', args.euler_angle, degrees=True).as_matrix()
        gaussians.rotate_setup(opt, mask3d, masked_center, rotate_matrix)

    point_cloud_path = os.path.join(model_path, "point_cloud"+"/iteration_{}".format(iteration))
    gaussians.save_ply(os.path.join(point_cloud_path, "point_cloud.ply"))

    return gaussians


def render_set(model_path, name, iteration, views, gaussians, pipeline, background, args):

    render_path = os.path.join(model_path, name, "ours{}".format(iteration), "renders")
    render_path_with_axis = os.path.join(model_path, name, "ours{}".format(iteration), "renders_with_axis")
    render_path_with_labels = os.path.join(model_path, name, "ours{}".format(iteration), "renders_with_labels")
    render_path_with_highlights = os.path.join(model_path, name, "ours{}".format(iteration), "renders_with_highlights")
    makedirs(render_path, exist_ok=True)
    makedirs(render_path_with_axis, exist_ok=True)
    makedirs(render_path_with_labels, exist_ok=True)
    makedirs(render_path_with_highlights, exist_ok=True)

    for idx, view in enumerate(tqdm(views, desc="Rendering progress")):
        results = render(view, gaussians, pipeline, background)
        rendering = results["render"]
        
        torchvision.utils.save_image(rendering, os.path.join(render_path, '{0:05d}'.format(idx) + ".png"))

        if args.render_highlights:
            
            rendering_with_highlights = rendering.permute(1,2,0).cpu().numpy()
            rendering_with_highlights = np.clip(rendering_with_highlights * 255, 0, 255).astype(np.uint8)
            # rendering_with_highlights = (rendering_with_highlights * 255).astype(np.uint8)
            rendering_with_highlights = cv2.cvtColor(rendering_with_highlights.astype(np.uint8), cv2.COLOR_RGB2BGR)

            color_list = [
                ("black", (0, 0, 0)),
                ("Red", (0, 0, 255)),
                ("Green", (0, 255, 0)),
                ("Blue", (255, 0, 0)),
                ("Yellow", (0, 255, 255)),
                ("Gray", (128, 128, 128)),
                ("Cyan", (255, 255, 0)),
                ("Orange", (0, 165, 255)),
                ("Purple", (128, 0, 128)),
                ("Pink", (203, 192, 255)),
                ("Brown", (42, 42, 165)),
            ]
            dir_list = np.array([[1,1,1],[1,1,-1],[1,-1,1],[1,-1,-1],[-1,1,1],[-1,1,-1],[-1,-1,1],[-1,-1,-1]])

            if args.render_highlights[0] == -1:
                obj_list = range(1,256)
            else:
                obj_list = args.render_highlights


            for obj_id in obj_list:
                with torch.no_grad():
                    mask3d = gaussians._objects_dc[:,0,0] == obj_id
                    if torch.count_nonzero(mask3d) < 5:
                        break
                    mask3d = mask3d.float()[:,None,None]
                    
                bb = {}
                get_bb(gaussians._xyz.detach(), mask3d, bb)

                pixel_coord = []
                for i in range(8):
                    point_coord = np.array(bb["centroid"]) + np.array(bb["size"]) * dir_list[i]
                    pixel_coord.append(get_pixel_coord(view, point_coord))
                
                min_coord = tuple(np.min(pixel_coord, axis=0))
                max_coord = tuple(np.max(pixel_coord, axis=0))
                
                cv2.rectangle(rendering_with_highlights, min_coord, max_coord, color_list[obj_id][1], 2)

            cv2.imwrite(os.path.join(render_path_with_highlights, '{0:05d}'.format(idx) + ".png"), rendering_with_highlights)

        if args.get_bbox_2d:
            dir_list = np.array([[1,1,1],[1,1,-1],[1,-1,1],[1,-1,-1],[-1,1,1],[-1,1,-1],[-1,-1,1],[-1,-1,-1]])
            centroid = [(args.get_bbox_2d[0]+args.get_bbox_2d[1])/2, (args.get_bbox_2d[2]+args.get_bbox_2d[3])/2, (args.get_bbox_2d[4]+args.get_bbox_2d[5])/2]
            size = [(args.get_bbox_2d[1]-args.get_bbox_2d[0])/2, (args.get_bbox_2d[3]-args.get_bbox_2d[2])/2, (args.get_bbox_2d[5]-args.get_bbox_2d[4])/2] 
            
            pixel_coord = []
            for i in range(8):
                point_coord = np.array(centroid) + np.array(size) * dir_list[i]
                pixel_coord.append(get_pixel_coord(view, point_coord))
            
            min_coord = tuple(np.min(pixel_coord, axis=0))
            max_coord = tuple(np.max(pixel_coord, axis=0))

            output = {}
            output['x_min'] = min_coord[0].item()
            output['x_max'] = max_coord[0].item()
            output['y_min'] = min_coord[1].item()
            output['y_max'] = max_coord[1].item()
            
            with open(args.model_path + '/bbox_2d.json', 'w') as f:  
                json.dump(output, f, indent=4) 
            
            print("bbox_2d saved.")


def editing(args):
    gaussians = GaussianModel(args.sh_degree)
    scene = Scene(args, gaussians, load_iteration=args.iteration, shuffle=False, only_gaussians=args.scanrefer)

    num_classes = args.num_classes
    print("Num classes: ",num_classes)
    bg_color = [1,1,1] if args.white_background else [0, 0, 0]
    background = torch.tensor(bg_color, dtype=torch.float32, device="cuda")

    if args.operation == "skip":
        point_cloud_path = os.path.join(args.model_path, "point_cloud", "iteration_{}".format(scene.loaded_iter))
        gaussians.save_ply(os.path.join(point_cloud_path, "point_cloud.ply"))
    
    elif args.operation == 'crop':

        objects_dc = gaussians._objects_dc.detach()
        objects_dc[:,0,0] = 0
        gaussians._objects_dc = objects_dc

        if args.crop_coord is None:
            points = gaussians._xyz.cpu().numpy()
            x_min = np.percentile(points[:,0], 5, axis=0)
            x_max = np.percentile(points[:,0], 95, axis=0)
            y_min = np.percentile(points[:,1], 5, axis=0)
            y_max = np.percentile(points[:,1], 95, axis=0)
            z_min = np.percentile(points[:,2], 5, axis=0)
            z_max = np.percentile(points[:,2], 95, axis=0)
        else:
            x_min, x_max, y_min, y_max, z_min, z_max = args.crop_coord

        mask = (gaussians._xyz[:, 0] >= x_min) & (gaussians._xyz[:, 0] <= x_max) & (gaussians._xyz[:, 1] >= y_min) & (gaussians._xyz[:, 1] <= y_max) & (gaussians._xyz[:, 2] >= z_min) & (gaussians._xyz[:, 2] <= z_max)

        gaussians._xyz = gaussians._xyz[mask].detach()
        gaussians._features_dc = gaussians._features_dc[mask].detach()
        gaussians._features_rest = gaussians._features_rest[mask].detach()
        gaussians._opacity = gaussians._opacity[mask].detach()
        gaussians._scaling = gaussians._scaling[mask].detach()
        gaussians._rotation = gaussians._rotation[mask].detach()
        gaussians._objects_dc = gaussians._objects_dc[mask].detach()
        
        point_cloud_path = os.path.join(args.model_path, "point_cloud", "iteration_{}".format(scene.loaded_iter))
        gaussians.save_ply(os.path.join(point_cloud_path, "point_cloud.ply"))

    elif args.operation == 'multi-editing':
        for i in range(len(args.operation_list)):
            args.operation = args.operation_list[i]
            args.select_obj_id = args.select_obj_id_list[i]
            args.dst_center = args.parameter_list[i]
            args.euler_angle = args.parameter_list[i]
            gaussians = editing_setup(opt, args.model_path, scene.loaded_iter, scene.getTrainCameras(), gaussians, pipeline, background, scene.cameras_extent, args.removal_thresh, args)

    else:
        gaussians = editing_setup(opt, args.model_path, scene.loaded_iter, scene.getTrainCameras(), gaussians, pipeline, background, scene.cameras_extent, args.removal_thresh, args)

    print(args.model_path)
    scene = Scene(args, gaussians, load_iteration='/iteration_'+str(scene.loaded_iter), shuffle=False, only_gaussians=args.scanrefer)
    if args.scanrefer:
        scene.loaded_iter = f'/iteration_{scene.loaded_iter}'

    with torch.no_grad():

        if args.render_obj != 256:

            mask3d = gaussians._objects_dc[:,0,0] == abs(args.render_obj)
            if not mask3d.any():
                raise ValueError("Non-existent object!")

            if args.render_obj < 0:
                mask3d = ~mask3d

            mask3d = mask3d.bool().squeeze()
            gaussians._xyz = gaussians._xyz[mask3d].detach()
            gaussians._features_dc = gaussians._features_dc[mask3d].detach()
            gaussians._features_rest = gaussians._features_rest[mask3d].detach()
            gaussians._opacity = gaussians._opacity[mask3d].detach()
            gaussians._scaling = gaussians._scaling[mask3d].detach()
            gaussians._rotation = gaussians._rotation[mask3d].detach()
            gaussians._objects_dc = gaussians._objects_dc[mask3d].detach()


        if args.render_ori:
            render_set(args.model_path, "train", scene.loaded_iter, scene.getTrainCameras(), gaussians, pipeline, background, args)

        elif args.render_all:

            if args.scanrefer:
                scene_id = args.model_path.split('/')[1]
                fovfile_path = f'/media/shared_space/data/scannet/scans/{scene_id}/{scene_id}.txt' 
                with open(fovfile_path, 'r') as file:
                    for line in file:
                        if line.startswith('fx_color'):
                            fx_color = float(line.split('=')[1].strip())
                        elif line.startswith('fy_color'):
                            fy_color = float(line.split('=')[1].strip())
                
            else:
                import pickle
                with open('config/cam_info.pkl', 'rb') as f:
                    cam_info_0 = pickle.load(f)
                    fx_color = cam_info_0.FovX
                    fy_color = cam_info_0.FovY
                    
            R_list = [[[1,0,0],[0,0,-1],[0,1,0]], [[1,0,0],[0,0,1],[0,-1,0]], [[0,0,1],[0,1,0],[-1,0,0]], [[0,0,-1],[0,1,0],[1,0,0]], [[1,0,0],[0,1,0],[0,0,1]], [[1,0,0],[0,-1,0],[0,0,-1]]]
            T_list = [[0,0,0], [0,0,0], [0,0,0], [0,0,0], [0,0,0], [0,0,6]] 

            from scene.dataset_readers import CameraInfo    
            from utils.camera_utils import cameraList_from_camInfos
            from utils.graphics_utils import focal2fov

            cam_infos = []
            
            if args.render_angle:
        
                points = gaussians._xyz.cpu().numpy()
                x_min = np.percentile(points[:,0], 1, axis=0)
                x_max = np.percentile(points[:,0], 99, axis=0)
                y_min = np.percentile(points[:,1], 1, axis=0)
                y_max = np.percentile(points[:,1], 99, axis=0)
                z_min = np.percentile(points[:,2], 1, axis=0)
                z_max = np.percentile(points[:,2], 99, axis=0)

                view_point_init = np.array( [(x_min + x_max) / 2, (y_min + y_max) / 2, z_max + (z_max - z_min) * 0.1] )
                view_point_shift =  np.array( [args.render_angle[3], args.render_angle[4], args.render_angle[5]] )
                view_point = view_point_init + view_point_shift
                dst_point = view_point + np.array( [args.render_angle[0], args.render_angle[1], args.render_angle[2]] )

                R = look_at(view_point, dst_point)
                T = np.array(view_point)
                T = -np.linalg.inv(R) @ T

                new_height = 2400
                new_width = 2400
                
                new_fovy = focal2fov(fy_color, new_height)
                new_fovx = focal2fov(fx_color, new_width)
                tmp_img = Image.new('RGB', (int(new_width), int(new_height)), (0, 0, 0))
                cam_info = CameraInfo(5, R, T, new_fovy, new_fovx, \
                            tmp_img, " ", " ", \
                            new_width, new_height, tmp_img)
                cam_infos.append(cam_info)


            else:
                points = gaussians._xyz.cpu().numpy()
                z_avg = np.average(points[:,2], axis=0)

                if args.render_coord is None:
                    x_min = np.percentile(points[:,0], 1, axis=0)
                    x_max = np.percentile(points[:,0], 99, axis=0)
                    y_min = np.percentile(points[:,1], 1, axis=0)
                    y_max = np.percentile(points[:,1], 99, axis=0)

                else:
                    x_min, x_max, y_min, y_max = args.render_coord

                print(x_min, x_max, y_min, y_max, z_avg)

                for i in [5]: 
                
                    pixels_per_unit = 1200 / max((x_max - x_min), (y_max - y_min))
                    new_width = 1200 
                    new_height = 1200 

                    z_cam = z_avg + fx_color / pixels_per_unit
                    T_real = [(x_min + x_max) / 2, (y_min + y_max) / 2, z_cam]

                    R = np.array(R_list[i])
                    T = np.array(T_real)
                    T = -np.linalg.inv(R) @ T

                    print(T_real)

                    new_fovy = focal2fov(fy_color, new_height)
                    new_fovx = focal2fov(fx_color, new_width)
                    tmp_img = Image.new('RGB', (int(new_width), int(new_height)), (0, 0, 0))
                    cam_info = CameraInfo(i, R, T, new_fovy, new_fovx, \
                                tmp_img, " ", " ", \
                                new_width, new_height, tmp_img)
                    cam_infos.append(cam_info)

            views = cameraList_from_camInfos(cam_infos, 1.0, args)

            render_set(args.model_path, "train", scene.loaded_iter, views, gaussians, pipeline, background, args)

        elif args.render_video:

            import pickle
            if args.scanrefer:
                scene_id = args.model_path.split('/')[1]
                fovfile_path = f'/media/shared_space/data/scannet/scans/{scene_id}/{scene_id}.txt' 
                with open(fovfile_path, 'r') as file:
                    for line in file:
                        if line.startswith('fx_color'):
                            fx_color = float(line.split('=')[1].strip())
                        elif line.startswith('fy_color'):
                            fy_color = float(line.split('=')[1].strip())
                
            else:
                with open('config/cam_info.pkl', 'rb') as f:
                    cam_info_0 = pickle.load(f)
                    fx_color = cam_info_0.FovX
                    fy_color = cam_info_0.FovY
                    
            from scene.dataset_readers import CameraInfo    
            from utils.camera_utils import cameraList_from_camInfos
            from utils.graphics_utils import focal2fov

            cam_infos = []
            
            if args.render_coord is None and args.render_angle is None:
                print("Invalid coord/angle!")
                exit(0)

            elif args.render_coord:
                
                x_min, x_max, y_min, y_max = args.render_coord
                
                points = gaussians._xyz.cpu().numpy()
                z_avg = np.average(points[:,2], axis=0)
                
                print(x_min, x_max, y_min, y_max, z_avg)

                new_width = 1200
                new_height = 1200
                pixels_per_unit = min(new_width / (x_max - x_min), new_height / (y_max - y_min))
                
                z_cam = z_avg + fx_color / pixels_per_unit
                T_real = np.array([(x_min + x_max) / 2, (y_min + y_max) / 2, z_cam])

                dis = cal_dis(gaussians._xyz, T_real)
                obj_center = np.array([T_real[0], T_real[1], T_real[2] - dis])

                camera_positions = generate_spiral_path(T_real, dis)
                
                camera_orientations = []

                for pos in camera_positions:
                    camera_orientations.append(look_at(pos, obj_center))
                
                camera_orientations = np.array(camera_orientations)

            elif args.render_angle:

                new_width = 2400
                new_height = 2400

                points = gaussians._xyz.cpu().numpy()
                x_min = np.percentile(points[:,0], 1, axis=0)
                x_max = np.percentile(points[:,0], 99, axis=0)
                y_min = np.percentile(points[:,1], 1, axis=0)
                y_max = np.percentile(points[:,1], 99, axis=0)
                z_min = np.percentile(points[:,2], 1, axis=0)
                z_max = np.percentile(points[:,2], 99, axis=0)

                view_point_init = np.array( [(x_min + x_max) / 2, (y_min + y_max) / 2, z_max + (z_max - z_min) * 0.1] )
                view_point_shift =  np.array( [args.render_angle[3], args.render_angle[4], args.render_angle[5]] )
                view_point = view_point_init + view_point_shift
                direction = np.array( [args.render_angle[0], args.render_angle[1], args.render_angle[2]] )
                dst_point = view_point + direction

                obj_center = find_first_intersection(view_point, direction, gaussians._xyz.detach().cpu().numpy())

                camera_positions = generate_circle_path(view_point, obj_center)

                camera_orientations = []

                for pos in camera_positions:
                    camera_orientations.append(look_at(pos, obj_center))
                camera_orientations[0] = look_at(view_point, dst_point) # nt
                
                camera_orientations = np.array(camera_orientations)

            for i in range(len(camera_positions)):

                R = np.array(camera_orientations[i])
                T = np.array(camera_positions[i])
                T = -np.linalg.inv(R) @ T

                new_fovy = focal2fov(fy_color, new_height)
                new_fovx = focal2fov(fx_color, new_width)
                tmp_img = Image.new('RGB', (int(new_width), int(new_height)), (0, 0, 0))
                cam_info = CameraInfo(i, R, T, new_fovy, new_fovx, \
                            tmp_img, " ", " ", \
                            new_width, new_height, tmp_img)
                cam_infos.append(cam_info)

            camera_path = os.path.join(args.model_path, "train", "ours{}".format(scene.loaded_iter), 'cam_infos.pkl')
            with open(camera_path, 'wb') as f:
                pickle.dump(cam_infos, f)

            views = cameraList_from_camInfos(cam_infos, 1.0, args)

            render_set(args.model_path, "train", scene.loaded_iter, views, gaussians, pipeline, background, args)

        elif args.render_obj_theta:

            import pickle
            with open('config/cam_info.pkl', 'rb') as f:
                cam_info_0 = pickle.load(f)
            
            from scene.dataset_readers import CameraInfo    
            from utils.camera_utils import cameraList_from_camInfos
            from utils.graphics_utils import focal2fov

            mask3d = torch.ones(gaussians._xyz.shape[0], dtype=torch.bool)
            bb = {}
            get_bb(gaussians._xyz.detach(), torch.tensor, bb)


            new_width = cam_info_0.height + 60
            new_height = cam_info_0.height + 40
            new_fovy = focal2fov(cam_info_0.FovY, new_height)
            new_fovx = focal2fov(cam_info_0.FovX, new_width)

            R_list = []
            T_list = []

            for theta in args.render_obj_theta:
                R, T, r = compute_camera_extrinsics(bb['centroid'], bb['size'], new_fovx, new_fovy, theta / 180 * np.pi)
                R_list.append(R)
                T_list.append(T)

            cam_infos = []
            
            for i in range(len(R_list)):

                cam_info = CameraInfo(i, np.array(R_list[i]), np.array(T_list[i]), new_fovy, new_fovx, \
                            crop_image_to_size(cam_info_0.image, new_width, new_height), cam_info_0.image_path, cam_info_0.image_name+f'{i}', \
                            new_width, new_height, crop_image_to_size(cam_info_0.objects, new_width, new_height))
                cam_infos.append(cam_info)

            views = cameraList_from_camInfos(cam_infos, 1.0, args)


            render_set(args.model_path, "train", scene.loaded_iter, views, gaussians, pipeline, background, args)


def get_bounding_box(args):
    gaussians = GaussianModel(args.sh_degree)
    scene = Scene(args, gaussians, load_iteration=args.iteration, shuffle=False,only_gaussians=args.scanrefer)
    num_classes = args.num_classes
    print("Num classes: ",num_classes)
    bg_color = [1,1,1] if args.white_background else [0, 0, 0]
    background = torch.tensor(bg_color, dtype=torch.float32, device="cuda")

    bb_list = []

    for obj_id in range(1, 256):

        with torch.no_grad():
            mask3d = gaussians._objects_dc[:,0,0] == obj_id
            if torch.count_nonzero(mask3d) < 5:
                break
            mask3d = mask3d.float()[:,None,None]
            
        bb = {}
        bb["id"] = obj_id
        get_bb(gaussians._xyz.detach(), mask3d, bb)
        bb_list.append(bb)
        
    output = {}
    output["Scene configurations"] = bb_list

    with open(args.model_path + '/bounding_boxes.json', 'w') as f:  
        json.dump(output, f, indent=4) 
    
    print("Bounding box saved.")


if __name__ == "__main__":
    parser = ArgumentParser(description="Testing script parameters")
    model = ModelParams(parser, sentinel=True)
    opt = OptimizationParams(parser)
    pipeline = PipelineParams(parser)
    parser.add_argument("--iteration", default=-1, type=int)
    parser.add_argument("--skip_train", action="store_true")
    parser.add_argument("--quiet", action="store_true")

    parser.add_argument("--config_file", type=str, help="Path to the configuration file")
    parser.add_argument("--operation", type=str, default="skip", help="removal/translate/rotate/skip/crop/multi-editing")
    parser.add_argument("--crop_coord", type=float, nargs='+')
    parser.add_argument("--select_obj_id", type=int, nargs='+')
    parser.add_argument("--dst_center", type=float, nargs='+')
    parser.add_argument("--euler_angle", type=float, nargs='+')

    parser.add_argument("--render_obj", type=int, default=256)

    parser.add_argument("--render_ori", action="store_true")
    parser.add_argument("--render_all", action="store_true")
    parser.add_argument("--render_video", action="store_true")
    parser.add_argument("--render_coord", type=float, nargs='+', default=None)
    parser.add_argument("--render_angle", type=float, nargs='+', default=None)
    parser.add_argument("--render_obj_theta", type=float, nargs='+', default=None)

    parser.add_argument("--render_highlights", type=int, nargs='+')
    parser.add_argument("--render_labels", action="store_true")

    parser.add_argument("--get_bbox_2d", type=float, nargs='+', default=None)
    parser.add_argument("--get_bounding_box", action="store_true")

    parser.add_argument("--scanrefer", action="store_true")

    args = parser.parse_args()
    # print(args)
    
    if not args.scanrefer:
        args = get_combined_args(parser)
    else:
        args.sh_degree = 0
        args.iteration = 1
        args.resolution = 1
        args.data_device="cuda"

    print("Rendering " + args.model_path)

    args.num_classes = 256
    args.removal_thresh = 0.3

    if hasattr(args, 'select_obj_id') and args.select_obj_id != None:
        args.select_obj_id = [ [args.select_obj_id[i]] for i in range(len(args.select_obj_id)) ]


    if args.operation == 'multi-editing':

        try:
            with open(args.config_file, 'r') as file:
                config = json.load(file)
        except FileNotFoundError:
            print(f"Error: Configuration file '{args.config_file}' not found.")
            exit(1)
        except json.JSONDecodeError as e:
            print(f"Error: Failed to parse the JSON configuration file: {e}")
            exit(1)

        args.operation_list = config.get("operation_list", ["removal"])
        args.select_obj_id_list = config.get("select_obj_id_list", [1])
        args.parameter_list = config.get("parameter_list", [[]])

    safe_state(args.quiet)

    if args.get_bounding_box:
        get_bounding_box(args)
    else:
        editing(args)


