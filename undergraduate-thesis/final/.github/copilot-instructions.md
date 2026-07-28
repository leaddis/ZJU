# AI Agent Guidelines for This Repository

## 1. Project Overview & Architecture
This repository contains two main parts:
1. **Thesis LaTeX Template/Document**: Located in `paper/`. It is based on the Zhejiang University (ZJU) thesis template. 
   - `paper/config/`: Contains format settings and package inclusions.
   - `paper/content/`: Contains the actual text of the thesis (e.g., `final/`, `proposal/`).
   - `paper/pages/`: Contains frontmatter and evaluation pages.
2. **Computational Code (Model Order Reduction / SVD)**: Various Python and Jupyter notebook files under `code/`.
   - `elliptic_solver_simple_a/` & `elliptic_solver_multiple_a/`: Implementations of parametrized partial differential equations (PDE) solvers, focusing on Reduced Basis Methods (RBM).
   - `incremental_SVD/` & `SVD/`: Implementations of Incremental SVD (ISVD) algorithms for efficient reduced basis generation, comparing batch SVD vs. ISVD (e.g., Zhang 2022).
   - `/1D/` & `/2D/`: Experiments for 1D and 2D domains.

## 2. Developer Workflows & Commands

### Code / PDE Solver execution
The numerical code contains makefiles (e.g., in `code/elliptic_solver_simple_a/`).
When working in `code/elliptic_solver_simple_a/`:
- `make` or `make run_simulation`: Runs the main simulation, including solving, visualization, and convergence analysis.
- `make clean`: Cleans up cached files, pyc, and plot directories.

### LaTeX Compilation
- The main LaTeX folder is `paper/`.
- Compile the thesis by running: `latexmk -xelatex -outdir=out zjuthesis` in the `paper/` directory.

## 3. Python Code Conventions (PDE & ROM)
- **Math & Algorithms**: The core mathematical structure revolves around proper weighting (e.g., mass matrix $W$) in Reduced Order Models (ROM). When generating SVD or ROM code, ensure that the inner product is correctly weighted with $W$ when necessary (Core SVD implementations).
- **Dependencies**: The numerical simulations actively utilize `numpy`, `scipy` (for meshes and sparse matrices), `matplotlib`, and custom PDE classes.
- **Notebooks**: Make sure not to execute long-running PDE loops if limits allow, or use caching where applicable. Keep visualizations clean and presentation-ready (Thesis-level plots).

## 4. LaTeX Document Conventions
- Do not edit `.cls` files; ZJU format relies on standard `tex` configs under `paper/config/`.
- To inject packages, place them in `config/packages.tex` or the major-specific settings. 
- Math blocks and theorem proofs are critical for the thesis parts; follow standardized Chinese LaTeX conventions.

## 5. Typical AI Agent Tasks
- **Refactoring Notebooks**: Convert exploratory `.ipynb` into modular scripts or generate high-quality thesis-ready plots.
- **Proofreading / Formatting**: Verify the LaTeX document structure within `paper/content/`.
