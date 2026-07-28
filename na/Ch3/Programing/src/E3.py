import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def load_curve_data_3d(filename):
    """Load 3D curve data from a file."""
    data = np.loadtxt(filename, delimiter=",")
    x = data[:, 0]
    y = data[:, 1]
    z = data[:, 2]
    x = np.append(x, x[0])
    y = np.append(y, y[0])
    z = np.append(z, z[0])
    return x, y, z

def plot_r3_3d_curves(N_list, output_filenames, output_image):
    """Plot 3D curves for different N values."""
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')

    # Plot fitted cubic splines for each N
    for N, filename in zip(N_list, output_filenames):
        x, y, z = load_curve_data_3d(filename)
        ax.plot(x, y, z, label=f"Cubic Spline (N={N})")

    ax.set_title("3D Curve Fitting for Different N Values", fontsize=14)
    ax.set_xlabel("x", fontsize=12)
    ax.set_ylabel("y", fontsize=12)
    ax.set_zlabel("z", fontsize=12)
    ax.legend()
    plt.tight_layout()
    plt.savefig("../pics/E3"+output_image)

if __name__ == "__main__":
    # N values and corresponding filenames
    N_list = [10, 40, 160]
    output_filenames = ["r3_3_curve_N10.csv", "r3_3_curve_N40.csv", "r3_3_curve_N160.csv"]
    output_image = "r3_curve_comparison_3d.png"

    # Plot the 3D curves
    plot_r3_3d_curves(N_list, output_filenames, output_image)

    output_filenames2 = ["r3_1_curve_N10.csv", "r3_1_curve_N40.csv", "r3_1_curve_N160.csv"]
    output_image2 = "r3_curve_comparison_3d_2.png"

    # Plot the 3D curves
    plot_r3_3d_curves(N_list, output_filenames2, output_image2)
