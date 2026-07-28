# pde_solver.py - 专门的PDE求解器
from ngsolve import *
from netgen.geom2d import unit_square

class PDESolver:
    def __init__(self, mesh, order=2):
        self.mesh = mesh
        self.order = order
        self.fes = H1(mesh, order=order, dirichlet=".*")
        self.gfu = None
    
    def solve(self, a_coef, f_coef):
        """求解PDE: -∇·(a∇u) = f"""
        u = self.fes.TrialFunction()
        v = self.fes.TestFunction()
        
        # 变分形式
        a = BilinearForm(self.fes, symmetric=True)
        a += a_coef * grad(u) * grad(v) * dx
        
        f = LinearForm(self.fes)
        f += f_coef * v * dx
        
        # 组装和求解
        a.Assemble()
        f.Assemble()
        
        self.gfu = GridFunction(self.fes)
        freedofs = self.fes.FreeDofs()
        inv = a.mat.Inverse(freedofs, inverse="sparsecholesky")
        self.gfu.vec.data = inv * f.vec
        
        return self.gfu
    
    def get_dofs(self):
        """获取自由度数量"""
        return self.fes.ndof