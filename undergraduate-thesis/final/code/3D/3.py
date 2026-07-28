import numpy as np
import scipy.sparse as sp
import scipy.sparse.linalg as spla
import time
import tracemalloc
from tqdm import tqdm
import matplotlib.pyplot as plt

# ==========================================
# 1. 物理模型与网格参数设置
# ==========================================
nx, ny = 60, 60
N_dofs = nx * ny
h = 1.0 / (nx + 1)
n_snapshots = 1000

print(f"=== 2D PDE 模型降阶 (ROM) 实验 ===")
print(f"方程: -∇·((1 + μ1*x + μ2*y + μ3*x*y)∇u) = 1")
print(f"网格: {nx}x{ny}, 自由度: {N_dofs}, 快照数: {n_snapshots}\n")

# ==========================================
# 2. 离线阶段：组装仿射刚度矩阵 K0, K1, K2, K3
# ==========================================
def build_affine_matrices():
    print("-> 正在组装仿射刚度矩阵 K0, K1, K2, K3 (离线阶段)...")
    K0 = sp.dok_matrix((N_dofs, N_dofs))
    K1 = sp.dok_matrix((N_dofs, N_dofs))
    K2 = sp.dok_matrix((N_dofs, N_dofs))
    K3 = sp.dok_matrix((N_dofs, N_dofs))
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
            add_entry(K3, idx, xr*y, xl*y, x*yt, x*yb)

    return K0.tocsr(), K1.tocsr(), K2.tocsr(), K3.tocsr(), F

t0 = time.time()
K0, K1, K2, K3, F = build_affine_matrices()
print(f"   完成! 耗时: {time.time() - t0:.3f} 秒\n")

# ==========================================
# 3. 在线阶段：采样与生成快照数据
# ==========================================
print("-> 正在生成快照矩阵 U_train...")
np.random.seed(42)
mu1_samples = np.random.uniform(-0.2, 0.2, n_snapshots)
mu2_samples = np.random.uniform(-0.2, 0.2, n_snapshots)
mu3_samples = np.random.uniform(-0.2, 0.2, n_snapshots)

U_train = np.zeros((N_dofs, n_snapshots))

t_snap_start = time.time()
for k in tqdm(range(n_snapshots), desc="PDE Solves", unit="snapshot"):
    mu1, mu2, mu3 = mu1_samples[k], mu2_samples[k], mu3_samples[k]
    K_total = K0 + mu1 * K1 + mu2 * K2 + mu3 * K3
    U_train[:, k] = spla.spsolve(K_total, F)
print(f"   快照生成完成! 总耗时: {time.time() - t_snap_start:.2f} 秒\n")

# ==========================================
# 4. 根据奇异值动态确定截断阶数（保留奇异值 > max_sv * 0.01 的模态）
# ==========================================
print("-> 计算全部快照的奇异值以确定截断阈值...")
U_full, S_full, _ = np.linalg.svd(U_train, full_matrices=False)
threshold = S_full[0] * 0.01
rank_truncation = np.sum(S_full > threshold)
print(f"   最大奇异值: {S_full[0]:.4f}, 阈值: {threshold:.4f}")
print(f"   保留的奇异值个数 (r) = {rank_truncation}\n")

# ==========================================
# 5. 降阶模型对比: Batch SVD vs. ISVD（使用动态确定的截断阶数）
# ==========================================

# --- 实验 A：全量 SVD (Batch SVD) ---
print("=== 实验 A: 传统全量 SVD (Batch SVD) ===")
tracemalloc.start()
t_batch_start = time.time()

U_batch, S_batch, Vt_batch = np.linalg.svd(U_train, full_matrices=False)
Q_batch = U_batch[:, :rank_truncation]

t_batch_time = time.time() - t_batch_start
_, batch_peak_mem = tracemalloc.get_traced_memory()
tracemalloc.stop()

ortho_err_batch = np.linalg.norm(Q_batch.T @ Q_batch - np.eye(rank_truncation), ord='fro')
print(f" [*] 计算耗时: {t_batch_time:.3f} 秒")
print(f" [*] 峰值内存: {batch_peak_mem / 1024 / 1024:.2f} MB")
print(f" [*] 正交性误差 ||Q^T Q - I||_F : {ortho_err_batch:.2e}\n")

# --- 实验 B：基于 Zhang (2022) 算法的分块增量 SVD (Block ISVD) ---
print("=== 实验 B: Zhang (2022) 增量 SVD ===")
tracemalloc.start()
t_isvd_start = time.time()

block_size = 50
n_blocks = n_snapshots // block_size
Q_isvd = None
S_isvd = None

for b in tqdm(range(n_blocks), desc="ISVD (Zhang 2022)", unit="block"):
    U_block = U_train[:, b*block_size : (b+1)*block_size]
    
    if Q_isvd is None:
        Q_isvd, S_isvd, _ = np.linalg.svd(U_block, full_matrices=False)
        Q_isvd = Q_isvd[:, :rank_truncation]
        S_isvd = np.diag(S_isvd[:rank_truncation])
    else:
        L = Q_isvd.T @ U_block
        H = U_block - Q_isvd @ L
        J, K_mat = np.linalg.qr(H)
        top = np.hstack([S_isvd, L])
        bottom = np.hstack([np.zeros((K_mat.shape[0], S_isvd.shape[1])), K_mat])
        B = np.vstack([top, bottom])
        U_B, S_new, _ = np.linalg.svd(B, full_matrices=False)
        U_B = U_B[:, :rank_truncation]
        S_isvd = np.diag(S_new[:rank_truncation])
        Q_isvd = np.hstack([Q_isvd, J]) @ U_B

t_isvd_time = time.time() - t_isvd_start
_, isvd_peak_mem = tracemalloc.get_traced_memory()
tracemalloc.stop()

r_final = Q_isvd.shape[1] 
ortho_err_isvd = np.linalg.norm(Q_isvd.T @ Q_isvd - np.eye(r_final), ord='fro')

print(f" [*] 计算耗时: {t_isvd_time:.3f} 秒")
print(f" [*] 峰值内存: {isvd_peak_mem / 1024 / 1024:.2f} MB")
print(f" [*] 正交性误差 ||Q^T Q - I||_F : {ortho_err_isvd:.2e}\n")

print("=== 对比总结 ===")
print(f"动态截断阶数 r = {rank_truncation} (阈值 = 最大奇异值 × 0.01)")
print(f"ISVD 节省内存约: {100 - (isvd_peak_mem / batch_peak_mem * 100):.1f} %")

# ==========================================
# 6. 画图对比展示（柱状图）
# ==========================================
print("\n-> 正在绘制结果对比图...")
fig, axes = plt.subplots(1, 3, figsize=(15, 5))
methods = ['Batch SVD', 'Block ISVD']
colors = ['#4A90E2', '#E94A5A']

# 图 1: 用时对比
times = [t_batch_time, t_isvd_time]
axes[0].bar(methods, times, color=colors, alpha=0.85)
axes[0].set_title('Compute Time Comparison', fontsize=12)
axes[0].set_ylabel('Time (Seconds)')
for i, v in enumerate(times):
    axes[0].text(i, v + 0.05, f"{v:.2f}s", ha='center', va='bottom', fontweight='bold')

# 图 2: 内存对比
mems = [batch_peak_mem / 1024**2, isvd_peak_mem / 1024**2]
axes[1].bar(methods, mems, color=colors, alpha=0.85)
axes[1].set_title('Peak Memory Usage', fontsize=12)
axes[1].set_ylabel('Memory (MB)')
for i, v in enumerate(mems):
    axes[1].text(i, v + (max(mems)*0.02), f"{v:.1f} MB", ha='center', va='bottom', fontweight='bold')

# 图 3: 正交性误差对比 (对数坐标轴)
errors = [ortho_err_batch, ortho_err_isvd]
axes[2].bar(methods, errors, color=colors, alpha=0.85)
axes[2].set_yscale('log')
axes[2].set_title('Orthogonality Error $||Q^T Q - I||_F$', fontsize=12)
axes[2].set_ylabel('Error (Log Scale)')
for i, v in enumerate(errors):
    axes[2].text(i, v * 1.5, f"{v:.1e}", ha='center', va='bottom', fontweight='bold')

plt.tight_layout()
plt.savefig('rom_comparison_dynamic_r.png', dpi=300, bbox_inches='tight')
print("-> 绘图完成！图表已保存至当前目录下的 'rom_comparison_dynamic_r.png'")
# plt.show()