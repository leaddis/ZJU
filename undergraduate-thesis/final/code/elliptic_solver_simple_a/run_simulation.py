# run_simulation.py
"""
用户运行脚本 - 只需运行这个文件即可
"""

import os
import sys

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from user_config import *
from main import EllipticSolver, plot_solution_plots, convergence_study
from ngsolve import Mesh
from netgen.geom2d import unit_square

def main():
    print("=" * 60)
    print("二维椭圆方程求解器")
    print("=" * 60)
    
    # 显示问题信息
    print(f"问题类型: {PROBLEM_TYPE}")
    print(f"有限元阶数: {ORDER}")
    print(f"网格尺寸: {MESH_H}")
    print(f"扩散系数 a: {a}")
    print(f"源项 f: {f}")
    print(f"精确解已知: {exact_solution is not None}")
    print("-" * 60)
    
    try:
        # 创建网格
        print("[1/4] 生成网格...")
        mesh = Mesh(unit_square.GenerateMesh(maxh=MESH_H))
        
        # 创建求解器
        print("[2/4] 设置求解器...")
        solver = EllipticSolver(mesh, order=ORDER)
        solver.setup_fem_space()
        solver.set_problem(a_coef=a, f_coef=f, exact_solution=exact_solution)
        
        # 求解方程
        print("[3/4] 求解方程...")
        solver.assemble_and_solve()
        l2err, h1err = solver.compute_errors()
        
        # 可视化
        print("[4/4] 生成可视化结果...")
        if PLOT_SOLUTION:
            plot_solution_plots(solver, mesh, exact_solution=exact_solution, 
                              l2_error=(float(l2err) if l2err is not None else None))
        
        # 收敛性分析
        if RUN_CONVERGENCE_STUDY and exact_solution is not None:
            print("\n运行收敛性分析...")
            convergence_study(exact_solution, a, f, 
                            orders=CONV_ORDERS, 
                            mesh_sizes=CONV_MESH_SIZES)
        
        print("\n" + "=" * 60)
        print("求解完成！")
        print(f"结果保存在 'plots/' 目录中")
        
        # 显示关键结果
        if l2err is not None:
            print(f"L2误差: {l2err:.2e}")
        if h1err is not None:
            print(f"H1误差: {h1err:.2e}")
            
        # 显示解在关键点的值
        points = [(0.5, 0.5), (0.25, 0.25), (0.75, 0.75)]
        print("\n解在关键点的值:")
        for p in points:
            val = solver.get_value_at(p[0], p[1])
            print(f"  u({p[0]}, {p[1]}) = {val:.6e}")
            
    except Exception as e:
        print(f"\n错误: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    success = main()
    if success:
        print("\n🎉 成功完成！要尝试其他问题，请编辑 'user_config.py'")
    else:
        print("\n❌ 运行失败，请检查错误信息")