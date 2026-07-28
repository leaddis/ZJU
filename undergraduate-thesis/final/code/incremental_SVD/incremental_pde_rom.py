"""
Incremental-SVD reduced-order solver for a 1D parametric elliptic PDE.

Problem
-------
Solve
    -d/dx( k(x; a) du/dx ) = f(x),   x in (0,1)
with homogeneous Dirichlet boundary conditions u(0)=u(1)=0.

Parameterization
----------------
A scalar parameter `a` controls the diffusion coefficient:
    k(x; a) = 1 + a * x
This keeps k(x;a) strictly positive for a > -1.

Workflow implemented here
-------------------------
1. Generate 500 training parameters a_train and 20 test parameters a_test.
2. Solve the full-order model (FOM) for all train/test parameters.
3. Build the snapshot matrix incrementally:
       U_k = [u(a_1), ..., u(a_k)]
   using the improved incremental SVD implementation from `incremental_svd.py`.
4. For every k >= 10:
   - finalize the current incremental SVD state,
   - choose a reduced basis dimension r by an energy threshold,
   - solve the ROM on the 20 test parameters,
   - compute the mean relative error on the test set,
   - record offline/online timings.
5. Pick the best snapshot matrix U_k according to the lowest test error
   (ties broken by smaller k).

Designed to be imported or called directly from a Jupyter notebook.
"""

from __future__ import annotations

from dataclasses import dataclass
from copy import deepcopy
from time import perf_counter
from typing import Dict, List, Optional, Tuple

import numpy as np

from incremental_svd import IncrementalSVD


@dataclass
class EvalRecord:
    k_snapshots: int
    reduced_rank: int
    energy_kept: float
    mean_rel_error: float
    max_rel_error: float
    offline_time_cum: float
    eval_time: float
    total_time: float


@dataclass
class BestModelSummary:
    best_k: int
    best_r: int
    best_mean_rel_error: float
    best_max_rel_error: float
    best_energy_kept: float
    offline_time_cum: float
    eval_time: float
    total_time: float


def gauss_legendre_2():
    xi = np.array([-1.0 / np.sqrt(3.0), 1.0 / np.sqrt(3.0)])
    w = np.array([1.0, 1.0])
    return xi, w


def coefficient(x: np.ndarray, a: float) -> np.ndarray:
    return 1.0 + a * x


def rhs_function(x: np.ndarray) -> np.ndarray:
    return np.ones_like(x)


def assemble_fem_matrices(n_elements: int, a: float) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    if a <= -1.0:
        raise ValueError("Parameter a must satisfy a > -1 so that k(x;a)=1+ax stays positive.")

    n_nodes = n_elements + 1
    x_nodes = np.linspace(0.0, 1.0, n_nodes)

    n_interior = n_nodes - 2
    A = np.zeros((n_interior, n_interior), dtype=float)
    M = np.zeros((n_interior, n_interior), dtype=float)
    b = np.zeros(n_interior, dtype=float)

    xi_q, w_q = gauss_legendre_2()
    dphi_dxi = np.array([-0.5, 0.5])

    for e in range(n_elements):
        xl, xr = x_nodes[e], x_nodes[e + 1]
        jac = (xr - xl) / 2.0

        A_loc = np.zeros((2, 2), dtype=float)
        M_loc = np.zeros((2, 2), dtype=float)
        b_loc = np.zeros(2, dtype=float)

        for xi, w in zip(xi_q, w_q):
            x = (xl + xr) / 2.0 + jac * xi
            phi = np.array([(1.0 - xi) / 2.0, (1.0 + xi) / 2.0])
            dphi_dx = dphi_dxi / jac

            kx = coefficient(np.array([x]), a)[0]
            fx = rhs_function(np.array([x]))[0]

            A_loc += w * kx * np.outer(dphi_dx, dphi_dx) * jac
            M_loc += w * np.outer(phi, phi) * jac
            b_loc += w * fx * phi * jac

        nodes = [e, e + 1]
        for i_local, i_global in enumerate(nodes):
            if i_global == 0 or i_global == n_nodes - 1:
                continue
            ii = i_global - 1
            b[ii] += b_loc[i_local]

            for j_local, j_global in enumerate(nodes):
                if j_global == 0 or j_global == n_nodes - 1:
                    continue
                jj = j_global - 1
                A[ii, jj] += A_loc[i_local, j_local]
                M[ii, jj] += M_loc[i_local, j_local]

    return A, M, b


def solve_full_order(n_elements: int, a: float) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    A, M, b = assemble_fem_matrices(n_elements=n_elements, a=a)
    u = np.linalg.solve(A, b)
    return u, A, M, b


def solve_reduced_order(A: np.ndarray, b: np.ndarray, V: np.ndarray) -> np.ndarray:
    Ar = V.T @ A @ V
    br = V.T @ b
    c = np.linalg.solve(Ar, br)
    return V @ c


def relative_error_M(u_ref: np.ndarray, u_rom: np.ndarray, M: np.ndarray) -> float:
    diff = u_ref - u_rom
    num = float(np.sqrt(diff.T @ M @ diff))
    den = float(np.sqrt(u_ref.T @ M @ u_ref))
    if den == 0.0:
        return num
    return num / den


def make_parameter_sets(
    n_train: int = 500,
    n_test: int = 20,
    a_min: float = 0.0,
    a_max: float = 2.0,
    seed: int = 42,
) -> Tuple[np.ndarray, np.ndarray]:
    rng = np.random.default_rng(seed)
    a_train = np.linspace(a_min, a_max, n_train)
    a_test = rng.uniform(a_min, a_max, size=n_test)
    return a_train, a_test


def choose_rank_by_energy(s: np.ndarray, energy_tol: float = 0.999) -> Tuple[int, float]:
    energy = np.cumsum(s**2)
    energy /= energy[-1]
    r = int(np.searchsorted(energy, energy_tol) + 1)
    return r, float(energy[r - 1])


def precompute_full_solutions(
    n_elements: int,
    a_values: np.ndarray,
) -> Tuple[List[np.ndarray], List[np.ndarray], np.ndarray, np.ndarray]:
    solutions = []
    A_list = []
    M_ref = None
    b_ref = None

    for a in a_values:
        u, A, M, b = solve_full_order(n_elements=n_elements, a=float(a))
        solutions.append(u)
        A_list.append(A)
        if M_ref is None:
            M_ref = M
            b_ref = b

    return solutions, A_list, M_ref, b_ref


def run_incremental_rom_selection(
    n_elements: int = 200,
    n_train: int = 500,
    n_test: int = 20,
    min_k: int = 10,
    a_min: float = 0.0,
    a_max: float = 2.0,
    seed: int = 42,
    svd_tol: float = 1e-10,
    energy_tol: float = 0.999,
    verbose: bool = True,
) -> Dict[str, object]:
    if min_k < 1 or min_k > n_train:
        raise ValueError("min_k must satisfy 1 <= min_k <= n_train.")

    a_train, a_test = make_parameter_sets(
        n_train=n_train,
        n_test=n_test,
        a_min=a_min,
        a_max=a_max,
        seed=seed,
    )

    t_fom_precompute_start = perf_counter()
    train_solutions, _, M, _ = precompute_full_solutions(n_elements=n_elements, a_values=a_train)
    test_solutions, test_A_list, _, test_b = precompute_full_solutions(n_elements=n_elements, a_values=a_test)
    t_fom_precompute = perf_counter() - t_fom_precompute_start

    inc = IncrementalSVD(W=M, tol=svd_tol, truncate=True)
    records: List[EvalRecord] = []

    cumulative_offline_time = 0.0
    best_record: Optional[EvalRecord] = None
    best_basis: Optional[np.ndarray] = None
    best_singular_values: Optional[np.ndarray] = None

    for k in range(1, n_train + 1):
        t0 = perf_counter()
        inc.update(train_solutions[k - 1])
        cumulative_offline_time += perf_counter() - t0

        if k < min_k:
            continue

        inc_tmp = deepcopy(inc)

        t_eval_start = perf_counter()
        core = inc_tmp.finalize()
        r, energy_kept = choose_rank_by_energy(core.s, energy_tol=energy_tol)
        V = core.Q[:, :r]

        rel_errors = []
        for u_ref, A_test in zip(test_solutions, test_A_list):
            u_rom = solve_reduced_order(A=A_test, b=test_b, V=V)
            rel_errors.append(relative_error_M(u_ref=u_ref, u_rom=u_rom, M=M))

        eval_time = perf_counter() - t_eval_start
        mean_rel_error = float(np.mean(rel_errors))
        max_rel_error = float(np.max(rel_errors))

        record = EvalRecord(
            k_snapshots=k,
            reduced_rank=r,
            energy_kept=energy_kept,
            mean_rel_error=mean_rel_error,
            max_rel_error=max_rel_error,
            offline_time_cum=cumulative_offline_time,
            eval_time=eval_time,
            total_time=cumulative_offline_time + eval_time,
        )
        records.append(record)

        if verbose and (k == min_k or k % 25 == 0 or k == n_train):
            print(
                f"[k={k:3d}] r={r:3d} | "
                f"mean err={mean_rel_error:.3e} | max err={max_rel_error:.3e} | "
                f"offline={cumulative_offline_time:.3f}s | eval={eval_time:.3f}s"
            )

        if (best_record is None) or (record.mean_rel_error < best_record.mean_rel_error - 1e-15) or (
            abs(record.mean_rel_error - best_record.mean_rel_error) <= 1e-15
            and record.k_snapshots < best_record.k_snapshots
        ):
            best_record = record
            best_basis = V.copy()
            best_singular_values = core.s.copy()

    if best_record is None:
        raise RuntimeError("No candidate model was evaluated. Check min_k and n_train.")

    best_summary = BestModelSummary(
        best_k=best_record.k_snapshots,
        best_r=best_record.reduced_rank,
        best_mean_rel_error=best_record.mean_rel_error,
        best_max_rel_error=best_record.max_rel_error,
        best_energy_kept=best_record.energy_kept,
        offline_time_cum=best_record.offline_time_cum,
        eval_time=best_record.eval_time,
        total_time=best_record.total_time,
    )

    if verbose:
        print("\n" + "=" * 78)
        print("Best incremental-SVD snapshot matrix U_k found on the 20-point test set")
        print("=" * 78)
        print(
            f"best_k = {best_summary.best_k}, "
            f"best_r = {best_summary.best_r}, "
            f"mean_rel_error = {best_summary.best_mean_rel_error:.6e}, "
            f"max_rel_error = {best_summary.best_max_rel_error:.6e}, "
            f"energy_kept = {best_summary.best_energy_kept:.6f}"
        )
        print(
            f"offline_time = {best_summary.offline_time_cum:.3f}s, "
            f"eval_time = {best_summary.eval_time:.3f}s, "
            f"total_time = {best_summary.total_time:.3f}s"
        )
        print(f"FOM precompute time (train+test full solves) = {t_fom_precompute:.3f}s")

    return {
        "a_train": a_train,
        "a_test": a_test,
        "records": records,
        "best_summary": best_summary,
        "best_basis": best_basis,
        "best_singular_values": best_singular_values,
        "M": M,
        "train_solutions": train_solutions,
        "test_solutions": test_solutions,
        "fom_precompute_time": t_fom_precompute,
    }


def records_to_arrays(records: List[EvalRecord]) -> Dict[str, np.ndarray]:
    return {
        "k": np.array([r.k_snapshots for r in records], dtype=int),
        "rank": np.array([r.reduced_rank for r in records], dtype=int),
        "energy": np.array([r.energy_kept for r in records], dtype=float),
        "mean_rel_error": np.array([r.mean_rel_error for r in records], dtype=float),
        "max_rel_error": np.array([r.max_rel_error for r in records], dtype=float),
        "offline_time_cum": np.array([r.offline_time_cum for r in records], dtype=float),
        "eval_time": np.array([r.eval_time for r in records], dtype=float),
        "total_time": np.array([r.total_time for r in records], dtype=float),
    }


def print_top_candidates(records: List[EvalRecord], top_n: int = 10):
    idx = np.argsort([r.mean_rel_error for r in records])[:top_n]
    print(f"{'rank':>4s} {'k':>6s} {'r':>6s} {'mean_err':>14s} {'max_err':>14s} {'offline(s)':>12s}")
    for j, i in enumerate(idx, 1):
        r = records[i]
        print(
            f"{j:4d} {r.k_snapshots:6d} {r.reduced_rank:6d} "
            f"{r.mean_rel_error:14.6e} {r.max_rel_error:14.6e} {r.offline_time_cum:12.4f}"
        )


def demo_run():
    result = run_incremental_rom_selection(
        n_elements=200,
        n_train=500,
        n_test=20,
        min_k=10,
        a_min=0.0,
        a_max=2.0,
        seed=42,
        svd_tol=1e-10,
        energy_tol=0.999,
        verbose=True,
    )
    print_top_candidates(result["records"], top_n=10)
    return result


if __name__ == "__main__":
    demo_run()
