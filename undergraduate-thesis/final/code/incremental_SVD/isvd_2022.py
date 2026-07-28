import numpy as np

class IncrementalSVD_Zhang2022:
    def __init__(self, W=None, tol=1e-10, reorth_tol=1e-10, truncate=True):
        self.W = W
        self.tol = tol
        self.reorth_tol = reorth_tol
        self.truncate = truncate
        self.Q = None
        self.s = None
        self.R = None
        self.V_list = []
        self.Q0 = None # 内部旋转累积矩阵
        self.q = 0     # 延迟计数
        self.m = None

    def _Wmv(self, x):
        return x if self.W is None else self.W @ x

    def _Wnorm(self, x):
        return np.sqrt(max(0, np.dot(x.T.flatten(), self._Wmv(x).flatten())))

    def update(self, u):
        u = u.reshape(-1, 1)
        if self.Q is None:
            # Algorithm 1: Initialization
            p = self._Wnorm(u)
            self.s = np.array([p])
            self.Q = u / p
            self.R = np.array([[1.0]])
            self.Q0 = np.eye(1)
            self.m = u.size
            self.q = 1
            return self

        # 1. 投影与残差计算[cite: 3]
        d = self.Q.T @ self._Wmv(u)
        e = u - self.Q @ d
        p = self._Wnorm(e)

        # 2. 处理线性相关列 (Algorithm 10, Line 2)[cite: 3]
        if p < self.tol:
            self.q += 1
            self.V_list.append(d) # 注意：此处直接存 d，由 Flush 统一处理坐标系[cite: 3]
            return self

        # 3. 秩增加更新前，首先刷新延迟列 (Algorithm 10, Lines 6-12)[cite: 3]
        if len(self.V_list) > 0:
            V_mat = np.column_stack(self.V_list)
            Y_deferred = np.column_stack([np.diag(self.s), V_mat])
            Uy, sy, Vty = np.linalg.svd(Y_deferred, full_matrices=False)
            
            # 更新基旋转和奇异值[cite: 3]
            self.Q = self.Q @ (self.Q0 @ Uy)
            self.s = sy
            self.Q0 = np.eye(self.s.size) # 立即重置 Q0[cite: 3]
            
            # 更新 R[cite: 3]
            k_old = Uy.shape[0]
            self.R = np.vstack([self.R @ Vty.T[:k_old, :], Vty.T[k_old:, :]])
            
            # 重新计算投影 d[cite: 3]
            d = self.Q.T @ self._Wmv(u)
            self.V_list = []
            self.q = 0

        # 4. 执行秩增加更新 (Algorithm 10, Lines 13-25)[cite: 3]
        e_unit = e / p
        # 增量重正交化：仅针对新向量 e[cite: 3]
        if abs(np.dot(e_unit.T, self._Wmv(self.Q[:, [0]]))) > self.reorth_tol:
            e_unit = e_unit - self.Q @ (self.Q.T @ self._Wmv(e_unit))
            e_unit /= self._Wnorm(e_unit)

        k = self.s.size
        Y = np.zeros((k + 1, k + 1))
        np.fill_diagonal(Y[:k, :k], self.s)
        Y[:k, k] = d.flatten()
        Y[k, k] = p
        
        Uy, sy, Vty = np.linalg.svd(Y, full_matrices=False)

        # 核心逻辑：应用 Q0 并重置 (Algorithm 10, Line 20/23)[cite: 3]
        temp_Q0 = np.eye(k + 1)
        temp_Q0[:k, :k] = self.Q0
        current_Q0 = temp_Q0 @ Uy

        if sy[-1] > self.tol or not self.truncate:
            # 不截断：Q = [Q|e] * current_Q0, 然后 Q0 = I[cite: 3]
            self.Q = np.column_stack([self.Q, e_unit]) @ current_Q0
            self.s = sy
            R_ext = np.zeros((self.R.shape[0] + 1, k + 1))
            R_ext[:self.R.shape[0], :k] = self.R
            R_ext[-1, -1] = 1.0
            self.R = R_ext @ Vty.T
            self.Q0 = np.eye(k + 1) # 必须重置[cite: 3]
        else:
            # 截断：仅保留前 k 个成分[cite: 3]
            self.Q = np.column_stack([self.Q, e_unit]) @ current_Q0[:, :k]
            self.s = sy[:k]
            R_ext = np.zeros((self.R.shape[0] + 1, k + 1))
            R_ext[:self.R.shape[0], :k] = self.R
            R_ext[-1, -1] = 1.0
            self.R = R_ext @ Vty.T[:, :k]
            self.Q0 = np.eye(k) # 必须重置[cite: 3]

        self.V_list = []
        self.q = 1
        return self

    def finalize(self):
        """处理最后可能遗留的 V 缓存[cite: 3]"""
        if len(self.V_list) > 0:
            V_mat = np.column_stack(self.V_list)
            Y = np.column_stack([np.diag(self.s), V_mat])
            Uy, sy, Vty = np.linalg.svd(Y, full_matrices=False)
            self.Q = self.Q @ (self.Q0 @ Uy)
            self.s = sy
            k_old = Uy.shape[0]
            self.R = np.vstack([self.R @ Vty.T[:k_old, :], Vty.T[k_old:, :]])
        else:
            self.Q = self.Q @ self.Q0
        self.Q0 = np.eye(self.s.size)
        return self.Q, self.s, self.R

    def orthogonality_error(self):
        if self.Q is None: return 0.0
        # 如果 Q0 已重置为 I，则 Q 就是当前基[cite: 1, 3]
        Q_eff = self.Q @ self.Q0
        G = Q_eff.T @ self._Wmv(Q_eff)
        return np.linalg.norm(np.eye(G.shape[0]) - G)