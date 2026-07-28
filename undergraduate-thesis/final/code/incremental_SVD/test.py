import numpy as np
from incremental_svd import IncrementalSVD

def test_incremental_svd_dimension_growth(
    start_dim=10,
    end_dim=15,
    n_cols=4,
    seed=42,
    tol=1e-10
):
    """
    测试矩阵维度从 start_dim 增长到 end_dim 时的增量SVD效果。
    
    思路：
    - 先生成一个 start_dim x n_cols 的初始矩阵
    - 每次给矩阵底部增加一行，使行数从 10 -> 11 -> ... -> 15
    - 每次都重新调用 IncrementalSVD 对当前矩阵逐列做增量分解
    - 输出当前 SVD 结果
    
    参数
    ----
    start_dim : int
        初始行维度
    end_dim : int
        最终行维度
    n_cols : int
        矩阵列数
    seed : int
        随机种子
    tol : float
        IncrementalSVD 的容差
    """
    rng = np.random.default_rng(seed)

    # 初始矩阵
    U = rng.normal(size=(start_dim, n_cols))

    print("=" * 80)
    print("开始测试：矩阵行维度逐步增长")
    print("=" * 80)

    for m in range(start_dim, end_dim + 1):
        if m > start_dim:
            # 每次在底部加一行，模拟“维度增长”
            new_row = rng.normal(size=(1, n_cols))
            U = np.vstack([U, new_row])

        print("\n" + "-" * 80)
        print(f"当前矩阵维度: {U.shape[0]} x {U.shape[1]}")
        print("-" * 80)
        print("当前矩阵 U =")
        print(U)

        # 对当前矩阵逐列做增量SVD
        model = IncrementalSVD(W=None, tol=tol)
        for j in range(U.shape[1]):
            model.update(U[:, j])

        svd = model.finalize()

        Q = svd.Q
        s = svd.s
        R = svd.R
        U_rec = svd.reconstruct()

        err = np.linalg.norm(U - U_rec, ord="fro")

        print("\n增量SVD结果：")
        print("Q.shape =", Q.shape)
        print("s.shape =", s.shape)
        print("R.shape =", R.shape)
        print("奇异值 s =")
        print(s)

        print("\nQ =")
        print(Q)

        print("\nR =")
        print(R)

        print("\n重构矩阵 U_rec =")
        print(U_rec)

        print("\nFrobenius 重构误差 = {:.6e}".format(err))
        print("W-正交误差 = {:.6e}".format(model.orthogonality_error()))

    print("\n" + "=" * 80)
    print("测试结束")
    print("=" * 80)

def test_update_block_equivalence(
    m=12,
    n_initial=4,
    n_block=5,
    seed=123,
    tol=1e-10
):
    """
    测试 update_block(C) 与逐列 update(C[:, j]) 的结果是否一致。

    思路：
    - 先生成一个初始矩阵 U0，建立初始 SVD 状态
    - 再生成一个新增块 C
    - 方法1：逐列 update
    - 方法2：一次 update_block(C)
    - 比较两者的奇异值、重构矩阵、正交误差
    """
    rng = np.random.default_rng(seed)

    U0 = rng.normal(size=(m, n_initial))
    C = rng.normal(size=(m, n_block))
    U_all = np.hstack([U0, C])

    print("=" * 80)
    print("开始测试：update_block 与逐列 update 的一致性")
    print("=" * 80)
    print(f"初始矩阵 U0.shape = {U0.shape}")
    print(f"新增块矩阵 C.shape = {C.shape}")
    print(f"总矩阵 U_all.shape = {U_all.shape}")

    # --------------------------------------------------
    # 方法1：逐列 update
    # --------------------------------------------------
    model_col = IncrementalSVD(W=None, tol=tol)
    for j in range(U0.shape[1]):
        model_col.update(U0[:, j])
    for j in range(C.shape[1]):
        model_col.update(C[:, j])
    svd_col = model_col.finalize()
    U_rec_col = svd_col.reconstruct()
    err_col = np.linalg.norm(U_all - U_rec_col, ord="fro")
    ortho_col = model_col.orthogonality_error()

    # --------------------------------------------------
    # 方法2：update_block
    # --------------------------------------------------
    model_block = IncrementalSVD(W=None, tol=tol)
    for j in range(U0.shape[1]):
        model_block.update(U0[:, j])
    model_block.update_block(C)
    svd_block = model_block.finalize()
    U_rec_block = svd_block.reconstruct()
    err_block = np.linalg.norm(U_all - U_rec_block, ord="fro")
    ortho_block = model_block.orthogonality_error()

    # --------------------------------------------------
    # 比较结果
    # --------------------------------------------------
    s_diff = np.linalg.norm(svd_col.s - svd_block.s)
    rec_diff = np.linalg.norm(U_rec_col - U_rec_block, ord="fro")

    print("\n--- 逐列 update 结果 ---")
    print("Q.shape =", svd_col.Q.shape)
    print("s =", svd_col.s)
    print("R.shape =", svd_col.R.shape)
    print("重构误差 = {:.6e}".format(err_col))
    print("W-正交误差 = {:.6e}".format(ortho_col))

    print("\n--- update_block 结果 ---")
    print("Q.shape =", svd_block.Q.shape)
    print("s =", svd_block.s)
    print("R.shape =", svd_block.R.shape)
    print("重构误差 = {:.6e}".format(err_block))
    print("W-正交误差 = {:.6e}".format(ortho_block))

    print("\n--- 两种方法差异 ---")
    print("奇异值差异 ||s_col - s_block||_2 = {:.6e}".format(s_diff))
    print("重构矩阵差异 ||Urec_col - Urec_block||_F = {:.6e}".format(rec_diff))

    ok_s = np.allclose(svd_col.s, svd_block.s, atol=1e-8, rtol=1e-6)
    ok_rec = np.allclose(U_rec_col, U_rec_block, atol=1e-8, rtol=1e-6)

    print("\n测试结论：")
    print("奇异值是否一致：", ok_s)
    print("重构结果是否一致：", ok_rec)

    print("=" * 80)
    print("测试结束")
    print("=" * 80)


def test_update_block_from_scratch(
    m=10,
    n_total=8,
    block_sizes=(3, 2, 3),
    seed=7,
    tol=1e-10
):
    """
    测试从空模型开始，多次调用 update_block(C) 是否正确。

    block_sizes 表示分几批喂入列，例如 (3,2,3) 表示：
    - 第一批 3 列
    - 第二批 2 列
    - 第三批 3 列
    """
    rng = np.random.default_rng(seed)
    U = rng.normal(size=(m, n_total))

    if sum(block_sizes) != n_total:
        raise ValueError("sum(block_sizes) must equal n_total")

    print("=" * 80)
    print("开始测试：从空状态多次调用 update_block")
    print("=" * 80)
    print(f"总矩阵 U.shape = {U.shape}")
    print(f"分块方式 = {block_sizes}")

    model = IncrementalSVD(W=None, tol=tol)

    start = 0
    for i, bs in enumerate(block_sizes):
        end = start + bs
        C = U[:, start:end]
        print(f"\n第 {i+1} 批: C.shape = {C.shape}")
        model.update_block(C)
        start = end

    svd = model.finalize()
    U_rec = svd.reconstruct()
    err = np.linalg.norm(U - U_rec, ord="fro")

    print("\n最终结果：")
    print("Q.shape =", svd.Q.shape)
    print("s =", svd.s)
    print("R.shape =", svd.R.shape)
    print("重构误差 = {:.6e}".format(err))
    print("W-正交误差 = {:.6e}".format(model.orthogonality_error()))

    print("=" * 80)
    print("测试结束")
    print("=" * 80)

if __name__ == "__main__":
    test_incremental_svd_dimension_growth()
    print("\n\n")
    test_update_block_equivalence()
    print("\n\n")
    test_update_block_from_scratch()