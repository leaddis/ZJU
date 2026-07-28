# model_reduction.py
import numpy as np
from scipy.linalg import svd
from pde_solver import PDESolver
import matplotlib.pyplot as plt

class ModelReductionSVD:
    def __init__(self, mesh, order=2):
        self.mesh = mesh
        self.order = order
        self.solver = PDESolver(mesh, order)
        self.snapshot_matrix = None
        self.U = None
        self.S = None
        self.VT = None
        self.reduced_basis = None
        
    def generate_training_data(self, a_min, a_max, num_samples, f_coef):
        """生成训练数据：对不同的a求解PDE"""
        print(f"生成训练数据: a ∈ [{a_min}, {a_max}], 样本数: {num_samples}")
        
        a_samples = np.linspace(a_min, a_max, num_samples)
        solutions = []
        
        for i, a_val in enumerate(a_samples):
            print(f"求解样本 {i+1}/{num_samples}: a = {a_val:.3f}")
            
            # 求解PDE
            solver = PDESolver(self.mesh, self.order)
            u = solver.solve(a_coef=a_val, f_coef=f_coef)
            
            # 获取解向量
            u_vector = self.solution_to_vector(u)
            solutions.append(u_vector)
        
        # 构建快照矩阵 (每列是一个解向量)
        self.snapshot_matrix = np.column_stack(solutions)
        print(f"快照矩阵形状: {self.snapshot_matrix.shape}")
        
        return a_samples, self.snapshot_matrix
    
    def perform_svd(self, energy_threshold=0.999):
        """对快照矩阵进行SVD分解"""
        print("进行SVD分解...")
        
        # 执行SVD
        self.U, self.S, self.VT = svd(self.snapshot_matrix, full_matrices=False)
        
        # 计算累积能量
        total_energy = np.sum(self.S ** 2)
        cumulative_energy = np.cumsum(self.S ** 2) / total_energy
        
        # 确定截断秩
        self.rank = np.argmax(cumulative_energy >= energy_threshold) + 1
        print(f"SVD完成: 原始维度 = {len(self.S)}, 截断秩 = {self.rank}")
        print(f"能量保留: {cumulative_energy[self.rank-1]:.4f}")
        
        # 构造降阶基
        self.reduced_basis = self.U[:, :self.rank]
        
        return self.U, self.S, self.VT, self.rank
    
    def project_to_reduced_space(self, u_vector):
        """将解向量投影到降阶空间"""
        if self.reduced_basis is None:
            raise ValueError("请先执行SVD分解")
        
        # 投影系数: u_reduced = U_r^T * u
        u_reduced = self.reduced_basis.T @ u_vector
        return u_reduced
    
    def reconstruct_from_reduced(self, u_reduced):
        """从降阶空间重构解向量"""
        if self.reduced_basis is None:
            raise ValueError("请先执行SVD分解")
        
        # 重构: u ≈ U_r * u_reduced
        u_reconstructed = self.reduced_basis @ u_reduced
        return u_reconstructed
    
    def solve_reduced(self, a_val, f_coef):
        """在降阶空间中求解新问题"""
        if self.reduced_basis is None:
            raise ValueError("请先执行SVD分解")
        
        # 这里需要实现降阶模型的求解
        # 简化版本：找到最近的训练样本的投影
        # 实际应该构建降阶的PDE系统
        
        # 临时方案：使用全阶求解然后投影
        u_full = self.solver.solve(a_coef=a_val, f_coef=f_coef)
        u_vector = self.solution_to_vector(u_full)
        u_reduced = self.project_to_reduced_space(u_vector)
        
        return u_reduced, u_vector
    
    def solution_to_vector(self, u_solution):
        """将PDE解转换为向量"""
        # 这里需要根据你的数据结构实现
        # 假设u_solution有vec属性
        return u_solution.vec.FV().NumPy()
    
    def vector_to_solution(self, u_vector):
        """将向量转换为PDE解"""
        u_solution = GridFunction(self.solver.fes)
        u_solution.vec.FV().NumPy()[:] = u_vector
        return u_solution