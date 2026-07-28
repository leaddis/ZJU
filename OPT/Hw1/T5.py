import numpy as np
import scipy.sparse

# 参数
m = 512
n = 1024
r = 0.1
mu = 1e-2
delta = 1e-3 * mu

# 生成随机矩阵A和稀疏向量x
A = np.random.randn(m, n)
x_true = scipy.sparse.random(n, 1, density=r).toarray()
b = (A @ x_true).flatten()  # Ensure b is 1D

# 目标函数
def L_delta(x, delta):
    return np.where(np.abs(x) < delta, 0.5 * (1/delta) * x**2, np.abs(x) - 0.5 * delta)

def grad_L_delta(x, delta):
    return np.where(np.abs(x) < delta, (1/delta) * x, np.sign(x))

def f(x, A, b, mu, delta):
    return 0.5 * np.linalg.norm(A @ x - b)**2 + mu * np.sum(L_delta(x, delta))

def grad_f(x, A, b, mu, delta):
    return A.T @ (A @ x - b) + mu * grad_L_delta(x, delta)

# 最速下降法
def steepest_descent(A, b, mu, delta, tol=1e-6, max_iter=1000):
    x = np.zeros(A.shape[1])
    for k in range(max_iter):
        grad = grad_f(x, A, b, mu, delta)
        alpha = 1e-3  # 固定步长或使用回溯线搜索
        x_new = x - alpha * grad
        if np.linalg.norm(x_new - x) < tol:
            break
        x = x_new
    return x

# Barzilai-Borwein法
def barzilai_borwein(A, b, mu, delta, tol=1e-6, max_iter=1000):
    x = np.zeros(A.shape[1])
    grad = grad_f(x, A, b, mu, delta)
    for k in range(1, max_iter):
        x_new = x - 1e-3 * grad  # 初始步长
        grad_new = grad_f(x_new, A, b, mu, delta)
        s = x_new - x
        y = grad_new - grad
        alpha = np.dot(s, s) / np.dot(s, y)
        x = x_new - alpha * grad_new
        if np.linalg.norm(x_new - x) < tol:
            break
        x = x_new
        grad = grad_new
    return x

# 使用两种方法求解
x_sd = steepest_descent(A, b, mu, delta)
x_bb = barzilai_borwein(A, b, mu, delta)

# 打印结果
print("Steepest Descent Solution:")
print(x_sd)
print("Barzilai-Borwein Solution:")
print(x_bb)
