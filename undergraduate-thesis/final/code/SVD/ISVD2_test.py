import numpy as np
from scipy.linalg import svd
from numpy.linalg import norm, pinv

def gram_schmidt(A, W=None):
    """Gram-Schmidt正交化（可选加权内积）"""
    if W is None:
        W = np.eye(A.shape[0])
    Q = np.zeros_like(A, dtype=np.float64)
    for i in range(A.shape[1]):
        v = A[:, i].copy().astype(np.float64)
        for j in range(i):
            qj = Q[:, j]
            # 计算投影系数（使用加权内积）
            proj = (qj.T @ W @ v) / (qj.T @ W @ qj)
            v = v - proj * qj
        # 计算加权范数
        v_norm = np.sqrt(v.T @ W @ v)
        if v_norm > 1e-10:
            Q[:, i] = v / v_norm
        else:
            Q[:, i] = v
    return Q

def InitializeISVD(u1, W):
    """算法1：初始化增量SVD"""
    u1 = u1.reshape(-1, 1)  # 确保是列向量
    sigma = np.sqrt(u1.T @ W @ u1).item()
    Q = u1 / sigma
    R = np.eye(1)  # 初始化为单位矩阵
    return Q, np.array([[sigma]]), R

def UpdateISVD2(Q, Sigma, R, Q_tilde, R_tilde, R_tilde_plus, u_new, W, tol=1e-10):
    """算法5：增量SVD更新"""
    u_new = u_new.reshape(-1, 1)  # 确保是列向量
    k = Q.shape[1]  # 当前基的维度
    
    # 计算投影和残差
    d = Q.T @ (W @ u_new)
    e = u_new - Q @ d
    p = np.sqrt(e.T @ W @ e).item()
    
    if p < tol:
        # Case 1: 新向量在当前基的线性张成中
        # 构建矩阵Y: [Σ d; 0 0]
        Y = np.zeros((k+1, k+1))
        Y[:k, :k] = Sigma
        Y[:k, k] = d.flatten()
        
        # 执行SVD
        Q_y, Sigma_y, R_y_T = svd(Y, full_matrices=False)
        R_y = R_y_T.T
        
        # 更新组件
        Q_tilde = Q_tilde @ Q_y
        Sigma = np.diag(Sigma_y)
        
        # 提取子矩阵
        R1 = R_y[:k, :k]
        R2 = R_y[k:, :k]
        
        # 更新R_tilde及其伪逆
        R_tilde_new = R_tilde @ R1
        R_tilde_plus_new = pinv(R1) @ R_tilde_plus
        
        # 更新R
        R_new = np.vstack([R, R2 @ R_tilde_plus_new])
        
    else:
        # Case 2: 新向量扩展基
        e = e / p
        # 构建矩阵Y: [Σ d; 0 p]
        Y = np.zeros((k+1, k+1))
        Y[:k, :k] = Sigma
        Y[:k, k] = d.flatten()
        Y[k, k] = p
        
        # 执行SVD
        Q_y, Sigma_y, R_y_T = svd(Y, full_matrices=False)
        R_y = R_y_T.T
        
        # 更新Q_tilde
        Q_tilde = np.block([
            [Q_tilde, np.zeros((Q_tilde.shape[0], 1))],
            [np.zeros((1, Q_tilde.shape[1])), 1]
        ]) @ Q_y
        
        # 扩展基
        Q = np.hstack([Q, e])
        
        # 更新Sigma
        Sigma = np.diag(Sigma_y)
        
        # 更新R_tilde及其伪逆
        R_tilde_new = np.block([
            [R_tilde, np.zeros((R_tilde.shape[0], 1))],
            [np.zeros((1, R_tilde.shape[1])), 1]
        ]) @ R_y
        
        R_tilde_plus_new = R_y.T @ np.block([
            [R_tilde_plus, np.zeros((R_tilde_plus.shape[0], 1))],
            [np.zeros((1, R_tilde_plus.shape[1])), 1]
        ])
        
        # 更新R
        R_new = np.block([
            [R, np.zeros((R.shape[0], 1))],
            [np.zeros((1, R.shape[1])), 1]
        ])
    
    # 重新正交化检查
    if Q_tilde.shape[1] > 1 and np.abs(Q_tilde[:, 0] @ Q_tilde[:, -1]) > tol:
        Q_tilde = gram_schmidt(Q_tilde, W)
    
    return Q, Sigma, R_new, Q_tilde, R_tilde_new, R_tilde_plus_new

def FullyIncrementalSVD2(A, W=None, tol=1e-10):
    """算法6：完全增量SVD"""
    if W is None:
        W = np.eye(A.shape[0])
    
    # 获取第一列
    u1 = A[:, 0].reshape(-1, 1)
    
    # 初始化
    Q, Sigma, R = InitializeISVD(u1, W)
    Q_tilde = np.eye(1)  # 初始化为单位矩阵
    R_tilde = np.eye(1)
    R_tilde_plus = np.eye(1)
    
    # 处理剩余列
    for i in range(1, A.shape[1]):
        u_new = A[:, i].reshape(-1, 1)
        Q, Sigma, R, Q_tilde, R_tilde, R_tilde_plus = UpdateISVD2(
            Q, Sigma, R, Q_tilde, R_tilde, R_tilde_plus, u_new, W, tol)
    
    return Q, Sigma, R, Q_tilde, R_tilde

# 测试与比较
if __name__ == "__main__":
    np.random.seed(42)
    np.set_printoptions(precision=4, suppress=True)
    
    # 创建测试矩阵 (5x4)
    m, n = 5, 4
    A = np.random.randn(m, n)
    W = np.eye(m)  # 单位权重矩阵
    
    print("测试矩阵 A:")
    print(A)
    print("\n" + "="*80 + "\n")
    
    # 使用增量SVD计算
    print("运行增量SVD...")
    Q_inc, Sigma_inc, R_inc, Q_tilde, R_tilde = FullyIncrementalSVD2(A, W)
    
    # 使用标准SVD计算
    print("运行标准SVD...")
    U_std, S_std, Vt_std = svd(A, full_matrices=False)
    Sigma_std = np.diag(S_std)
    V_std = Vt_std.T
    
    # 重构矩阵
    A_rec_inc = Q_inc @ Sigma_inc @ R_inc.T
    A_rec_std = U_std @ Sigma_std @ Vt_std
    
    # 计算误差
    error_inc = norm(A - A_rec_inc, 'fro')
    error_std = norm(A - A_rec_std, 'fro')
    
    # 打印结果
    print("\n增量SVD结果:")
    print("Q (shape: {}):\n{}".format(Q_inc.shape, Q_inc))
    print("\nSigma (shape: {}):\n{}".format(Sigma_inc.shape, Sigma_inc))
    print("\nR (shape: {}):\n{}".format(R_inc.shape, R_inc))
    
    print("\n标准SVD结果:")
    print("U (shape: {}):\n{}".format(U_std.shape, U_std))
    print("\nSigma (shape: {}):\n{}".format(Sigma_std.shape, Sigma_std))
    print("\nV (shape: {}):\n{}".format(V_std.shape, V_std))
    
    print("\n重构误差:")
    print("增量SVD:", error_inc)
    print("标准SVD:", error_std)
    
    # 比较奇异值
    inc_svals = np.diag(Sigma_inc)
    std_svals = S_std
    svd_diff = norm(inc_svals - std_svals) / norm(std_svals)
    
    print("\n奇异值比较:")
    print("增量SVD:", inc_svals)
    print("标准SVD:", std_svals)
    print("相对差异:", svd_diff)
    
    # 正交性检查
    ortho_inc = norm(Q_inc.T @ W @ Q_inc - np.eye(Q_inc.shape[1]), 'fro')
    ortho_std = norm(U_std.T @ U_std - np.eye(U_std.shape[1]), 'fro')
    
    print("\n正交性检查:")
    print("Q正交性误差:", ortho_inc)
    print("U正交性误差:", ortho_std)
    
    # 相对重构误差
    rel_error_inc = error_inc / norm(A, 'fro')
    rel_error_std = error_std / norm(A, 'fro')
    
    print("\n相对重构误差:")
    print("增量SVD:", rel_error_inc)
    print("标准SVD:", rel_error_std)
    
    # 检查增量SVD的R矩阵与标准V矩阵的关系
    print("\nR矩阵与标准V矩阵比较:")
    print("R (增量):\n", R_inc)
    print("V (标准):\n", V_std)
    print("R - V 差异:", norm(R_inc - V_std, 'fro'))