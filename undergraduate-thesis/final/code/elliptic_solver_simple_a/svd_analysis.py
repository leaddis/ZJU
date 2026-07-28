# svd_analysis.py - SVD分析和可视化
import numpy as np
import matplotlib.pyplot as plt

def analyze_svd_results(S, rank, energy_threshold):
    """Analyze SVD results"""
    plt.figure(figsize=(15, 5))
    
    # Subplot 1: Singular value spectrum
    plt.subplot(1, 3, 1)
    plt.semilogy(range(1, len(S) + 1), S, 'bo-', markersize=4)
    plt.axvline(x=rank, color='r', linestyle='--', label=f'Truncation rank = {rank}')
    plt.xlabel('Singular value index')
    plt.ylabel('Singular values (log scale)')
    plt.title('Singular Value Spectrum')
    plt.legend()
    plt.grid(True)
    
    # Subplot 2: Cumulative energy
    plt.subplot(1, 3, 2)
    total_energy = np.sum(S ** 2)
    cumulative_energy = np.cumsum(S ** 2) / total_energy
    plt.plot(range(1, len(S) + 1), cumulative_energy, 'ro-', markersize=4)
    plt.axhline(y=energy_threshold, color='g', linestyle='--', 
                label=f'Threshold = {energy_threshold}')
    plt.axvline(x=rank, color='r', linestyle='--', label=f'Truncation rank = {rank}')
    plt.xlabel('Number of singular values')
    plt.ylabel('Cumulative energy ratio')
    plt.title('Cumulative Energy Distribution')
    plt.legend()
    plt.grid(True)
    
    # Subplot 3: First few modes
    plt.subplot(1, 3, 3)
    n_modes = min(6, rank)
    for i in range(n_modes):
        plt.plot(range(1, len(S) + 1), S * (np.arange(len(S)) == i), 
                'o-', label=f'Mode {i+1}')
    plt.xlabel('Index')
    plt.ylabel('Singular values')
    plt.title(f'First {n_modes} Modes')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('plots/svd_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"SVD analysis completed:")
    print(f"  Total number of singular values: {len(S)}")
    print(f"  Truncation rank: {rank}")
    print(f"  Energy retained: {cumulative_energy[rank-1]:.6f}")
    print(f"  Compression ratio: {len(S)/rank:.2f}x")

def compare_solutions(u_full, u_reduced, reduced_basis, title):
    """Compare full-order and reduced-order solutions"""
    u_reconstructed = reduced_basis @ u_reduced
    
    plt.figure(figsize=(12, 4))
    
    plt.subplot(1, 3, 1)
    plt.plot(u_full, 'b-', label='Full-order solution', linewidth=2)
    plt.title('Full-Order Solution')
    plt.grid(True)
    
    plt.subplot(1, 3, 2)
    plt.plot(u_reconstructed, 'r--', label='Reduced-order reconstructed solution', linewidth=2)
    plt.title('Reduced-Order Reconstructed Solution')
    plt.grid(True)
    
    plt.subplot(1, 3, 3)
    error = np.abs(u_full - u_reconstructed)
    plt.plot(error, 'g-', label='Absolute error', linewidth=2)
    plt.title(f'Absolute Error (max={np.max(error):.2e})')
    plt.grid(True)
    
    plt.suptitle(title)
    plt.tight_layout()
    plt.savefig(f'plots/{title}_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    return np.max(error), np.linalg.norm(error)