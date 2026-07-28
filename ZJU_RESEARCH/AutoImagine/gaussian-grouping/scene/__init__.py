#
# Copyright (C) 2023, Inria
# GRAPHDECO research group, https://team.inria.fr/graphdeco
# All rights reserved.
#
# This software is free for non-commercial, research and evaluation use 
# under the terms of the LICENSE.md file.
#
# For inquiries contact  george.drettakis@inria.fr
#

import os
import random
import json
from utils.system_utils import searchForMaxIteration
from scene.dataset_readers import sceneLoadTypeCallbacks
from scene.gaussian_model import GaussianModel
from arguments import ModelParams
from utils.camera_utils import cameraList_from_camInfos, camera_to_JSON
from PIL import Image
import numpy as np
from scene.dataset_readers import CameraInfo, getNerfppNorm
from utils.graphics_utils import focal2fov

class Scene:

    gaussians : GaussianModel

    def __init__(self, args : ModelParams, gaussians : GaussianModel, load_iteration=None, shuffle=True, resolution_scales=[1.0], only_gaussians=False):
        """b
        :param path: Path to colmap scene main folder.
        """
        self.model_path = args.model_path
        self.loaded_iter = None
        self.gaussians = gaussians

        # --- mtd ---
        only_gaussians = True # 在我的实验中，我们不从头生成，而是使用预生成的 gaussian 模型，来进行 editing
        # --- mtd ---

        if only_gaussians:

            self.loaded_iter = 1
            # self.loaded_iter = load_iteration
            # --- mtd ---
            self.loaded_iter = load_iteration # 我们这里用 load_iteration

            # import pdb; pdb.set_trace()
            # --- mtd ---
            if isinstance(self.loaded_iter,str):
                ply_path = os.path.join(self.model_path, "point_cloud" + self.loaded_iter, "point_cloud.ply")
            else:
                ply_path = os.path.join(self.model_path, "point_cloud", "iteration_" + str(self.loaded_iter), "point_cloud.ply")
  
            # print(self.model_path)
            # print(self.loaded_iter)
            # print(isinstance(self.loaded_iter,str))
            # if isinstance(self.loaded_iter,str):
            #     print(os.path.join(self.model_path, "point_cloud", self.loaded_iter, "point_cloud.ply"))
            # print(ply_path)

            # if os.path.exists(ply_path) and False:
            if os.path.exists(ply_path):
                # print(123456)
                self.gaussians.load_ply(ply_path)
            else:
                import open3d as o3d
                scene_id = self.model_path.split('/')[1]
                file_path = f'/media/shared_space/data/scannet/scans/{scene_id}/{scene_id}_vh_clean.ply'  # DO NOT UST NT xxx_clean_2.ply !
                pcd = o3d.io.read_point_cloud(file_path)
            
                self.gaussians.create_from_pcd(pcd, 1.0)

            # --- mtd ---
            # ------------------ 新增：尝试读取摄像机信息 ------------------
            self.train_cameras = {}
            self.test_cameras = {}

            cam_infos = []
            cam_json_path = os.path.join(self.model_path, "cameras.json")
            if os.path.exists(cam_json_path):
                with open(cam_json_path, 'r') as f:
                    json_cams = json.load(f)
                for entry in json_cams:
                    try:
                        width  = entry.get('width', 800)
                        height = entry.get('height', 600)
                        rot    = np.array(entry['rotation'])  # shape (3,3)
                        pos    = np.array(entry['position'])  # shape (3,)
                        # 将 camera->world 转为 world->camera
                        R_wc = rot.T
                        T_wc = -R_wc @ pos
                        fy = entry.get('fy', 500.0)
                        fx = entry.get('fx', 500.0)
                        FovY = focal2fov(fy, height)
                        FovX = focal2fov(fx,  width)
                        blank_img = Image.new('RGB', (width, height))
                        dummy_objects = np.empty((0,), dtype=np.float32)
                        cam_infos.append(CameraInfo(uid=entry['id'], R=R_wc, T=T_wc,
                                                     FovY=FovY, FovX=FovX, image=blank_img,
                                                     image_path='', image_name=str(entry['id']),
                                                     width=width, height=height, objects=dummy_objects))
                    except Exception as e:
                        print(f"[Scene] Warning: skip camera entry due to {e}")
            else:
                # fallback：基于 correct.json 生成一个顶视虚拟相机
                correct_json_path = os.path.join(self.model_path, "correct.json")
                if os.path.exists(correct_json_path):
                    width, height = 800, 800
                    # 顶视方向：Z 轴朝相机，Y 轴向下
                    R_cw = np.array([[1, 0, 0],
                                     [0, 0, -1],
                                     [0, 1, 0]])  # camera->world
                    pos = np.array([0.0, 0.0, 10.0])
                    R_wc = R_cw.T
                    T_wc = -R_wc @ pos
                    FovX = FovY = np.radians(60.0)
                    blank_img = Image.new('RGB', (width, height))
                    dummy_objects = np.empty((0,), dtype=np.float32)
                    cam_infos.append(CameraInfo(uid=0, R=R_wc, T=T_wc, FovY=FovY,
                                                 FovX=FovX, image=blank_img, image_path='',
                                                 image_name='virtual_top', width=width,
                                                 height=height, objects=dummy_objects))

            # 若仍然没有可用相机，则继续但给出提示
            if not cam_infos:
                print("[Scene] Warning: no camera information found; train/test camera lists will be empty.")
                self.cameras_extent = 1.0
                for resolution_scale in resolution_scales:
                    self.train_cameras[resolution_scale] = []
                    self.test_cameras[resolution_scale] = []
            else:
                # 计算 nerf 归一化半径
                nerf_norm = getNerfppNorm(cam_infos)
                self.cameras_extent = nerf_norm["radius"]
                for resolution_scale in resolution_scales:
                    self.train_cameras[resolution_scale] = cameraList_from_camInfos(cam_infos, resolution_scale, args)
                    self.test_cameras[resolution_scale] = []
            # ------------------ 新增结束 ------------------
            # --- mtd ---

            # 保证高斯模型已加载
            if os.path.exists(ply_path):
                pass  # 前面已 load_ply
            else:
                # 若之前 create_from_pcd 已进行，这里不重复
                pass

            # 注意：不要提前 return，后续代码可能还需使用 self.train_cameras
            return

        if load_iteration:
            if load_iteration == -1:
                self.loaded_iter = searchForMaxIteration(os.path.join(self.model_path, "point_cloud"))
            else:
                self.loaded_iter = load_iteration
            print("Loading trained model at iteration {}".format(self.loaded_iter))

        self.train_cameras = {}
        self.test_cameras = {}

        # print(os.path.join(args.source_path, "sparse"))

        if os.path.exists(os.path.join(args.source_path, "sparse")):
            scene_info = sceneLoadTypeCallbacks["Colmap"](args.source_path, args.images, args.eval, args.object_path, n_views=args.n_views, random_init=args.random_init, train_split=args.train_split)
        elif os.path.exists(os.path.join(args.source_path, "transforms_train.json")):
            print("Found transforms_train.json file, assuming Blender data set!")
            scene_info = sceneLoadTypeCallbacks["Blender"](args.source_path, args.white_background, args.eval)
        else:
            assert False, "Could not recognize scene type!"

        if not self.loaded_iter:
            with open(scene_info.ply_path, 'rb') as src_file, open(os.path.join(self.model_path, "input.ply") , 'wb') as dest_file:
                dest_file.write(src_file.read())
            json_cams = []
            camlist = []
            if scene_info.test_cameras:
                camlist.extend(scene_info.test_cameras)
            if scene_info.train_cameras:
                camlist.extend(scene_info.train_cameras)
            for id, cam in enumerate(camlist):
                json_cams.append(camera_to_JSON(id, cam))
            with open(os.path.join(self.model_path, "cameras.json"), 'w') as file:
                json.dump(json_cams, file)

        if shuffle:
            random.shuffle(scene_info.train_cameras)  # Multi-res consistent random shuffling
            random.shuffle(scene_info.test_cameras)  # Multi-res consistent random shuffling

        self.cameras_extent = scene_info.nerf_normalization["radius"]

        for resolution_scale in resolution_scales:
            print("Loading Training Cameras")
            self.train_cameras[resolution_scale] = cameraList_from_camInfos(scene_info.train_cameras, resolution_scale, args)
            print("Loading Test Cameras")
            self.test_cameras[resolution_scale] = cameraList_from_camInfos(scene_info.test_cameras, resolution_scale, args)

        if self.loaded_iter:
            if isinstance(self.loaded_iter,str):
                print("edit load path", self.loaded_iter)
                self.gaussians.load_ply(os.path.join(self.model_path,
                                                            "point_cloud"+self.loaded_iter,
                                                            "point_cloud.ply"))
            else:
                self.gaussians.load_ply(os.path.join(self.model_path,
                                                            "point_cloud",
                                                            "iteration_" + str(self.loaded_iter),
                                                            "point_cloud.ply"))
        else:
            self.gaussians.create_from_pcd(scene_info.point_cloud, self.cameras_extent)

    def save(self, iteration):
        point_cloud_path = os.path.join(self.model_path, "point_cloud/iteration_{}".format(iteration))
        self.gaussians.save_ply(os.path.join(point_cloud_path, "point_cloud.ply"))

    def getTrainCameras(self, scale=1.0):
        return self.train_cameras[scale]

    def getTestCameras(self, scale=1.0):
        return self.test_cameras[scale] 