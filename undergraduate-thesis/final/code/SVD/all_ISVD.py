import numpy as np
import time
import matplotlib.pyplot as plt
from scipy.linalg import norm, svd, qr
from scipy.sparse import diags

###########################################################################################
######################## 生成有限元数据的函数 ##############################################
###########################################################################################
def generate_fem_data(m, n):
    """
    生成有限元数据
    :param m: 网格点数量 (需要是完全平方数)
    :param n: 时间点数量
    :return: 数据矩阵 U, 质量矩阵 M
    """
    # 计算网格尺寸
    grid_size = int(np.sqrt(m))
    if grid_size**2 != m:
        grid_size = int(np.sqrt(m)) + 1
        m = grid_size**2
        print(f"警告: m 不是完全平方数, 自动调整为 {m}")
    
    # 创建网格
    x = np.linspace(0, 1, grid_size)
    y = np.linspace(0, 1, grid_size)
    X, Y = np.meshgrid(x, y)
    nodes = np.vstack([X.ravel(), Y.ravel()]).T
    
    # 生成时间网格
    t = np.linspace(0, 10, n)
    
    # 生成数据矩阵
    U = np.zeros((m, n))
    for i in range(n):
        f = np.cos(t[i] * (nodes[:, 0] + nodes[:, 1]))
        U[:, i] = f
    
    # 有限元质量矩阵 (简化为对角矩阵)
    M = np.eye(m)  # 实际应用中应为有限元质量矩阵
    
    return U, M

###########################################################################################
# 增量SVD (I) 实现 (Brand原始算法)
###########################################################################################

class IncrementalSVDv1:
    def __init__(self, W, tol=1e-12):
        self.W = W  # 加权矩阵
        self.tol = tol  # 容差值
        self.initialized = False
        self.ortho_errors = []
    
    def initialize(self, u1):
        """算法1: 初始化增量SVD (Algorithm 1)"""
        Wu = self.W @ u1
        p = np.sqrt(u1.T @ Wu)
        if p > self.tol:
            self.Q = u1.reshape(-1, 1) / p
        else:
            self.Q = np.zeros_like(u1).reshape(-1, 1)
        self.Sigma = np.array([[p]])
        self.R = np.ones((1, 1))
        self.rank = 1
        self.initialized = True
        self.ortho_errors.append(self.orthogonal_error())
    
    def update(self, u_new):
        """算法2: 更新增量SVD (Algorithm 2)"""
        # 计算残差和投影
        Wu = self.W @ u_new
        d = self.Q.T @ Wu
        e = u_new - self.Q @ d
        p = np.sqrt(e.T @ self.W @ e)
        
        # 步骤2-6: 处理小范数情况
        if p < self.tol:
            p = 0.0
            # 构造Y矩阵
            Y_top = np.hstack([self.Sigma, d.reshape(-1, 1)])
            Y = np.vstack([Y_top, np.zeros((1, Y_top.shape[1]))])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新矩阵 - 修正维度问题
            k = self.rank
            self.Q = self.Q @ Q_Y[:k, :k]  # 只取前k行和前k列
            self.Sigma = np.diag(Sigma_Y[:k])
            
            # 扩展R矩阵
            R_ext_top = np.hstack([self.R, np.zeros((self.R.shape[0], 1))])
            R_ext_bottom = np.hstack([np.zeros((1, self.R.shape[1])), np.ones((1, 1))])
            R_ext = np.vstack([R_ext_top, R_ext_bottom])
            
            # 更新R矩阵
            self.R = R_ext @ R_Y[:, :k]
        else:
            # 步骤7-8: 处理大范数情况
            e_norm = e / p
            # 构造Y矩阵
            Y_top = np.hstack([self.Sigma, d.reshape(-1, 1)])
            Y_bottom = np.hstack([np.zeros((1, self.rank)), np.array([[p]])])
            Y = np.vstack([Y_top, Y_bottom])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=True)
            
            # 更新矩阵
            self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)]) @ Q_Y
            self.Sigma = np.diag(Sigma_Y)
            
            # 扩展R矩阵
            R_ext_top = np.hstack([self.R, np.zeros((self.R.shape[0], 1))])
            R_ext_bottom = np.hstack([np.zeros((1, self.R.shape[1])), np.ones((1, 1))])
            R_ext = np.vstack([R_ext_top, R_ext_bottom])
            
            # 更新R矩阵
            self.R = R_ext @ R_Y
            self.rank = self.Sigma.shape[0]
            
            # 步骤13-15: 截断小奇异值
            svals = np.diag(self.Sigma)
            valid_sv = np.where(svals >= self.tol)[0]
            if len(valid_sv) < self.rank:
                self.Q = self.Q[:, :len(valid_sv)]
                self.Sigma = np.diag(svals[valid_sv])
                self.R = self.R[:, :len(valid_sv)]
                self.rank = len(valid_sv)
        
        # 重正交化
        self.reorthogonalize()
        self.ortho_errors.append(self.orthogonal_error())
    
    def reorthogonalize(self):
        """算法3: 重正交化 (Algorithm 3) - 加权Gram-Schmidt"""
        k = self.Q.shape[1]
        for i in range(k):
            alpha = self.Q[:, i].copy()
            for j in range(i):
                proj = alpha.T @ self.W @ self.Q[:, j]
                self.Q[:, i] -= proj * self.Q[:, j]
            norm_val = np.sqrt(self.Q[:, i].T @ self.W @ self.Q[:, i])
            if norm_val > self.tol:
                self.Q[:, i] /= norm_val
    
    def full_svd(self, U):
        """算法4: 完整增量SVD (Algorithm 4)"""
        self.initialize(U[:, 0])
        for i in range(1, U.shape[1]):
            self.update(U[:, i])
        return self.Q, self.Sigma, self.R
    
    def orthogonal_error(self):
        """计算正交性误差"""
        if not hasattr(self, 'Q') or self.Q.size == 0:
            return np.nan
        QtWQ = self.Q.T @ self.W @ self.Q
        return norm(np.eye(self.rank) - QtWQ, 'fro')
    
###########################################################################################
# 增量SVD (II) 实现 (Brand改进算法)
########################################################################################### 

class IncrementalSVDv2:
    def __init__(self, W, tol=1e-12):
        self.W = W
        self.tol = tol
        self.initialized = False
        self.ortho_errors = []
        self.small_ortho_errors = []
    
    def initialize(self, u1):
        Wu = self.W @ u1
        p = np.sqrt(u1.T @ Wu)
        if p > self.tol:
            self.Q = u1.reshape(-1, 1) / p
        else:
            self.Q = np.zeros_like(u1).reshape(-1, 1)
        self.Sigma = np.array([[p]])
        self.R = np.ones((1, 1))
        self.Q_tilde = np.eye(1)  # 初始化为单位矩阵
        self.R_tilde = np.eye(1)
        self.R_tilde_plus = np.eye(1)  # 伪逆初始化为单位矩阵
        self.rank = 1
        self.initialized = True
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def update(self, u_new):
        Wu = self.W @ u_new
        d = self.Q.T @ Wu
        e = u_new - self.Q @ d
        p = np.sqrt(e.T @ self.W @ e)
        
        if p < self.tol:
            # 步骤3: 构造Y矩阵 - 修正维度问题
            Y = np.zeros((self.rank + 1, self.rank))
            Y[:self.rank, :self.rank] = self.Sigma
            Y[self.rank, :] = d
            
            # 步骤4: 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 步骤5: 更新小型矩阵
            self.Q_tilde = self.Q_tilde @ Q_Y
            self.Sigma = np.diag(Sigma_Y)
            
            # 提取子矩阵 - 修正维度问题
            k = self.rank
            R1 = R_Y[:k, :]
            R2 = R_Y[k, :].reshape(1, -1)  # 修正R2的提取方式
            
            # 步骤6: 更新R_tilde和R_tilde_plus
            self.R_tilde = self.R_tilde @ R1
            
            # 计算R1的伪逆 - 修正伪逆计算
            U_r, S_r, Vt_r = svd(R1, full_matrices=False)
            S_inv = np.zeros_like(R1.T)
            for i in range(len(S_r)):
                if S_r[i] > self.tol * 10:  # 过滤小奇异值
                    S_inv[i, i] = 1 / S_r[i]
            R1_plus = Vt_r.T @ S_inv @ U_r.T
            self.R_tilde_plus = R1_plus @ self.R_tilde_plus
            
            # 更新R矩阵
            self.R = np.vstack([self.R, R2 @ self.R_tilde_plus])
        
        else:
            # 步骤8: 归一化残差
            e_norm = e / p
            
            # 步骤9: 构造Y矩阵
            Y = np.zeros((self.rank + 1, self.rank + 1))
            Y[:self.rank, :self.rank] = self.Sigma
            Y[:self.rank, self.rank] = d.flatten()
            Y[self.rank, self.rank] = p
            
            # 步骤10: 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=True)
            
            # 步骤11: 更新矩阵
            # 扩展Q_tilde
            Q_tilde_ext = np.eye(self.rank + 1)
            Q_tilde_ext[:self.rank, :self.rank] = self.Q_tilde
            self.Q_tilde = Q_tilde_ext @ Q_Y
            
            # 扩展Q
            self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)])
            self.Sigma = np.diag(Sigma_Y)
            
            # 扩展R_tilde
            R_tilde_ext = np.eye(self.rank + 1)
            R_tilde_ext[:self.rank, :self.rank] = self.R_tilde
            self.R_tilde = R_tilde_ext @ R_Y
            
            # 扩展R_tilde_plus - 修正更新逻辑
            R_tilde_plus_ext = np.eye(self.rank + 1)
            R_tilde_plus_ext[:self.rank, :self.rank] = self.R_tilde_plus
            self.R_tilde_plus = R_Y.T @ R_tilde_plus_ext
            
            # 扩展R矩阵
            R_ext = np.zeros((self.R.shape[0] + 1, self.rank + 1))
            R_ext[:self.R.shape[0], :self.rank] = self.R
            R_ext[-1, -1] = 1
            self.R = R_ext
            
            self.rank += 1
        
        # 步骤14-16: 重正交化触发条件 - 修正触发条件
        if self.rank > 1:
            col1 = self.Q_tilde[:, 0]
            col_end = self.Q_tilde[:, -1]
            inner_product = np.abs(col1 @ col_end)
            if inner_product > self.tol:
                self._reorthogonalize_small()
        
        # 记录正交误差
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def _reorthogonalize_small(self):
        """改进的重正交化实现 - 使用Gram-Schmidt"""
        k = self.Q_tilde.shape[1]
        # 使用Gram-Schmidt过程
        for j in range(k):
            # 正交化当前列
            for i in range(j):
                proj = self.Q_tilde[:, i] @ self.Q_tilde[:, j]
                self.Q_tilde[:, j] -= proj * self.Q_tilde[:, i]
            
            # 归一化
            norm_val = np.linalg.norm(self.Q_tilde[:, j])
            if norm_val > self.tol:
                self.Q_tilde[:, j] /= norm_val
            else:
                # 处理零向量
                self.Q_tilde[:, j] = 0
    
    def full_svd(self, U):
        """完整增量SVD"""
        self.initialize(U[:, 0])
        for i in range(1, U.shape[1]):
            self.update(U[:, i])
        full_Q = self.Q @ self.Q_tilde
        full_R = self.R @ self.R_tilde
        return full_Q, self.Sigma, full_R
    
    def orthogonal_error(self):
        """计算正交性误差"""
        if not hasattr(self, 'Q') or self.Q.size == 0:
            return np.nan
        full_Q = self.Q @ self.Q_tilde
        QtWQ = full_Q.T @ self.W @ full_Q
        return norm(np.eye(self.rank) - QtWQ, 'fro')
    
    def small_orthogonal_error(self):
        """计算小型正交矩阵的正交性误差"""
        if not hasattr(self, 'Q_tilde') or self.Q_tilde.size == 0:
            return np.nan
        QtQ = self.Q_tilde.T @ self.Q_tilde
        return norm(np.eye(self.rank) - QtQ, 'fro')

###########################################################################################
# 增量SVD (III) 实现 (论文中的算法7)
###########################################################################################

class IncrementalSVDv3:
    def __init__(self, W, tol=1e-12):
        self.W = W
        self.tol = tol
        self.initialized = False
        self.ortho_errors = []
        self.small_ortho_errors = []
        self.q = 0  # 累积的小残差向量计数
        self.V = []  # 存储小残差向量
        self.Q0 = None  # 累积的小正交矩阵
    
    def initialize(self, u1):
        Wu = self.W @ u1
        p = np.sqrt(u1.T @ Wu)
        if p > self.tol:
            self.Q = u1.reshape(-1, 1) / p
        else:
            self.Q = np.zeros_like(u1).reshape(-1, 1)
        self.Sigma = np.array([[p]])
        self.R = np.ones((1, 1))
        self.Q0 = np.eye(1)  # 初始化为单位矩阵
        self.rank = 1
        self.initialized = True
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def update(self, u_new):
        Wu = self.W @ u_new
        d = self.Q.T @ Wu
        e = u_new - self.Q @ d
        p = np.sqrt(e.T @ self.W @ e)
        
        # 步骤4-6: 处理小范数情况 (累积)
        if p < self.tol:
            self.q += 1
            self.V.append(d.reshape(-1, 1))
            # 步骤7-12: 当累积到向量时更新
            if self.q > 0:
                # 构造Y矩阵
                V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
                Y = np.hstack([self.Sigma, V_mat])
                
                # 计算SVD
                Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
                
                # 更新小型矩阵
                self.Q0 = self.Q0 @ Q_Y
                self.Sigma = np.diag(Sigma_Y)
                
                # 更新R矩阵
                k = self.Sigma.shape[0]
                R1 = R_Y[:k, :k]
                R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
                
                # 更新R
                R_new = np.vstack([self.R @ R1, R2])
                self.R = R_new
                
                # 更新d
                d = Q_Y.T @ d
            return
        
        # 步骤13-16: 重正交化 (仅当有累积更新时)
        if self.q > 0:
            # 构造Y矩阵
            V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
            Y = np.hstack([self.Sigma, V_mat])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新小型矩阵
            self.Q0 = self.Q0 @ Q_Y
            self.Sigma = np.diag(Sigma_Y)
            
            # 更新R矩阵
            k = self.Sigma.shape[0]
            R1 = R_Y[:k, :k]
            R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
            
            # 更新R
            R_new = np.vstack([self.R @ R1, R2])
            self.R = R_new
            
            # 更新d
            d = Q_Y.T @ d
            
            # 重置累积变量
            self.V = []
            self.q = 0
        
        # 步骤17-19: 重正交化新向量
        e_norm = e / p
        if np.abs(e_norm.T @ self.W @ self.Q[:, 0]) > self.tol:
            # 加权Gram-Schmidt
            proj = self.Q.T @ (self.W @ e_norm)
            e_norm = e_norm - self.Q @ proj
            p = np.sqrt(e_norm.T @ self.W @ e_norm)
            if p > self.tol:
                e_norm = e_norm / p
        
        # 步骤20-26: 处理大范数情况
        Y = np.vstack([
            np.hstack([self.Sigma, d.reshape(-1, 1)]),
            np.hstack([np.zeros((1, self.Sigma.shape[1])), np.array([[p]])])
        ])
        
        Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=True)
        
        # 更新矩阵
        Q0_ext = np.eye(self.Q0.shape[0] + 1)
        Q0_ext[:self.Q0.shape[0], :self.Q0.shape[1]] = self.Q0
        self.Q0 = Q0_ext @ Q_Y
        
        self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)])
        self.Sigma = np.diag(Sigma_Y)
        
        # 更新R矩阵
        R_ext = np.eye(self.R.shape[1] + 1)
        R_ext[:self.R.shape[1], :self.R.shape[1]] = np.eye(self.R.shape[1])
        R_new = R_ext @ R_Y
        
        # 扩展原始R矩阵
        R_orig_ext = np.zeros((self.R.shape[0] + 1, self.R.shape[1] + 1))
        R_orig_ext[:self.R.shape[0], :self.R.shape[1]] = self.R
        R_orig_ext[-1, -1] = 1
        self.R = R_orig_ext @ R_new
        
        self.rank = self.Sigma.shape[0]
        
        # 记录正交误差
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def final_check(self):
        """算法8: 最终检查"""
        if self.q > 0:
            # 构造Y矩阵
            V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
            Y = np.hstack([self.Sigma, V_mat])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新矩阵
            self.Q = self.Q @ (self.Q0 @ Q_Y)
            self.Sigma = np.diag(Sigma_Y)
            
            # 更新R矩阵
            k = self.Sigma.shape[0]
            R1 = R_Y[:k, :k]
            R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
            
            # 更新R
            R_new = np.vstack([self.R @ R1, R2])
            self.R = R_new
            
            # 重置累积变量
            self.V = []
            self.q = 0
            self.Q0 = np.eye(self.Sigma.shape[0])
    
    def full_svd(self, U):
        """算法9: 完整增量SVD (III)"""
        self.initialize(U[:, 0])
        for i in range(1, U.shape[1]):
            self.update(U[:, i])
        self.final_check()
        return self.Q, self.Sigma, self.R
    
    def orthogonal_error(self):
        """计算正交性误差"""
        if not hasattr(self, 'Q') or self.Q.size == 0:
            return np.nan
        QtWQ = self.Q.T @ self.W @ self.Q
        return norm(np.eye(self.rank) - QtWQ, 'fro')
    
    def small_orthogonal_error(self):
        """计算小型正交矩阵的正交性误差"""
        if not hasattr(self, 'Q0') or self.Q0.size == 0:
            return np.nan
        QtQ = self.Q0.T @ self.Q0
        return norm(np.eye(self.Q0.shape[0]) - QtQ, 'fro')

###########################################################################################
# 增量SVD (IV) 实现 (论文中的算法10)
###########################################################################################

class IncrementalSVDv4:
    def __init__(self, W, tol=1e-12):
        self.W = W
        self.tol = tol
        self.initialized = False
        self.ortho_errors = []
        self.small_ortho_errors = []
        self.q = 0  # 累积的小残差向量计数
        self.V = []  # 存储小残差向量
        self.Q0 = None  # 累积的小正交矩阵
    
    def initialize(self, u1):
        Wu = self.W @ u1
        p = np.sqrt(u1.T @ Wu)
        if p > self.tol:
            self.Q = u1.reshape(-1, 1) / p
        else:
            self.Q = np.zeros_like(u1).reshape(-1, 1)
        self.Sigma = np.array([[p]])  # 保持为二维数组
        self.R = np.ones((1, 1))
        self.Q0 = np.eye(1)  # 初始化为单位矩阵
        self.rank = 1
        self.initialized = True
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def update(self, u_new):
        Wu = self.W @ u_new
        d = self.Q.T @ Wu
        e = u_new - self.Q @ d
        p = np.sqrt(e.T @ self.W @ e)
        
        # 步骤4-6: 处理小范数情况 (累积)
        if p < self.tol:
            self.q += 1
            self.V.append(d.reshape(-1, 1))
            # 步骤7-12: 当累积到向量时更新
            if self.q > 0:
                # 构造Y矩阵
                V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
                Y = np.hstack([self.Sigma, V_mat])
                
                # 计算SVD
                Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
                
                # 更新小型矩阵
                self.Q0 = self.Q0 @ Q_Y
                self.Sigma = np.diag(Sigma_Y)  # 保持为二维数组
                
                # 更新R矩阵
                k = self.Sigma.shape[0]
                R1 = R_Y[:k, :k]
                R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
                
                # 更新R
                R_new = np.vstack([self.R @ R1, R2])
                self.R = R_new
                
                # 更新d
                d = Q_Y.T @ d
            return
        
        # 步骤13-16: 重正交化 (仅当有累积更新时)
        if self.q > 0:
            # 构造Y矩阵
            V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
            Y = np.hstack([self.Sigma, V_mat])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新小型矩阵
            self.Q0 = self.Q0 @ Q_Y
            self.Sigma = np.diag(Sigma_Y)  # 保持为二维数组
            
            # 更新R矩阵
            k = self.Sigma.shape[0]
            R1 = R_Y[:k, :k]
            R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
            
            # 更新R
            R_new = np.vstack([self.R @ R1, R2])
            self.R = R_new
            
            # 更新d
            d = Q_Y.T @ d
            
            # 重置累积变量
            self.V = []
            self.q = 0
        
        # 步骤17-19: 重正交化新向量
        e_norm = e / p
        if np.abs(e_norm.T @ self.W @ self.Q[:, 0]) > self.tol:
            # 加权Gram-Schmidt
            proj = self.Q.T @ (self.W @ e_norm)
            e_norm = e_norm - self.Q @ proj
            p = np.sqrt(e_norm.T @ self.W @ e_norm)
            if p > self.tol:
                e_norm = e_norm / p
        
        # 步骤20-34: 处理大范数情况，包括额外的截断
        # 确保所有数组都是二维的
        d_reshaped = d.reshape(-1, 1)  # 确保d是二维数组
        Y_top = np.hstack([self.Sigma, d_reshaped])
        Y_bottom = np.hstack([np.zeros((1, self.Sigma.shape[1])), np.array([[p]])])
        Y = np.vstack([Y_top, Y_bottom])
        
        Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=True)
        sigma_vals = Sigma_Y  # 奇异值向量
        
        # 步骤25-33: 根据最后一个奇异值决定是否截断
        if sigma_vals[-1] > self.tol:
            # 不截断
            Q0_ext = np.eye(self.Q0.shape[0] + 1)
            Q0_ext[:self.Q0.shape[0], :self.Q0.shape[1]] = self.Q0
            self.Q0 = Q0_ext @ Q_Y
            
            self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)])
            self.Sigma = np.diag(sigma_vals)  # 保持为二维数组
            
            # 更新R矩阵
            R_ext = np.eye(self.R.shape[1] + 1)
            R_ext[:self.R.shape[1], :self.R.shape[1]] = np.eye(self.R.shape[1])
            R_new = R_ext @ R_Y
            
            # 扩展原始R矩阵
            R_orig_ext = np.zeros((self.R.shape[0] + 1, self.R.shape[1] + 1))
            R_orig_ext[:self.R.shape[0], :self.R.shape[1]] = self.R
            R_orig_ext[-1, -1] = 1
            self.R = R_orig_ext @ R_new
            
            self.rank = self.Sigma.shape[0]
        else:
            # 截断最后一个奇异值
            Q0_ext = np.eye(self.Q0.shape[0] + 1)
            Q0_ext[:self.Q0.shape[0], :self.Q0.shape[1]] = self.Q0
            self.Q0 = Q0_ext @ Q_Y[:, :-1]  # 截断最后一列
            
            self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)]) @ Q_Y[:, :-1]
            self.Sigma = np.diag(sigma_vals[:-1])  # 保持为二维数组
            
            # 更新R矩阵 (截断)
            R_ext = np.eye(self.R.shape[1] + 1)
            R_ext[:self.R.shape[1], :self.R.shape[1]] = np.eye(self.R.shape[1])
            R_new = R_ext @ R_Y[:, :-1]
            
            # 扩展原始R矩阵
            R_orig_ext = np.zeros((self.R.shape[0] + 1, self.R.shape[1] + 1))
            R_orig_ext[:self.R.shape[0], :self.R.shape[1]] = self.R
            R_orig_ext[-1, -1] = 1
            self.R = R_orig_ext @ R_new
            
            self.rank = self.Sigma.shape[0]
        
        # 重置累积变量
        self.V = []
        self.q = 0
        
        # 记录正交误差
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def final_check(self):
        """最终检查 (类似算法8)"""
        if self.q > 0:
            # 构造Y矩阵
            V_mat = np.hstack(self.V) if self.q > 1 else self.V[0]
            Y = np.hstack([self.Sigma, V_mat])
            
            # 计算SVD
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新矩阵
            self.Q = self.Q @ (self.Q0 @ Q_Y)
            self.Sigma = np.diag(Sigma_Y)
            
            # 更新R矩阵
            k = self.Sigma.shape[0]
            R1 = R_Y[:k, :k]
            R2 = R_Y[k:, :k] if R_Y.shape[0] > k else np.zeros((1, k))
            
            # 更新R
            R_new = np.vstack([self.R @ R1, R2])
            self.R = R_new
            
            # 重置累积变量
            self.V = []
            self.q = 0
            self.Q0 = np.eye(self.Sigma.shape[0])
    
    def full_svd(self, U):
        """完整增量SVD (IV)"""
        self.initialize(U[:, 0])
        for i in range(1, U.shape[1]):
            self.update(U[:, i])
        self.final_check()
        return self.Q, self.Sigma, self.R
    
    def orthogonal_error(self):
        """计算正交性误差"""
        if not hasattr(self, 'Q') or self.Q.size == 0:
            return np.nan
        QtWQ = self.Q.T @ self.W @ self.Q
        return norm(np.eye(self.rank) - QtWQ, 'fro')
    
    def small_orthogonal_error(self):
        """计算小型正交矩阵的正交性误差"""
        if not hasattr(self, 'Q0') or self.Q0.size == 0:
            return np.nan
        QtQ = self.Q0.T @ self.Q0
        return norm(np.eye(self.Q0.shape[0]) - QtQ, 'fro')

###########################################################################################
# 测试所有算法并复现论文结果
###########################################################################################

def main():
    # 生成测试数据
    m, n = 1024, 500  # 1024 = 32x32 是完全平方数
    print(f"生成测试数据: m={m}, n={n}")
    U, M = generate_fem_data(m, n)
    
    tol = 1e-10
    algorithms = [
        ("Algorithm I (Brand Original)", IncrementalSVDv1(np.eye(m), tol)),
        ("Algorithm II (Brand Improved)", IncrementalSVDv2(np.eye(m), tol)),
        ("Algorithm III (Our Improved)", IncrementalSVDv3(np.eye(m), tol)),
        ("Algorithm IV (With Truncation)", IncrementalSVDv4(np.eye(m), tol))
    ]
    
    # 结果存储
    results = {}
    
    # 测试所有算法 (W=I)
    for name, svd_class in algorithms:
        print(f"\n测试 {name} (W=I)...")
        start_time = time.time()
        Q, Sigma, R = svd_class.full_svd(U)
        elapsed = time.time() - start_time
        
        # 计算正交误差
        if isinstance(Sigma, np.ndarray) and Sigma.ndim == 2:
            svals = np.diag(Sigma)
        else:
            svals = Sigma
        
        rank = len(svals)
        QtQ = Q.T @ Q
        ortho_error = norm(np.eye(rank) - QtQ, 'fro')
        
        # 存储结果
        results[name] = {
            'time': elapsed,
            'ortho_error': ortho_error,
            'rank': rank,
            'svals': svals,
            'ortho_history': svd_class.ortho_errors
        }
        print(f"耗时: {elapsed:.2f}秒, 正交误差: {ortho_error:.2e}, 秩: {rank}")
    
    # 测试加权情况 (W=M)
    weighted_algorithms = [
        ("Algorithm III (W=M)", IncrementalSVDv3(W=M, tol=tol)),
        ("Algorithm IV (W=M)", IncrementalSVDv4(W=M, tol=tol))
    ]
    
    for name, svd_class in weighted_algorithms:
        print(f"\n测试 {name}...")
        start_time = time.time()
        Q, Sigma, R = svd_class.full_svd(U)
        elapsed = time.time() - start_time
        
        # 计算加权正交误差
        if isinstance(Sigma, np.ndarray) and Sigma.ndim == 2:
            svals = np.diag(Sigma)
        else:
            svals = Sigma
        
        rank = len(svals)
        QtMQ = Q.T @ M @ Q
        ortho_error = norm(np.eye(rank) - QtMQ, 'fro')
        
        # 存储结果
        results[name] = {
            'time': elapsed,
            'ortho_error': ortho_error,
            'rank': rank,
            'svals': svals,
            'ortho_history': svd_class.ortho_errors
        }
        print(f"耗时: {elapsed:.2f}秒, 正交误差: {ortho_error:.2e}, 秩: {rank}")
    
    # 计算标准SVD作为基准
    print("\n计算标准SVD (W=I)...")
    start_time = time.time()
    U_full, S_full, VT_full = np.linalg.svd(U, full_matrices=False)
    time_full = time.time() - start_time
    QtQ_full = U_full.T @ U_full
    ortho_error_full = norm(np.eye(U_full.shape[1]) - QtQ_full, 'fro')
    print(f"标准SVD 耗时: {time_full:.2f}秒, 正交误差: {ortho_error_full:.2e}")
    
    # 计算加权标准SVD
    print("\n计算加权标准SVD (W=M)...")
    start_time = time.time()
    M_sqrt = np.sqrt(M)  # 因为M是对角矩阵
    U_weighted = M_sqrt @ U
    Uw_full, Sw_full, VTw_full = np.linalg.svd(U_weighted, full_matrices=False)
    time_w_full = time.time() - start_time
    QtMQ_full = Uw_full.T @ M @ Uw_full
    ortho_error_w_full = norm(np.eye(Uw_full.shape[1]) - QtMQ_full, 'fro')
    print(f"加权标准SVD 耗时: {time_w_full:.2f}秒, 正交误差: {ortho_error_w_full:.2e}")
    
    ######################################################################################
    ######################### 绘制结果 (复现论文中的图表) ##################################
    ######################################################################################
    
    # 图1: 正交性误差比较 (W=I)
    plt.figure(figsize=(12, 8))
    
    plt.subplot(221)
    for name in ["Algorithm I (Brand Original)", "Algorithm II (Brand Improved)", 
                 "Algorithm III (Our Improved)", "Algorithm IV (With Truncation)"]:
        if name in results:
            plt.semilogy(results[name]['ortho_history'], label=name)
    plt.title('Orthogonality Error (W=I)')
    plt.xlabel('Number of Vectors Added')
    plt.ylabel(r'$\mathcal{E}_I(Q)$')
    plt.legend()
    plt.grid(True)
    
        # 测试所有算法 (W=I)
    for name, svd_class in algorithms:
        print(f"\n测试 {name} (W=I)...")
        start_time = time.time()
        Q, Sigma, R = svd_class.full_svd(U)
        elapsed = time.time() - start_time
        
        # 计算正交误差
        if isinstance(Sigma, np.ndarray) and Sigma.ndim == 2:
            svals = np.diag(Sigma)
        else:
            svals = Sigma
        
        rank = len(svals)
        QtQ = Q.T @ Q
        ortho_error = norm(np.eye(rank) - QtQ, 'fro')
        
        # 存储结果
        results[name] = {
            'time': elapsed,
            'ortho_error': ortho_error,
            'rank': rank,
            'svals': svals,
            'ortho_history': svd_class.ortho_errors,
            'small_ortho_errors': getattr(svd_class, 'small_ortho_errors', [])  # 添加这一行
        }
        print(f"耗时: {elapsed:.2f}秒, 正交误差: {ortho_error:.2e}, 秩: {rank}")

    # ... [后面的代码保持不变] ...

    # 修改绘图部分
    plt.figure(figsize=(12, 8))
    
    plt.subplot(221)
    for name in ["Algorithm I (Brand Original)", "Algorithm II (Brand Improved)", 
                 "Algorithm III (Our Improved)", "Algorithm IV (With Truncation)"]:
        if name in results and 'ortho_history' in results[name]:
            plt.semilogy(results[name]['ortho_history'], label=name)
    plt.title('Orthogonality Error (W=I)')
    plt.xlabel('Number of Vectors Added')
    plt.ylabel(r'$\mathcal{E}_I(Q)$')
    plt.legend()
    plt.grid(True)
    
    # 修改小型正交矩阵误差图
    plt.subplot(222)
    if "Algorithm II (Brand Improved)" in results and 'small_ortho_errors' in results["Algorithm II (Brand Improved)"]:
        plt.semilogy(results["Algorithm II (Brand Improved)"]['small_ortho_errors'], 
                    label='Algorithm II (Q_tilde)', linestyle='--')
    if "Algorithm III (Our Improved)" in results and 'small_ortho_errors' in results["Algorithm III (Our Improved)"]:
        plt.semilogy(results["Algorithm III (Our Improved)"]['small_ortho_errors'], 
                    label='Algorithm III (Q0)', linestyle='-.')
    plt.title('Small Orthogonal Matrix Error (W=I)')
    plt.xlabel('Number of Vectors Added')
    plt.ylabel(r'$\mathcal{E}_I(\widetilde{Q})$')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('all_results_comparison.png')
    plt.show()
    
    # 表格: 计算时间比较
    print("\n计算时间比较:")
    print(f"{'Algorithm':<30} | {'Time (s)':>10} | {'Ortho Error':>12}")
    print("-" * 55)
    for name, data in results.items():
        print(f"{name:<30} | {data['time']:>10.2f} | {data['ortho_error']:>12.2e}")
    print(f"{'Standard SVD (W=I)':<30} | {time_full:>10.2f} | {ortho_error_full:>12.2e}")
    print(f"{'Standard SVD (W=M)':<30} | {time_w_full:>10.2f} | {ortho_error_w_full:>12.2e}")

if __name__ == "__main__":
    main()