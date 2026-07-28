"""
Incremental SVD implementation based on Yangwen Zhang (2022),
"An answer to an open question in the incremental SVD".

This implements the improved Algorithm III / Algorithm 9 style update:
- W-weighted core SVD: U ≈ Q @ diag(s) @ R.T, with Q.T @ W @ Q = I, R.T @ R = I
- batched handling of columns whose residual is below tol
- reorthogonalization of the newly added outer basis vector only
- final flush of deferred dependent columns

Designed to be pasted into or imported from a Jupyter notebook.

Author: OpenAI assistant
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Sequence, Union

import numpy as np

try:
    import scipy.sparse as sp  # optional
except Exception:  # pragma: no cover
    sp = None


ArrayLike = Union[np.ndarray, "sp.spmatrix"]


def _as_1d(x: np.ndarray) -> np.ndarray:
    x = np.asarray(x, dtype=float)
    if x.ndim == 2 and x.shape[1] == 1:
        x = x[:, 0]
    if x.ndim != 1:
        raise ValueError(f"Expected a vector, got shape {x.shape}")
    return x


def _svd_econ(A: np.ndarray):
    U, s, Vt = np.linalg.svd(A, full_matrices=False)
    return U, s, Vt.T


@dataclass
class CoreSVD:
    Q: np.ndarray     # m x r
    s: np.ndarray     # r
    R: np.ndarray     # n x r

    def reconstruct(self) -> np.ndarray:
        return self.Q @ (self.s[:, None] * self.R.T)

    @property
    def Sigma(self) -> np.ndarray:
        return np.diag(self.s)

    @property
    def rank(self) -> int:
        return self.s.size


class IncrementalSVD:
    """
    Incremental weighted core SVD.

    Parameters
    ----------
    W : None, ndarray, or scipy sparse matrix
        Weight matrix for the inner product <a,b>_W = a^T W b.
        If None, W = I.
    tol : float
        Tolerance used for dependent-column detection and singular-value truncation.
    reorth_tol : float or None
        Trigger for reorthogonalizing the newly appended outer basis vector.
        If None, uses tol.
    truncate : bool
        If True, drop singular values <= tol after each rank-increasing update and final flush.
    """

    def __init__(
        self,
        W: Optional[ArrayLike] = None,
        tol: float = 1e-10,
        reorth_tol: Optional[float] = None,
        truncate: bool = True,
    ):
        self.W = W
        self.tol = float(tol)
        self.reorth_tol = float(tol if reorth_tol is None else reorth_tol)
        self.truncate = bool(truncate)

        self.Q: Optional[np.ndarray] = None         # outer weighted basis
        self.s: Optional[np.ndarray] = None         # singular values
        self.R: Optional[np.ndarray] = None         # right singular vectors for processed columns

        # Algorithm III state
        self.V_list: list[np.ndarray] = []          # deferred projected coordinates
        self.Q0: Optional[np.ndarray] = None        # accumulated small orthogonal factor

        self.n_cols_seen = 0
        self.m = None

    # ---------- basic weighted linear algebra ----------
    def _Wdot(self, x: np.ndarray, y: np.ndarray) -> float:
        x = _as_1d(x)
        y = _as_1d(y)
        if self.W is None:
            return float(x @ y)
        Wy = self._Wmv(y)
        return float(x @ Wy)

    def _Wmv(self, x: np.ndarray) -> np.ndarray:
        x = np.asarray(x, dtype=float)
        if self.W is None:
            return x
        if sp is not None and sp.issparse(self.W):
            return np.asarray(self.W @ x)
        return np.asarray(self.W @ x)

    def _Wnorm(self, x: np.ndarray) -> float:
        val = self._Wdot(x, x)
        if val < 0 and abs(val) < 1e-12:
            val = 0.0
        if val < 0:
            raise ValueError(f"Weighted norm squared is negative ({val}); W may be indefinite.")
        return float(np.sqrt(val))

    def _project_coeffs(self, u: np.ndarray) -> np.ndarray:
        # d = Q^T (W u)
        if self.Q is None or self.Q.shape[1] == 0:
            return np.zeros(0)
        Wu = self._Wmv(u)
        return self.Q.T @ Wu

    def _residual_against_Q(self, u: np.ndarray) -> tuple[np.ndarray, np.ndarray, float]:
        d = self._project_coeffs(u)
        e = u - self.Q @ d if d.size else u.copy()
        p = self._Wnorm(e)
        return d, e, p

    def _reorth_new_vector(self, e: np.ndarray) -> np.ndarray:
        """
        Reorthogonalize only the newly appended vector against existing Q,
        matching the paper's recommendation for the outer basis.
        """
        if self.Q is None or self.Q.shape[1] == 0:
            nrm = self._Wnorm(e)
            return e / nrm

        trigger = abs(self._Wdot(e, self.Q[:, 0])) > self.reorth_tol
        if trigger:
            coeff = self.Q.T @ self._Wmv(e)
            e = e - self.Q @ coeff
            nrm = self._Wnorm(e)
            if nrm <= self.tol:
                raise RuntimeError("New vector collapsed during reorthogonalization.")
            e = e / nrm
        return e

    def _trim(self):
        if not self.truncate or self.s is None:
            return
        keep = self.s > self.tol
        if np.all(keep):
            return
        self.Q = self.Q[:, keep]
        self.s = self.s[keep]
        self.R = self.R[:, keep]
        if self.Q0 is not None:
            # Q0 acts in current reduced space, so reset it after trimming
            self.Q = self.Q @ self.Q0[: self.Q.shape[1], : self.Q.shape[1]]
            self.Q0 = np.eye(self.Q.shape[1])

    # ---------- initialization ----------
    def initialize(self, u1: np.ndarray):
        u1 = _as_1d(u1)
        self.m = u1.size
        sigma1 = self._Wnorm(u1)
        if sigma1 <= self.tol:
            raise ValueError("The first column is zero or numerically zero.")
        self.Q = (u1 / sigma1)[:, None]
        self.s = np.array([sigma1], dtype=float)
        self.R = np.array([[1.0]], dtype=float)
        self.Q0 = np.eye(1)
        self.V_list = []
        self.n_cols_seen = 1
        return self

    # ---------- deferred dependent-columns flush ----------
    def _flush_dependent(self):
        """
        Flush the deferred nearly-dependent columns V, updating:
            Y = [diag(s) | V]
            Q0 <- Q0 @ QY
            s  <- diag(SigmaY)
            R  <- [R @ R1; R2]
        This is the direct matrix version of Algorithm 7 / 8.
        """
        q = len(self.V_list)
        if q == 0:
            return None

        if self.Q is None or self.s is None or self.R is None or self.Q0 is None:
            raise RuntimeError("State is not initialized.")

        k = self.s.size
        V = np.column_stack(self.V_list)                    # k x q
        Y = np.hstack([np.diag(self.s), V])                # k x (k+q)

        QY, s_new, RY = _svd_econ(Y)                       # QY: k x k, RY: (k+q) x k

        R1 = RY[:k, :]                                     # k x k
        R2 = RY[k:, :]                                     # q x k

        self.Q0 = self.Q0 @ QY
        self.s = s_new
        self.R = np.vstack([self.R @ R1, R2])

        self.V_list = []
        return QY

    # ---------- main incremental update ----------
    def update(self, u: np.ndarray):
        """
        Add one new column u.
        """
        u = _as_1d(u)
        if self.Q is None:
            return self.initialize(u)

        if u.size != self.m:
            raise ValueError(f"Column size mismatch: expected {self.m}, got {u.size}")

        d, e, p = self._residual_against_Q(u)

        # Case 1: nearly dependent -> defer
        if p < self.tol:
            if self.Q0 is None:
                raise RuntimeError("Q0 is not initialized.")
            # store current reduced coordinates in the Q0 frame, as in Algorithm 7 line 4
            self.V_list.append(self.Q0.T @ d)
            self.n_cols_seen += 1
            return self

        # Case 2: rank-increasing update
        # First flush any deferred dependent columns
        if len(self.V_list) > 0:
            QY_flush = self._flush_dependent()
            if QY_flush is not None:
                d = QY_flush.T @ d

        e = e / p
        e = self._reorth_new_vector(e)

        # Small update Y = [[Sigma, d], [0, p]]
        k = self.s.size
        Y = np.block([
            [np.diag(self.s), d.reshape(-1, 1)],
            [np.zeros((1, k)), np.array([[p]])],
        ])
        QY, s_new, RY = _svd_econ(Y)                       # (k+1)x(k+1)

        # Update small accumulated factor and outer basis
        self.Q0 = np.block([
            [self.Q0, np.zeros((k, 1))],
            [np.zeros((1, k)), np.ones((1, 1))],
        ]) @ QY

        self.Q = np.column_stack([self.Q, e])
        self.s = s_new
        self.R = np.block([
            [self.R, np.zeros((self.R.shape[0], 1))],
            [np.zeros((1, self.R.shape[1])), np.ones((1, 1))],
        ]) @ RY

        self.n_cols_seen += 1

        # Optional singular-value truncation
        if self.truncate:
            self.finalize_current_basis()
            self._trim()

        return self
    
    def update_block(self, C: np.ndarray):
        C = np.asarray(C, dtype=float)
        if C.ndim != 2:
            raise ValueError("C must be a 2D array with shape (m, q).")

        if self.Q is None:
            if C.shape[1] == 0:
                raise ValueError("C must contain at least one column.")
            self.initialize(C[:, 0])
            start = 1
        else:
            if C.shape[0] != self.m:
                raise ValueError(f"Row size mismatch: expected {self.m}, got {C.shape[0]}")
            start = 0

        for j in range(start, C.shape[1]):
            self.update(C[:, j])

        return self

    def finalize_current_basis(self):
        """
        Apply the accumulated small factor Q0 to the outer basis Q.
        This is safe because Q0 is only multiplied in O(r) times in the paper's argument.
        """
        if self.Q is None or self.Q0 is None:
            return self
        self.Q = self.Q @ self.Q0
        self.Q0 = np.eye(self.Q.shape[1])
        return self

    def finalize(self):
        """
        Flush deferred columns and return the final core SVD.
        """
        if self.Q is None:
            raise RuntimeError("No data has been processed.")

        if len(self.V_list) > 0:
            self._flush_dependent()

        self.finalize_current_basis()

        if self.truncate:
            self._trim()

        return CoreSVD(Q=self.Q.copy(), s=self.s.copy(), R=self.R.copy())

    # ---------- convenience APIs ----------
    def fit(self, U: np.ndarray) -> CoreSVD:
        """
        Consume all columns of U incrementally.

        Parameters
        ----------
        U : ndarray, shape (m, n)
            Snapshot matrix, one column at a time.
        """
        U = np.asarray(U, dtype=float)
        if U.ndim != 2:
            raise ValueError("U must be a 2D array.")
        for j in range(U.shape[1]):
            self.update(U[:, j])
        return self.finalize()

    def singular_values(self) -> np.ndarray:
        if self.s is None:
            raise RuntimeError("No SVD state available.")
        if self.Q0 is None or np.allclose(self.Q0, np.eye(self.Q0.shape[0])):
            return self.s.copy()
        return self.s.copy()

    def orthogonality_error(self) -> float:
        if self.Q is None:
            return 0.0
        Q_eff = self.Q if self.Q0 is None else self.Q @ self.Q0
        G = Q_eff.T @ self._Wmv(Q_eff)
        return float(np.linalg.norm(np.eye(G.shape[0]) - G))

    def reconstruction_error(self, U: np.ndarray) -> float:
        """
        Frobenius reconstruction error on a dense matrix U.
        """
        svd = self.finalize()
        return float(np.linalg.norm(U - svd.reconstruct(), ord="fro"))


def incremental_svd(U: np.ndarray, W: Optional[ArrayLike] = None, tol: float = 1e-10,
                    reorth_tol: Optional[float] = None, truncate: bool = True) -> CoreSVD:
    """
    Functional wrapper for notebook use.
    """
    model = IncrementalSVD(W=W, tol=tol, reorth_tol=reorth_tol, truncate=truncate)
    return model.fit(U)
