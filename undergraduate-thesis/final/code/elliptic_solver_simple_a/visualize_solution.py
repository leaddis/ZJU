# visualize_solution.py
import matplotlib.pyplot as plt
import numpy as np
from elliptic_solver import EllipticSolver, create_mesh
from ngsolve import x, y, GridFunction
import os

# 确保plot目录存在
PLOT_DIR = "plots"
if not os.path.exists(PLOT_DIR):
    os.makedirs(PLOT_DIR)

def solve_and_visualize():
    """求解并可视化椭圆方程的解"""
    print("=== 椭圆方程求解与可视化 ===\n")
    
    # 创建网格和求解器
    mesh = create_mesh(maxh=0.1)
    solver = EllipticSolver(mesh, order=2)
    solver.setup_fem_space()
    
    # 使用已知精确解
    exact_solution = x*(1-x)*y*(1-y)
    f = 2*(y*(1-y) + x*(1-x))
    
    solver.define_coefficients(a_coef=1.0, f_coef=f, exact_solution=exact_solution)
    solver.setup_variational_forms()
    solution = solver.solve()
    
    # 计算误差
    l2_error, h1_error = solver.compute_error()
    
    # 可视化解
    visualize_with_matplotlib(solver, mesh, "finite_element_solution", l2_error)
    
    # 比较精确解和数值解
    compare_solutions(solver, exact_solution, mesh, l2_error)
    
    return solver, l2_error

def visualize_with_matplotlib(solver, mesh, title, l2_error=None):
    """使用matplotlib可视化解"""
    print("使用matplotlib进行可视化...")
    
    # 创建采样网格
    nx, ny = 100, 100
    x_vals = np.linspace(0.01, 0.99, nx)  # 避免边界
    y_vals = np.linspace(0.01, 0.99, ny)
    X, Y = np.meshgrid(x_vals, y_vals)
    Z = np.zeros((ny, nx))
    
    # 在网格点上采样数值解
    for i in range(ny):
        for j in range(nx):
            try:
                Z[i, j] = solver.get_solution_value(x_vals[j], y_vals[i])
            except:
                Z[i, j] = 0
    
    # 创建图形 - 只使用2D绘图
    plt.figure(figsize=(15, 5))
    
    # 子图1: 等高线图
    plt.subplot(1, 3, 1)
    contour = plt.contourf(X, Y, Z, levels=50, cmap='viridis')
    plt.colorbar(contour, label='Solution Value')
    plt.xlabel('x')
    plt.ylabel('y')
    if l2_error is not None:
        plt.title(f'Finite Element Solution\nL2 Error: {l2_error:.2e}')
    else:
        plt.title('Finite Element Solution')
    plt.axis('equal')
    
    # 子图2: 热力图
    plt.subplot(1, 3, 2)
    im = plt.imshow(Z, extent=[0, 1, 0, 1], origin='lower', cmap='plasma', aspect='auto')
    plt.colorbar(im, label='Solution Value')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Heatmap View')
    
    # 子图3: 沿对角线的剖面
    plt.subplot(1, 3, 3)
    diagonal = np.diag(Z)
    plt.plot(x_vals, diagonal, 'b-', linewidth=2)
    plt.xlabel('x (along diagonal y=x)')
    plt.ylabel('Solution Value')
    plt.title('Solution along Diagonal')
    plt.grid(True)
    
    plt.tight_layout()
    
    # 保存到plot文件夹
    plot_path = os.path.join(PLOT_DIR, f'{title}.png')
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"图形已保存为 {plot_path}")

def evaluate_exact_solution(exact_solution, mesh, x_val, y_val):
    """安全地计算精确解在给定点的值"""
    try:
        # 创建精确解的网格函数
        fes = mesh.space
        exact_gf = GridFunction(fes)
        exact_gf.Set(exact_solution)
        
        # 在给定点求值
        point = (x_val, y_val)
        val = exact_gf(mesh(point[0], point[1]))
        
        # 确保返回标量
        if hasattr(val, '__len__') and len(val) > 0:
            return float(val[0])
        else:
            return float(val)
    except:
        # 如果失败，尝试直接计算
        try:
            # 创建一个临时网格函数来求值
            from ngsolve import H1
            fes_temp = H1(mesh, order=2)
            exact_gf = GridFunction(fes_temp)
            exact_gf.Set(exact_solution)
            
            point = (x_val, y_val)
            val = exact_gf(mesh(point[0], point[1]))
            
            if hasattr(val, '__len__') and len(val) > 0:
                return float(val[0])
            else:
                return float(val)
        except:
            return 0.0

def compare_solutions(solver, exact_solution, mesh, l2_error=None):
    """比较精确解和数值解"""
    print("\n=== 解的比较 ===")
    
    # 创建采样网格
    nx, ny = 50, 50
    x_vals = np.linspace(0.1, 0.9, nx)
    y_vals = np.linspace(0.1, 0.9, ny)
    X, Y = np.meshgrid(x_vals, y_vals)
    
    # 计算精确解和数值解
    Z_exact = np.zeros((ny, nx))
    Z_numeric = np.zeros((ny, nx))
    Z_error = np.zeros((ny, nx))
    
    for i in range(ny):
        for j in range(nx):
            x_val, y_val = x_vals[j], y_vals[i]
            # 使用安全的方法计算精确解
            Z_exact[i, j] = evaluate_exact_solution(exact_solution, mesh, x_val, y_val)
            Z_numeric[i, j] = solver.get_solution_value(x_val, y_val)
            Z_error[i, j] = abs(Z_exact[i, j] - Z_numeric[i, j])
    
    # 创建比较图形
    plt.figure(figsize=(15, 5))
    
    # 子图1: 精确解
    plt.subplot(1, 3, 1)
    contour1 = plt.contourf(X, Y, Z_exact, levels=50, cmap='viridis')
    plt.colorbar(contour1, label='Exact Solution')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Exact Solution: u = x(1-x)y(1-y)')
    plt.axis('equal')
    
    # 子图2: 数值解
    plt.subplot(1, 3, 2)
    contour2 = plt.contourf(X, Y, Z_numeric, levels=50, cmap='viridis')
    plt.colorbar(contour2, label='Numerical Solution')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Numerical Solution')
    plt.axis('equal')
    
    # 子图3: 误差
    plt.subplot(1, 3, 3)
    contour3 = plt.contourf(X, Y, Z_error, levels=50, cmap='hot')
    plt.colorbar(contour3, label='Absolute Error')
    plt.xlabel('x')
    plt.ylabel('y')
    if l2_error is not None:
        plt.title(f'Absolute Error\nL2 Error: {l2_error:.2e}')
    else:
        plt.title('Absolute Error')
    plt.axis('equal')
    
    plt.tight_layout()
    
    # 保存到plot文件夹
    plot_path = os.path.join(PLOT_DIR, 'solution_comparison.png')
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"比较图形已保存为 {plot_path}")
    
    # 打印一些统计信息
    max_error = np.max(Z_error)
    mean_error = np.mean(Z_error)
    print(f"最大绝对误差: {max_error:.2e}")
    print(f"平均绝对误差: {mean_error:.2e}")
    if l2_error is not None:
        print(f"L2误差: {l2_error:.2e}")

def convergence_visualization():
    """收敛性可视化"""
    print("\n=== 收敛性可视化 ===")
    
    mesh_sizes = [0.2, 0.1, 0.05]
    orders = [1, 2]
    l2_errors = {1: [], 2: []}
    
    for order in orders:
        print(f"计算多项式阶数 {order} 的收敛性...")
        for h in mesh_sizes:
            mesh = create_mesh(maxh=h)
            solver = EllipticSolver(mesh, order=order)
            solver.setup_fem_space()
            
            exact_solution = x*(1-x)*y*(1-y)
            f = 2*(y*(1-y) + x*(1-x))
            
            solver.define_coefficients(a_coef=1.0, f_coef=f, exact_solution=exact_solution)
            solver.setup_variational_forms()
            solver.solve()
            
            l2_error, h1_error = solver.compute_error()
            if l2_error is not None:
                l2_errors[order].append(l2_error)
                print(f"  网格尺寸 {h}: L2误差 = {l2_error:.2e}")
    
    # 绘制收敛曲线
    plt.figure(figsize=(10, 6))
    
    for order in orders:
        if len(l2_errors[order]) > 0:
            plt.loglog(mesh_sizes[:len(l2_errors[order])], l2_errors[order], 
                      'o-', linewidth=2, markersize=8, label=f'L2 Error (p={order})')
    
    # 添加理论收敛率参考线
    if len(l2_errors[1]) > 0:
        ref_line1 = [l2_errors[1][0] * (h/mesh_sizes[0])**2 for h in mesh_sizes]
        plt.loglog(mesh_sizes, ref_line1, 'k:', label='O(h²)', alpha=0.7)
    
    if len(l2_errors[2]) > 0:
        ref_line2 = [l2_errors[2][0] * (h/mesh_sizes[0])**3 for h in mesh_sizes]
        plt.loglog(mesh_sizes, ref_line2, 'k--', label='O(h³)', alpha=0.7)
    
    plt.xlabel('Mesh Size h')
    plt.ylabel('L2 Error')
    plt.title('Convergence Analysis')
    plt.legend()
    plt.grid(True, which='both', linestyle='--', alpha=0.7)
    
    # 保存到plot文件夹
    plot_path = os.path.join(PLOT_DIR, 'convergence_analysis.png')
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    plt.show()
    
    # 计算并显示收敛率
    for order in orders:
        if len(l2_errors[order]) > 1:
            rates = []
            for i in range(1, len(l2_errors[order])):
                rate = np.log(l2_errors[order][i-1]/l2_errors[order][i]) / np.log(mesh_sizes[i-1]/mesh_sizes[i])
                rates.append(rate)
            avg_rate = np.mean(rates)
            print(f"阶数 {order} 的平均收敛率: {avg_rate:.3f}")
    
    print(f"收敛性分析图已保存为 {plot_path}")

def plot_solution_profile(solver, exact_solution=None, mesh=None):
    """绘制解的剖面图"""
    print("\n=== 解的剖面图 ===")
    
    # 创建x轴上的点
    x_vals = np.linspace(0.01, 0.99, 100)
    
    plt.figure(figsize=(12, 5))
    
    # 子图1: 在y=0.5处的剖面
    plt.subplot(1, 2, 1)
    y_fixed = 0.5
    numeric_vals = [solver.get_solution_value(x, y_fixed) for x in x_vals]
    plt.plot(x_vals, numeric_vals, 'b-', linewidth=2, label='Numerical')
    
    if exact_solution is not None and mesh is not None:
        exact_vals = [evaluate_exact_solution(exact_solution, mesh, x, y_fixed) for x in x_vals]
        plt.plot(x_vals, exact_vals, 'r--', linewidth=2, label='Exact')
    
    plt.xlabel('x')
    plt.ylabel(f'Solution at y={y_fixed}')
    plt.title(f'Solution Profile at y={y_fixed}')
    plt.legend()
    plt.grid(True)
    
    # 子图2: 在x=0.5处的剖面
    plt.subplot(1, 2, 2)
    x_fixed = 0.5
    y_vals = np.linspace(0.01, 0.99, 100)
    numeric_vals = [solver.get_solution_value(x_fixed, y) for y in y_vals]
    plt.plot(y_vals, numeric_vals, 'b-', linewidth=2, label='Numerical')
    
    if exact_solution is not None and mesh is not None:
        exact_vals = [evaluate_exact_solution(exact_solution, mesh, x_fixed, y) for y in y_vals]
        plt.plot(y_vals, exact_vals, 'r--', linewidth=2, label='Exact')
    
    plt.xlabel('y')
    plt.ylabel(f'Solution at x={x_fixed}')
    plt.title(f'Solution Profile at x={x_fixed}')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    
    # 保存到plot文件夹
    plot_path = os.path.join(PLOT_DIR, 'solution_profiles.png')
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"剖面图已保存为 {plot_path}")

if __name__ == "__main__":
    try:
        # 求解并可视化
        solver, l2_error = solve_and_visualize()
        
        # 绘制解的剖面图
        exact_solution = x*(1-x)*y*(1-y)
        plot_solution_profile(solver, exact_solution, solver.mesh)
        
        # 收敛性可视化
        convergence_visualization()
        
        print("\n=== 所有可视化完成 ===")
        print(f"生成的图片文件保存在 {PLOT_DIR} 文件夹中:")
        print(f"- {PLOT_DIR}/finite_element_solution.png")
        print(f"- {PLOT_DIR}/solution_comparison.png") 
        print(f"- {PLOT_DIR}/solution_profiles.png")
        print(f"- {PLOT_DIR}/convergence_analysis.png")
        
    except Exception as e:
        print(f"运行过程中出现错误: {e}")
        import traceback
        traceback.print_exc()