import numpy as np
import time
import matplotlib.pyplot as plt
from scipy.linalg import norm, svd
###########################################################################################
######################## 生成有限元数据的函数 ##############################################
###########################################################################################
# 修正后的有限元数据生成函数
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
        # 伪逆初始化为单位矩阵
        self.R_tilde_plus = np.eye(1)
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
            # 构造Y矩阵 - 确保维度正确
            Y = np.zeros((self.rank + 1, self.rank + 1))
            Y[:self.rank, :self.rank] = self.Sigma
            Y[:self.rank, self.rank] = d.flatten()
            
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=False)
            
            # 更新小型矩阵
            self.Q_tilde = self.Q_tilde @ Q_Y
            self.Sigma = np.diag(Sigma_Y)
            
            # 伪逆操作 - 添加稳定性处理
            R1 = R_Y[:self.rank, :self.rank]
            R2 = R_Y[self.rank, :self.rank].reshape(1, -1)
            
            # 稳定伪逆计算
            U_r, S_r, Vt_r = svd(R1, full_matrices=False)
            S_inv = np.zeros_like(R1).T
            for i in range(len(S_r)):
                if S_r[i] > self.tol * 10:  # 过滤小奇异值
                    S_inv[i, i] = 1 / S_r[i]
            R1_plus = Vt_r.T @ S_inv @ U_r.T
            
            self.R_tilde = self.R_tilde @ R1
            self.R_tilde_plus = R1_plus @ self.R_tilde_plus
            self.R = np.vstack([self.R, R2 @ self.R_tilde_plus])
        
        else:
            e_norm = e / p
            # 构造Y矩阵 - 确保维度正确
            Y = np.zeros((self.rank + 1, self.rank + 1))
            Y[:self.rank, :self.rank] = self.Sigma
            Y[:self.rank, self.rank] = d.flatten()
            Y[self.rank, self.rank] = p
            
            Q_Y, Sigma_Y, R_Y = svd(Y, full_matrices=True)
            
            # 更新矩阵 - 使用更精确的扩展方法
            Q_tilde_ext = np.eye(self.rank + 1)
            Q_tilde_ext[:self.rank, :self.rank] = self.Q_tilde
            self.Q_tilde = Q_tilde_ext @ Q_Y
            
            self.Q = np.hstack([self.Q, e_norm.reshape(-1, 1)])
            self.Sigma = np.diag(Sigma_Y)
            
            # 伪逆操作 - 添加稳定性处理
            R_tilde_ext = np.eye(self.rank + 1)
            R_tilde_ext[:self.rank, :self.rank] = self.R_tilde
            self.R_tilde = R_tilde_ext @ R_Y
            
            R_tilde_plus_ext = np.eye(self.rank + 1)
            R_tilde_plus_ext[:self.rank, :self.rank] = self.R_tilde_plus
            self.R_tilde_plus = R_Y.T @ R_tilde_plus_ext
            
            # 更新R矩阵
            R_new = np.zeros((self.R.shape[0] + 1, self.rank + 1))
            R_new[:self.R.shape[0], :self.R.shape[1]] = self.R
            R_new[-1, -1] = 1
            self.R = R_new
            
            self.rank += 1
        
        # 奇异值截断 - 关键修复
        svals = np.diag(self.Sigma)
        valid_idx = np.where(svals > self.tol)[0]
        if len(valid_idx) < self.rank:
            self.rank = len(valid_idx)
            self.Q = self.Q[:, :self.rank]
            self.Q_tilde = self.Q_tilde[:, :self.rank]
            self.Sigma = np.diag(svals[valid_idx])
            self.R = self.R[:, :self.rank]
            self.R_tilde = self.R_tilde[:self.rank, :self.rank]
            # 重新计算伪逆
            U_r, S_r, Vt_r = svd(self.R_tilde, full_matrices=False)
            S_inv = np.diag(1/(S_r + 1e-15))
            self.R_tilde_plus = Vt_r.T @ S_inv @ U_r.T
        
        # 改进的重正交化触发条件
        QtQ = self.Q_tilde.T @ self.Q_tilde
        ortho_dev = np.max(np.abs(QtQ - np.eye(self.rank)))
        if ortho_dev > self.tol:
            self._reorthogonalize_small()
        
        # 记录正交误差
        self.ortho_errors.append(self.orthogonal_error())
        self.small_ortho_errors.append(self.small_orthogonal_error())
    
    def _reorthogonalize_small(self):
        """改进的重正交化实现"""
        k = self.Q_tilde.shape[1]
        # 使用改进的Gram-Schmidt
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
        """算法6: 完整增量SVD (Algorithm 6)"""
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
# 测试增量SVD算法
###########################################################################################

def main():
    # 生成测试数据 - 使用完全平方数
    m, n = 1024, 500  # 1024 = 32x32 是完全平方数
    print(f"生成测试数据: m={m}, n={n}")
    U, M = generate_fem_data(m, n)
    
    # 测试算法(I)
    print("\n测试增量SVD (I)算法...")
    start_time = time.time()
    svd_v1 = IncrementalSVDv1(W=np.eye(m))  # W=I
    Q1, Sigma1, R1 = svd_v1.full_svd(U)
    time_v1 = time.time() - start_time
    ortho_error_v1 = svd_v1.orthogonal_error()
    print(f"算法(I) 耗时: {time_v1:.2f}秒, 正交误差: {ortho_error_v1:.2e}")
    
    # 测试算法(II)
    print("\n测试增量SVD (II)算法...")
    start_time = time.time()
    svd_v2 = IncrementalSVDv2(W=np.eye(m))  # W=I
    Q2, Sigma2, R2 = svd_v2.full_svd(U)
    time_v2 = time.time() - start_time
    ortho_error_small_v2 = svd_v2.small_orthogonal_error()
    print(f"算法(II) 耗时: {time_v2:.2f}秒, 小型矩阵正交误差: {ortho_error_small_v2:.2e}")
    
    # 与标准SVD比较
    print("\n计算标准SVD...")
    start_time = time.time()
    U_full, S_full, VT_full = np.linalg.svd(U, full_matrices=False)
    time_full = time.time() - start_time
    ortho_error_full = norm(np.eye(U_full.shape[1]) - U_full.T @ U_full, 'fro')
    print(f"标准SVD 耗时: {time_full:.2f}秒, 正交误差: {ortho_error_full:.2e}")
    
# Plot singular value comparison
    plt.figure(figsize=(12, 5))
    
    # Algorithm (I) vs Standard SVD
    plt.subplot(121)
    svals_v1 = np.diag(Sigma1)[:50] if len(Sigma1.shape) == 2 else Sigma1[:50]
    plt.plot(svals_v1, 'ro-', label='Algorithm (I)')
    plt.plot(S_full[:50], 'bs-', label='Standard SVD')
    plt.title('First 50 Singular Values Comparison (Algorithm I vs Standard SVD)')
    plt.xlabel('Index')
    plt.ylabel('Singular Value')
    plt.legend()
    
    # Algorithm (II) vs Standard SVD
    plt.subplot(122)
    svals_v2 = np.diag(Sigma2)[:50] if len(Sigma2.shape) == 2 else Sigma2[:50]
    plt.plot(svals_v2, 'go-', label='Algorithm (II)')
    plt.plot(S_full[:50], 'bs-', label='Standard SVD')
    plt.title('First 50 Singular Values Comparison (Algorithm II vs Standard SVD)')
    plt.xlabel('Index')
    plt.ylabel('Singular Value')
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('svd_comparison.png')
    plt.show()
    
    # Weighted inner product test (W=M)
    print("\nTesting weighted inner product (W=M)...")
    svd_weighted = IncrementalSVDv1(W=M)
    start_time = time.time()
    Qw, Sigmaw, Rw = svd_weighted.full_svd(U)
    time_weighted = time.time() - start_time
    ortho_error_weighted = svd_weighted.orthogonal_error()
    print(f"Weighted SVD (W=M) time: {time_weighted:.2f}s, orthogonality error: {ortho_error_weighted:.2e}")
    
    # Plot orthogonality error changes with incremental updates
    if hasattr(svd_weighted, 'ortho_errors') and svd_weighted.ortho_errors:
        plt.figure(figsize=(10, 6))
        plt.semilogy(svd_weighted.ortho_errors, 'r-')
        plt.title('Orthogonality Error Changes with Incremental Updates (W=M)')
        plt.xlabel('Number of added vectors')
        plt.ylabel(r'$\mathcal{E}_W$')
        plt.grid(True)
        plt.savefig('orthogonality_error.png')
        plt.show()


if __name__ == "__main__":
    main()