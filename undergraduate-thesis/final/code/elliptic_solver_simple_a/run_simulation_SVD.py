# run_simulation.py - 主运行脚本（修改版）
import os
import sys
import numpy as np

sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from user_config import *
from pde_solver import PDESolver
from model_reduction import ModelReductionSVD
from svd_analysis import analyze_svd_results, compare_solutions
from ngsolve import Mesh
from netgen.geom2d import unit_square

def main():
    print("=" * 60)
    print("基于SVD的PDE模型降阶方法")
    print("=" * 60)
    
    # 创建网格
    print("[1/5] 生成网格...")
    mesh = Mesh(unit_square.GenerateMesh(maxh=MESH_H))
    
    if RUN_SVD_ANALYSIS:
        # 模型降阶流程
        print("[2/5] 初始化模型降阶...")
        rom = ModelReductionSVD(mesh, order=ORDER)
        
        # 生成训练数据
        print("[3/5] 生成训练数据...")
        a_samples, snapshot_matrix = rom.generate_training_data(
            A_MIN, A_MAX, NUM_TRAINING_SAMPLES, f)
        
        # 执行SVD
        print("[4/5] 执行SVD分解...")
        U, S, VT, rank = rom.perform_svd(ENERGY_THRESHOLD)
        
        # 分析结果
        analyze_svd_results(S, rank, ENERGY_THRESHOLD)
        
        # 测试降阶模型
        print("[5/5] 测试降阶模型...")
        test_a = (A_MIN + A_MAX) / 2  # 测试参数
        u_reduced, u_full = rom.solve_reduced(test_a, f)
        
        # 比较结果
        max_err, norm_err = compare_solutions(
            u_full, u_reduced, rom.reduced_basis, 
            f"测试参数 a={test_a:.3f}"
        )
        
        print(f"\n降阶模型测试结果:")
        print(f"  测试参数 a = {test_a:.3f}")
        print(f"  降阶维度: {u_reduced.shape[0]}")
        print(f"  全阶维度: {u_full.shape[0]}")
        print(f"  最大绝对误差: {max_err:.2e}")
        print(f"  L2误差: {norm_err:.2e}")
        print(f"  压缩比: {u_full.shape[0]/u_reduced.shape[0]:.2f}x")
    
    else:
        # 传统单次求解
        print("[2/5] 传统单次求解...")
        solver = PDESolver(mesh, ORDER)
        u = solver.solve(a, f)
        print(f"求解完成，自由度: {solver.get_dofs()}")
    
    print("\n" + "=" * 60)
    print("程序执行完成！")

if __name__ == "__main__":
    main()