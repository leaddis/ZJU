#!/usr/bin/env python3
"""
solve_and_plot.py

统一的二维椭圆方程求解与可视化脚本：
    - 方程: -∇·( a(x,y) ∇u ) = f(x,y) in Ω = [0,1]^2, u = 0 on ∂Ω
    - 可配置 a(x,y), f(x,y), exact_solution (可选用于误差计算)
    - 自动求解、打印误差、保存并显示多种图像

依赖:
    - NGSolve / Netgen (ngsolve, netgen.geom2d)
    - numpy, matplotlib
运行:
    python solve_and_plot.py

编辑位置:
    在脚本中搜索 "USER: modify here" 来修改 a, f, exact_solution, order, mesh size 等
"""

import os
import numpy as np
import matplotlib.pyplot as plt

# --- NGSolve imports (must have NGSolve installed) ---
try:
    from ngsolve import *
    from netgen.geom2d import unit_square
except Exception as e:
    raise ImportError("需要安装 NGSolve/Netgen: 无法导入 ngsolve 或 netgen. 原始错误: " + str(e))

# ------------------ Helper / Solver class ------------------
class EllipticSolver:
    def __init__(self, mesh, order=2):
        self.mesh = mesh
        self.order = order
        self.fes = None
        self.gfu = None
        self.a_coef = None
        self.f_coef = None
        self.exact_solution = None

    def setup_fem_space(self):
        # Dirichlet on the whole boundary
        self.fes = H1(self.mesh, order=self.order, dirichlet=".*")
        print(f"[INFO] Finite element space: H1, order={self.order}, ndof={self.fes.ndof}")

    def set_problem(self, a_coef, f_coef, exact_solution=None):
        """
        a_coef, f_coef: 可以是常数、ngsolve.CoefficientFunction（例如 1.0, x*(1-x) 等）
        exact_solution: 若有，则用于误差计算（也可为 None）
        """
        self.a_coef = a_coef
        self.f_coef = f_coef
        self.exact_solution = exact_solution
        print(f"[INFO] Problem set: a = {a_coef}, f = {f_coef}, exact_solution provided: {exact_solution is not None}")

    def assemble_and_solve(self):
        if self.fes is None:
            raise RuntimeError("Finite element space 未设置，请先调用 setup_fem_space()")

        u = self.fes.TrialFunction()
        v = self.fes.TestFunction()

        a_form = BilinearForm(self.fes, symmetric=True)
        a_form += self.a_coef * grad(u) * grad(v) * dx

        l_form = LinearForm(self.fes)
        l_form += self.f_coef * v * dx

        a_form.Assemble()
        l_form.Assemble()

        self.gfu = GridFunction(self.fes)
        freedofs = self.fes.FreeDofs()
        inv = a_form.mat.Inverse(freedofs, inverse="sparsecholesky")
        self.gfu.vec.data = inv * l_form.vec

        print("[INFO] Solve complete.")
        self._report_solution_stats()
        return self.gfu

    def _report_solution_stats(self):
        try:
            l2norm = sqrt(Integrate(self.gfu**2, self.mesh))
            print(f"[INFO] Numerical solution L2 norm = {l2norm:.6e}")
        except Exception as e:
            print(f"[WARN] 无法计算 L2 范数: {e}")

        # sample some points
        pts = [(0.5,0.5),(0.25,0.25),(0.75,0.75)]
        for p in pts:
            try:
                val = self.get_value_at(p[0], p[1])
                print(f"  u({p[0]:.3f},{p[1]:.3f}) = {val:.6e}")
            except Exception:
                pass

    def get_value_at(self, xval, yval):
        if self.gfu is None:
            raise RuntimeError("尚未求解，请先调用 assemble_and_solve()")
        pt = (xval, yval)
        val = self.gfu(self.mesh(pt[0], pt[1]))
        # handle possible vector return
        if hasattr(val, "__len__") and len(val) > 0:
            return float(val[0])
        return float(val)

    def compute_errors(self):
        if self.exact_solution is None:
            print("[INFO] 未提供精确解，跳过误差计算。")
            return None, None
        try:
            exact_gf = GridFunction(self.fes)
            exact_gf.Set(self.exact_solution)

            error_gf = exact_gf - self.gfu
            l2err = sqrt(Integrate(error_gf**2, self.mesh))
            print(f"[INFO] L2 error = {l2err:.6e}")
        except Exception as e:
            print(f"[WARN] 计算 L2 误差失败: {e}")
            l2err = None

        # H1 error (attempt)
        h1err = None
        try:
            # compute gradient difference in a vector H1 space
            fes_vec = H1(self.mesh, order=self.order, dim=2)
            grad_exact = GridFunction(fes_vec)
            grad_num = GridFunction(fes_vec)
            grad_exact.Set(grad(self.exact_solution))
            grad_num.Set(grad(self.gfu))
            err_grad = grad_exact - grad_num
            h1_semi = sqrt(Integrate(InnerProduct(err_grad, err_grad), self.mesh))
            if l2err is not None:
                h1err = sqrt(l2err**2 + h1_semi**2)
                print(f"[INFO] H1 error = {h1err:.6e}")
        except Exception as e:
            print(f"[WARN] 计算 H1 误差失败: {e}")

        return l2err, h1err

# ------------------ Visualization utilities ------------------
PLOT_DIR = "plots"
os.makedirs(PLOT_DIR, exist_ok=True)

def sample_solution_to_grid(solver, nx=100, ny=100, padding=0.01):
    x_vals = np.linspace(0+padding, 1-padding, nx)
    y_vals = np.linspace(0+padding, 1-padding, ny)
    X, Y = np.meshgrid(x_vals, y_vals)
    Z = np.zeros_like(X)
    for i in range(ny):
        for j in range(nx):
            try:
                Z[i,j] = solver.get_value_at(float(x_vals[j]), float(y_vals[i]))
            except Exception:
                Z[i,j] = 0.0
    return X, Y, Z

def plot_solution_plots(solver, mesh, exact_solution=None, l2_error=None):
    X, Y, Z = sample_solution_to_grid(solver, nx=150, ny=150)

    # 1) Combined figure: contour, heatmap, diagonal profile
    plt.figure(figsize=(15,5))

    plt.subplot(1,3,1)
    cs = plt.contourf(X, Y, Z, levels=50)
    plt.colorbar(cs, label='u(x,y)')
    plt.title('Finite Element Solution (contour)')
    plt.xlabel('x'); plt.ylabel('y'); plt.axis('equal')

    plt.subplot(1,3,2)
    plt.imshow(Z, extent=[0,1,0,1], origin='lower', aspect='auto')
    plt.colorbar(label='u(x,y)')
    plt.title('Finite Element Solution (heatmap)')
    plt.xlabel('x'); plt.ylabel('y')

    plt.subplot(1,3,3)
    diag = np.diag(Z)
    xdiag = np.linspace(0+0.01, 1-0.01, diag.size)
    plt.plot(xdiag, diag, '-', linewidth=2)
    plt.title('Solution along diagonal y=x')
    plt.xlabel('x'); plt.ylabel('u(x,x)'); plt.grid(True)

    if l2_error is not None:
        plt.suptitle(f'Finite Element Solution (L2 error = {l2_error:.2e})')
    plt.tight_layout()
    file1 = os.path.join(PLOT_DIR, 'finite_element_solution.png')
    plt.savefig(file1, dpi=300, bbox_inches='tight')
    plt.show()
    print(f"[INFO] Saved {file1}")

    # 2) If exact solution provided: exact vs numeric vs abs error
    if exact_solution is not None:
        # evaluate exact on sample grid via GridFunction if possible, else direct eval
        Z_exact = np.zeros_like(Z)
        for i in range(Z.shape[0]):
            for j in range(Z.shape[1]):
                xv = float(X[i,j]); yv = float(Y[i,j])
                try:
                    # attempt to evaluate via ngsolve expression
                    Z_exact[i,j] = float(exact_solution(xv, yv))
                except Exception:
                    try:
                        # fallback: create temporary fes to evaluate (costly but robust)
                        fes_temp = H1(mesh, order=solver.order)
                        gf = GridFunction(fes_temp)
                        gf.Set(exact_solution)
                        Z_exact[i,j] = float(gf(mesh(xv,yv)))
                    except Exception:
                        Z_exact[i,j] = 0.0

        Z_err = np.abs(Z_exact - Z)

        plt.figure(figsize=(15,5))
        plt.subplot(1,3,1)
        plt.contourf(X, Y, Z_exact, levels=50)
        plt.title('Exact solution')
        plt.colorbar()

        plt.subplot(1,3,2)
        plt.contourf(X, Y, Z, levels=50)
        plt.title('Numerical solution')
        plt.colorbar()

        plt.subplot(1,3,3)
        plt.contourf(X, Y, Z_err, levels=50)
        plt.title('Absolute error')
        if l2_error is not None:
            plt.suptitle(f'Error (L2 = {l2_error:.2e})')
        plt.colorbar()

        file2 = os.path.join(PLOT_DIR, 'solution_comparison.png')
        plt.tight_layout()
        plt.savefig(file2, dpi=300, bbox_inches='tight')
        plt.show()
        print(f"[INFO] Saved {file2}")

    # 3) Profiles along x=0.5 and y=0.5
    xvals = np.linspace(0.01, 0.99, 200)
    y_line = 0.5
    profile_y = [solver.get_value_at(xv, y_line) for xv in xvals]
    yvals = np.linspace(0.01, 0.99, 200)
    x_line = 0.5
    profile_x = [solver.get_value_at(x_line, yv) for yv in yvals]

    plt.figure(figsize=(12,5))
    plt.subplot(1,2,1)
    plt.plot(xvals, profile_y, '-', linewidth=2, label='Numerical')
    if exact_solution is not None:
        exact_profile_y = []
        for xv in xvals:
            try:
                exact_profile_y.append(float(exact_solution(xv, y_line)))
            except:
                exact_profile_y.append(0.0)
        plt.plot(xvals, exact_profile_y, '--', label='Exact')
    plt.title(f'Profile at y={y_line}')
    plt.xlabel('x'); plt.ylabel('u(x,0.5)'); plt.grid(True); plt.legend()

    plt.subplot(1,2,2)
    plt.plot(yvals, profile_x, '-', linewidth=2, label='Numerical')
    if exact_solution is not None:
        exact_profile_x = []
        for yv in yvals:
            try:
                exact_profile_x.append(float(exact_solution(x_line, yv)))
            except:
                exact_profile_x.append(0.0)
        plt.plot(yvals, exact_profile_x, '--', label='Exact')
    plt.title(f'Profile at x={x_line}')
    plt.xlabel('y'); plt.ylabel('u(0.5,y)'); plt.grid(True); plt.legend()

    file3 = os.path.join(PLOT_DIR, 'solution_profiles.png')
    plt.tight_layout()
    plt.savefig(file3, dpi=300, bbox_inches='tight')
    plt.show()
    print(f"[INFO] Saved {file3}")

# ------------------ Convergence study utility ------------------
def convergence_study(fixed_exact_solution, a_coef, f_coef, orders=[1,2], mesh_sizes=[0.2,0.1,0.05]):
    results = {}
    for order in orders:
        l2_list = []
        for h in mesh_sizes:
            mesh = Mesh(unit_square.GenerateMesh(maxh=h))
            solver = EllipticSolver(mesh, order=order)
            solver.setup_fem_space()
            solver.set_problem(a_coef=a_coef, f_coef=f_coef, exact_solution=fixed_exact_solution)
            solver.assemble_and_solve()
            l2err, h1err = solver.compute_errors()
            if l2err is not None:
                l2_list.append(float(l2err))
        results[order] = (mesh_sizes[:len(l2_list)], l2_list)

    # plot
    plt.figure(figsize=(8,6))
    for order, (hs, errs) in results.items():
        if len(errs) > 0:
            plt.loglog(hs, errs, 'o-', label=f'p={order}')
    # reference lines if possible
    plt.xlabel('mesh size h'); plt.ylabel('L2 error'); plt.title('Convergence study'); plt.grid(True); plt.legend()
    file_conv = os.path.join(PLOT_DIR, 'convergence_analysis.png')
    plt.savefig(file_conv, dpi=300, bbox_inches='tight')
    plt.show()
    print(f"[INFO] Saved {file_conv}")

# ------------------ MAIN: USER CONFIGURATION (edit here) ------------------
if __name__ == "__main__":
    # ---------- USER: modify here ----------
    # Choose finite element order
    ORDER = 2

    # Mesh size parameter for the main run
    MESH_H = 0.1

    # Define coefficients and (optional) exact solution using ngsolve symbols x,y
    # Examples:
    #   constant a and f: a = 1.0, f = 1.0
    #   variable a: a = 1.0 + 0.5*x + 0.5*y
    #   exact solution example: u = x*(1-x)*y*(1-y)
    # NOTE: use ngsolve.Symbols x,y (imported above from ngsolve)
    a = 1.0                                # <-- change here if you want variable a, e.g. 1.0 + 0.5*x + 0.5*y
    exact_solution = x*(1-x)*y*(1-y)       # <-- set to None if you don't have exact solution
    f = 2*(y*(1-y) + x*(1-x))              # <-- change source term as needed
    # ---------- end USER section ----------

    print("[INFO] Creating mesh...")
    mesh = Mesh(unit_square.GenerateMesh(maxh=MESH_H))
    solver = EllipticSolver(mesh, order=ORDER)
    solver.setup_fem_space()
    solver.set_problem(a_coef=a, f_coef=f, exact_solution=exact_solution)
    solver.assemble_and_solve()
    l2err, h1err = solver.compute_errors()

    # Visualize & save plots
    plot_solution_plots(solver, mesh, exact_solution=exact_solution, l2_error=(float(l2err) if l2err is not None else None))

    # Convergence run (optional, comment out if not needed)
    try:
        convergence_study(exact_solution, a, f, orders=[1,2], mesh_sizes=[0.2,0.1,0.05])
    except Exception as e:
        print("[WARN] 收敛性分析失败或被跳过: " + str(e))

    print("[INFO] All done. Plots saved in 'plots/' directory.")
