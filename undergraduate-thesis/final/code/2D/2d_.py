import numpy as np
import scipy.sparse as sp
import scipy.sparse.linalg as spla
import time
import tracemalloc
from tqdm import tqdm
import matplotlib.pyplot as plt

# ==========================================
# 1. 物理模型与参数设置
# ==========================================
nx, ny = 40, 40
N_dofs = nx * ny
h = 1.0 / (nx + 1)
n_snapshots_total = 50000

# 生成评估节点
snapshot_nodes = np.unique(np.geomspace(100, n_snapshots_total, num=20).astype(int))
snapshot_nodes = [m for m in snapshot_nodes if m <= n_snapshots_total]
print(f"评估节点数量: {len(snapshot_nodes)}")

print(f"=== 增量SVD vs 批SVD：性能随快照数变化 ===")
print(f"方程: -∇·((1 + μ1*x + μ2*y)∇u) = 1")
print(f"网格: {nx}x{ny}, 自由度: {N_dofs}")
print(f"总快照数: {n_snapshots_total}, 评估节点: {snapshot_nodes}\n")

# ==========================================
# 2. 离线组装仿射刚度矩阵（K0, K1, K2）
# ==========================================
def build_affine_matrices():
    print("-> 正在组装仿射刚度矩阵 K0, K1, K2...")
    K0 = sp.dok_matrix((N_dofs, N_dofs))
    K1 = sp.dok_matrix((N_dofs, N_dofs))
    K2 = sp.dok_matrix((N_dofs, N_dofs))
    F = np.ones(N_dofs) * (h**2)

    for j in range(ny):
        for i in range(nx):
            idx = j * nx + i
            x = (i + 1) * h
            y = (j + 1) * h
            xr, xl = x + h/2, x - h/2
            yt, yb = y + h/2, y - h/2

            def add_entry(K_m, row, val_r, val_l, val_t, val_b):
                diag = val_r + val_l + val_t + val_b
                K_m[row, row] += diag
                if i < nx - 1: K_m[row, row + 1] -= val_r
                if i > 0:      K_m[row, row - 1] -= val_l
                if j < ny - 1: K_m[row, row + nx] -= val_t
                if j > 0:      K_m[row, row - nx] -= val_b

            add_entry(K0, idx, 1, 1, 1, 1)
            add_entry(K1, idx, xr, xl, x, x)
            add_entry(K2, idx, y, y, yt, yb)

    return K0.tocsr(), K1.tocsr(), K2.tocsr(), F

t0 = time.time()
K0, K1, K2, F = build_affine_matrices()
print(f"   刚度矩阵组装完成，耗时 {time.time()-t0:.3f} 秒\n")

# ==========================================
# 3. 生成全部快照（U_train 矩阵）
# ==========================================
print(f"-> 正在生成 {n_snapshots_total} 个快照...")
np.random.seed(42)
mu1_samples = np.random.uniform(-0.2, 0.2, n_snapshots_total)
mu2_samples = np.random.uniform(-0.2, 0.2, n_snapshots_total)

U_train = np.zeros((N_dofs, n_snapshots_total), dtype=np.float32)

t_snap_start = time.time()
for k in tqdm(range(n_snapshots_total), desc="PDE Solves", unit="snapshot"):
    mu1, mu2 = mu1_samples[k], mu2_samples[k]
    K_total = K0 + mu1 * K1 + mu2 * K2
    U_train[:, k] = spla.spsolve(K_total, F).astype(np.float32)
print(f"   快照生成完成！总耗时 {time.time()-t_snap_start:.2f} 秒\n")

# ==========================================
# 4. 根据奇异值阈值确定截断阶数（阈值 = 最大奇异值 × 0.1）
# ==========================================
print("-> 计算全部快照的奇异值以确定截断阈值...")
U_full, S_full, _ = np.linalg.svd(U_train, full_matrices=False)
threshold = S_full[0] * 0.1
rank_truncation = np.sum(S_full > threshold)
print(f"   最大奇异值: {S_full[0]:.4f}, 阈值: {threshold:.4f}")
print(f"   保留的奇异值个数 (r) = {rank_truncation}\n")

# ==========================================
# 5. 辅助函数：计算重建误差
# ==========================================
def reconstruction_error(U, Q):
    U_recon = Q @ (Q.T @ U)
    return np.linalg.norm(U - U_recon, 'fro') / np.linalg.norm(U, 'fro')

# ==========================================
# 6. Batch SVD 评估（每个节点独立执行一次完整 SVD）
# ==========================================
print("-> 执行 Batch SVD 评估...")
batch_times = []
batch_mems = []
batch_errors = []

for m in tqdm(snapshot_nodes, desc="Batch SVD"):
    U_m = U_train[:, :m]
    tracemalloc.start()
    t_start = time.time()
    U_svd, S, _ = np.linalg.svd(U_m, full_matrices=False)
    t_elapsed = time.time() - t_start
    _, peak_mem = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    Q = U_svd[:, :rank_truncation]
    err = reconstruction_error(U_m, Q)
    batch_times.append(t_elapsed)
    batch_mems.append(peak_mem / 1024**2)
    batch_errors.append(err)

# ==========================================
# 7. 增量 SVD (Zhang 算法) 流式评估
# ==========================================
print("\n-> 执行增量 SVD (Zhang) 流式处理...")
def incremental_svd_tracking(U, snapshot_nodes, r, block_size=100):
    Q = None
    Sigma = None
    total_time = 0.0
    node_times = []
    node_mems = []
    node_Qs = []
    node_idx = 0
    next_node = snapshot_nodes[node_idx]
    processed = 0

    tracemalloc.start()
    for start in range(0, U.shape[1], block_size):
        end = min(start + block_size, U.shape[1])
        U_block = U[:, start:end]

        t_block_start = time.time()
        if Q is None:
            Q, s, _ = np.linalg.svd(U_block, full_matrices=False)
            Q = Q[:, :r]
            Sigma = np.diag(s[:r])
        else:
            L = Q.T @ U_block
            H = U_block - Q @ L
            J, K_mat = np.linalg.qr(H)
            top = np.hstack([Sigma, L])
            bottom = np.hstack([np.zeros((K_mat.shape[0], Sigma.shape[0])), K_mat])
            B = np.vstack([top, bottom])
            U_B, s_new, _ = np.linalg.svd(B, full_matrices=False)
            U_B = U_B[:, :r]
            Sigma = np.diag(s_new[:r])
            Q = np.hstack([Q, J]) @ U_B
        t_block = time.time() - t_block_start
        total_time += t_block
        processed = end

        while node_idx < len(snapshot_nodes) and processed >= next_node:
            _, peak_mem = tracemalloc.get_traced_memory()
            node_times.append(total_time)
            node_mems.append(peak_mem / 1024**2)
            node_Qs.append(Q.copy())
            node_idx += 1
            if node_idx < len(snapshot_nodes):
                next_node = snapshot_nodes[node_idx]
            else:
                break

    tracemalloc.stop()
    return node_times, node_mems, node_Qs

isvd_times, isvd_mems, isvd_Qs = incremental_svd_tracking(
    U_train, snapshot_nodes, rank_truncation, block_size=100
)

isvd_errors = []
for i, m in enumerate(snapshot_nodes):
    Q = isvd_Qs[i]
    U_m = U_train[:, :m]
    err = reconstruction_error(U_m, Q)
    isvd_errors.append(err)

# ==========================================
# 8. 绘图
# ==========================================
print("\n-> 正在绘制性能曲线...")
fig, axes = plt.subplots(1, 3, figsize=(15, 5))

axes[0].plot(snapshot_nodes, batch_times, '-', label='Batch SVD', color='#4A90E2')
axes[0].plot(snapshot_nodes, isvd_times, '-', label='Incremental SVD (Zhang)', color='#E94A5A')
axes[0].set_xlabel('Number of snapshots')
axes[0].set_ylabel('Cumulative Time (seconds)')
axes[0].set_title('Time vs. Snapshots')
axes[0].legend()
axes[0].grid(True, linestyle='--', alpha=0.6)
axes[0].set_xscale('log')
axes[0].set_yscale('log')

axes[1].plot(snapshot_nodes, batch_mems, '-', label='Batch SVD', color='#4A90E2')
axes[1].plot(snapshot_nodes, isvd_mems, '-', label='Incremental SVD (Zhang)', color='#E94A5A')
axes[1].set_xlabel('Number of snapshots')
axes[1].set_ylabel('Peak Memory (MB)')
axes[1].set_title('Memory vs. Snapshots')
axes[1].legend()
axes[1].grid(True, linestyle='--', alpha=0.6)
axes[1].set_xscale('log')

axes[2].plot(snapshot_nodes, batch_errors, '-', label='Batch SVD', color='#4A90E2')
axes[2].plot(snapshot_nodes, isvd_errors, '-', label='Incremental SVD (Zhang)', color='#E94A5A')
axes[2].set_xlabel('Number of snapshots')
axes[2].set_ylabel('Relative Reconstruction Error')
axes[2].set_title('Accuracy vs. Snapshots')
axes[2].legend()
axes[2].grid(True, linestyle='--', alpha=0.6)
axes[2].set_xscale('log')
axes[2].set_yscale('log')

plt.tight_layout()
plt.savefig('svd_vs_snapshots_adaptive_r.png', dpi=300, bbox_inches='tight')
print("-> 曲线图已保存为 'svd_vs_snapshots_adaptive_r.png'")

print("\n=== 详细对比数据 ===")
print(f"自适应截断阶数 r = {rank_truncation} (阈值 = 最大奇异值 × 0.1)")
print(f"{'Snapshots':>10} | {'Batch Time(s)':>12} | {'Batch Mem(MB)':>13} | {'Batch Error':>12} | {'ISVD Time(s)':>12} | {'ISVD Mem(MB)':>13} | {'ISVD Error':>12}")
print("-" * 95)
for i, m in enumerate(snapshot_nodes):
    print(f"{m:10d} | {batch_times[i]:12.2f} | {batch_mems[i]:13.1f} | {batch_errors[i]:12.2e} | {isvd_times[i]:12.2f} | {isvd_mems[i]:13.1f} | {isvd_errors[i]:12.2e}")