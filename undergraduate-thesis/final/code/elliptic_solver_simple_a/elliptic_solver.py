from ngsolve import * # type: ignore
from netgen.geom2d import unit_square # type: ignore
import matplotlib.pyplot as plt
import numpy as np

class EllipticSolver:
    """
    稳定的椭圆方程求解器类：from netgen.geom2d import unit_square
mesh = Mesh(unit_square.GenerateMesh(maxh=MESH_H))
    """
    
    def __init__(self, mesh, order=1):
        """
        初始化求解器
        
        Args:
            mesh: 计算网格
            order: 有限元空间的多项式阶数
        """
        self.mesh = mesh
        self.order = order
        self.fes = None
        self.gfu = None
        self.a = None
        self.f = None
        self.exact_solution = None
        
    def setup_fem_space(self):
        """设置有限元空间"""
        self.fes = H1(self.mesh, order=self.order, dirichlet=".*")
        print(f"有限元空间: 分片{self.order}次多项式")
        print(f"自由度数量: {self.fes.ndof}")
        
    def define_coefficients(self, a_coef=1.0, f_coef=1.0, exact_solution=None):
        """
        定义方程系数和精确解（如果已知）
        
        Args:
            a_coef: 扩散系数,可以是常数或 CoefficientFunction
            f_coef: 源项,可以是常数或 CoefficientFunction
            exact_solution: 精确解（如果已知）
        """
        self.a = a_coef
        self.f = f_coef
        self.exact_solution = exact_solution
        
        print(f"扩散系数 a = {a_coef}")
        print(f"源项 f = {f_coef}")
        
    def setup_variational_forms(self):
        """设置变分形式"""
        u = self.fes.TrialFunction()
        v = self.fes.TestFunction()
        
        # 双线性形式: a(u,v) = ∫ a * ∇u · ∇v dx
        self.a_form = BilinearForm(self.fes, symmetric=True)
        self.a_form += self.a * grad(u) * grad(v) * dx
        
        # 线性形式: l(v) = ∫ f * v dx
        self.l_form = LinearForm(self.fes)
        self.l_form += self.f * v * dx
        
    def solve(self):
        """求解方程"""
        # 创建网格函数
        self.gfu = GridFunction(self.fes)
        
        # 组装矩阵和向量
        self.a_form.Assemble()
        self.l_form.Assemble()
        
        # 求解线性系统
        freedofs = self.fes.FreeDofs()
        inv = self.a_form.mat.Inverse(freedofs, inverse="sparsecholesky")
        self.gfu.vec.data = inv * self.l_form.vec
        
        print("求解完成!")
        
        # 检查解的质量
        self._check_solution_quality()
        
        return self.gfu
    
    def _check_solution_quality(self):
        """检查解的质量"""
        try:
            norm = sqrt(Integrate(self.gfu**2, self.mesh))
            print(f"解的L2范数: {norm:.6e}")
            
            # 获取解在顶点上的值范围
            values = []
            for v in self.mesh.vertices:
                val = self.gfu(self.mesh(v.point[0], v.point[1]))
                if hasattr(val, '__len__') and len(val) > 0:
                    val = val[0]
                values.append(float(val))
            
            print(f"解的最小值: {min(values):.6e}")
            print(f"解的最大值: {max(values):.6e}")
            
        except Exception as e:
            print(f"检查解质量时出错: {e}")
    
    def compute_error(self):
        """
        计算数值解的误差（如果精确解已知）
        
        Returns:
            l2_error: L2误差
            h1_error: H1半范数误差（如果可计算）
        """
        if self.exact_solution is None:
            print("未提供精确解,无法计算误差")
            return None, None
        
        try:
            # 创建精确解的网格函数
            exact_gf = GridFunction(self.fes)
            exact_gf.Set(self.exact_solution)
            
            # 计算L2误差
            error = exact_gf - self.gfu
            l2_error = sqrt(Integrate(error**2, self.mesh))
            
            print(f"L2误差: {l2_error:.6e}")
            
            # 尝试计算H1误差
            h1_error = None
            try:
                # 创建向量值函数空间用于计算梯度
                fes_vec = H1(self.mesh, order=self.order, dim=2)
                grad_exact = GridFunction(fes_vec)
                grad_numerical = GridFunction(fes_vec)
                
                grad_exact.Set(grad(self.exact_solution))
                grad_numerical.Set(grad(self.gfu))
                
                error_grad = grad_exact - grad_numerical
                h1_semi_error = sqrt(Integrate(InnerProduct(error_grad, error_grad), self.mesh))
                h1_error = sqrt(l2_error**2 + h1_semi_error**2)
                
                print(f"H1误差: {h1_error:.6e}")
            except:
                print("...")
            
            return l2_error, h1_error
            
        except Exception as e:
            print(f"计算误差时出错: {e}")
            return None, None
    
    def get_solution_value(self, x, y):
        """
        在给定点处获取解的值
        
        Args:
            x, y: 点的坐标
            
        Returns:
            解在该点处的值
        """
        try:
            point = (x, y)
            val = self.gfu(self.mesh(point[0], point[1]))
            # 确保返回标量
            if hasattr(val, '__len__') and len(val) > 0:
                return float(val[0])
            else:
                return float(val)
        except Exception as e:
            print(f"获取解在点({x},{y})的值时出错: {e}")
            return float('nan')
    
    def plot_solution(self, title="solution"):
        """绘制解"""
        try:
            Draw(self.gfu, self.mesh, title)
            print(f"解已绘制: {title}")
        except Exception as e:
            print(f"绘制解时出错: {e}")

def create_mesh(maxh=0.1, geometry="unit_square"):
    """创建网格"""
    if geometry == "unit_square":
        from netgen.geom2d import unit_square
        ngmesh = unit_square.GenerateMesh(maxh=maxh)
        mesh = Mesh(ngmesh)
    else:
        # 可以扩展其他几何形状
        raise ValueError(f"不支持的几何形状: {geometry}")
    
    print(f"网格信息: {mesh.ne} 个单元")
    return mesh

def example_constant_coefficients():
    """常数系数示例"""
    print("=== 常数系数示例 ===\n")
    
    # 创建网格
    mesh = create_mesh(maxh=0.1)
    
    # 创建求解器
    solver = EllipticSolver(mesh, order=2)
    solver.setup_fem_space()
    
    # 定义系数
    a = 1.0
    f = 1.0
    
    solver.define_coefficients(a_coef=a, f_coef=f)
    solver.setup_variational_forms()
    solver.solve()
    
    # 在几个点检查解的值
    points = [(0.5, 0.5), (0.25, 0.25), (0.75, 0.75)]
    for p in points:
        value = solver.get_solution_value(p[0], p[1])
        print(f"解在点({p[0]},{p[1]})的值: {value:.6e}")
    
    # 绘制解
    solver.plot_solution("constant_coefficients")
    
    return solver

def example_known_solution():
    """已知精确解示例"""
    print("\n=== 已知精确解示例 ===\n")
    
    # 创建网格
    mesh = create_mesh(maxh=0.1)
    
    # 创建求解器
    solver = EllipticSolver(mesh, order=2)
    solver.setup_fem_space()
    
    # 定义已知精确解和对应的源项
    exact_solution = x*(1-x)*y*(1-y)
    f = 2*(y*(1-y) + x*(1-x))
    
    solver.define_coefficients(
        a_coef=1.0, 
        f_coef=f, 
        exact_solution=exact_solution
    )
    solver.setup_variational_forms()
    solver.solve()
    
    # 计算误差
    l2_error, h1_error = solver.compute_error()
    
    # 在中心点比较
    exact_center = exact_solution(0.5, 0.5)
    num_center = solver.get_solution_value(0.5, 0.5)
    print(f"中心点精确解: {exact_center:.6e}")
    print(f"中心点数值解: {num_center:.6e}")
    print(f"中心点绝对误差: {abs(exact_center - num_center):.6e}")
    
    # 绘制解
    solver.plot_solution("known_solution")
    
    return solver, l2_error

def example_variable_coefficients():
    """变系数示例"""
    print("\n=== 变系数示例 ===\n")
    
    # 创建网格
    mesh = create_mesh(maxh=0.1)
    
    # 创建求解器
    solver = EllipticSolver(mesh, order=2)
    solver.setup_fem_space()
    
    # 定义变系数和源项
    a = 1.0 + 0.5*x + 0.5*y  # 空间变化的扩散系数
    f = 1.0
    
    solver.define_coefficients(a_coef=a, f_coef=f)
    solver.setup_variational_forms()
    solver.solve()
    
    # 绘制解
    solver.plot_solution("variable_coefficients")
    
    return solver

def convergence_study():
    """收敛性研究"""
    print("\n=== 收敛性研究 ===\n")
    
    mesh_sizes = [0.2, 0.1, 0.05]
    orders = [1, 2, 3]
    
    plt.figure(figsize=(10, 8))
    
    for order in orders:
        print(f"\n多项式阶数: {order}")
        l2_errors = []
        h1_errors = []
        
        for h in mesh_sizes:
            print(f"  网格尺寸: {h}")
            
            # 创建网格和求解器
            mesh = create_mesh(maxh=h)
            solver = EllipticSolver(mesh, order=order)
            solver.setup_fem_space()
            
            # 使用已知精确解
            exact_solution = x*(1-x)*y*(1-y)
            f = 2*(y*(1-y) + x*(1-x))
            
            solver.define_coefficients(
                a_coef=1.0,
                f_coef=f,
                exact_solution=exact_solution
            )
            solver.setup_variational_forms()
            solver.solve()
            
            # 计算误差
            l2_error, h1_error = solver.compute_error()
            
            if l2_error is not None:
                l2_errors.append(l2_error)
            if h1_error is not None:
                h1_errors.append(h1_error)
        
        # 绘制收敛曲线
        if len(l2_errors) > 0:
            plt.loglog(mesh_sizes[:len(l2_errors)], l2_errors, 'o-', 
                      label=f'L2 error (p={order})')
        
        if len(h1_errors) > 0:
            plt.loglog(mesh_sizes[:len(h1_errors)], h1_errors, 's--', 
                      label=f'H1 error (p={order})')
        
        # 计算收敛率
        if len(l2_errors) > 1:
            l2_rates = []
            for i in range(1, len(l2_errors)):
                rate = np.log(l2_errors[i-1]/l2_errors[i]) / np.log(mesh_sizes[i-1]/mesh_sizes[i])
                l2_rates.append(rate)
            avg_l2_rate = np.mean(l2_rates)
            print(f"  L2平均收敛率: {avg_l2_rate:.3f}")
        
        if len(h1_errors) > 1:
            h1_rates = []
            for i in range(1, len(h1_errors)):
                rate = np.log(h1_errors[i-1]/h1_errors[i]) / np.log(mesh_sizes[i-1]/mesh_sizes[i])
                h1_rates.append(rate)
            avg_h1_rate = np.mean(h1_rates)
            print(f"  H1平均收敛率: {avg_h1_rate:.3f}")
    
    plt.xlabel('Mesh size h')
    plt.ylabel('Error')
    plt.title('Convergence Study')
    plt.legend()
    plt.grid(True, which="both", ls="--")
    plt.show()

if __name__ == "__main__":
    # 运行示例
    try:
        solver1 = example_constant_coefficients()
    except Exception as e:
        print(f"常数系数示例失败: {e}")
    
    try:
        solver2, error = example_known_solution()
    except Exception as e:
        print(f"已知解示例失败: {e}")
    
    try:
        solver3 = example_variable_coefficients()
    except Exception as e:
        print(f"变系数示例失败: {e}")
    
    try:
        convergence_study()
    except Exception as e:
        print(f"收敛性研究失败: {e}")
    
    print("\n=== 所有示例完成 ===")
    
    # 基本用法
mesh = create_mesh(maxh=0.1)
solver = EllipticSolver(mesh, order=2)
solver.setup_fem_space()
solver.define_coefficients(a_coef=1.0, f_coef=1.0)
solver.setup_variational_forms()
solution = solver.solve()

# 使用已知精确解
exact_solution = x*(1-x)*y*(1-y)
f = 2*(y*(1-y) + x*(1-x))
solver.define_coefficients(a_coef=1.0, f_coef=f, exact_solution=exact_solution)
solver.setup_variational_forms()
solution = solver.solve()
l2_error, h1_error = solver.compute_error()

# 在特定点获取解的值
value = solver.get_solution_value(0.5, 0.5)

# 绘制解
solver.plot_solution("my_solution")