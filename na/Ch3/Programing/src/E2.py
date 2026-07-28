import numpy as np
import matplotlib.pyplot as plt

def load_curve_data(filename):
    """Load curve data from a file."""
    data = np.loadtxt(filename, delimiter=",")
    return data[:, 0], data[:, 1]
    

def plot_r2_curves(N_list, output_filenames, output_image):
    """Plot curves for different N values."""
    plt.figure(figsize=(10, 6))

    # Plot fitted cubic splines for each N
    for N, filename in zip(N_list, output_filenames):
        x, y = load_curve_data(filename)
        plt.plot(x, y, label=f"Cubic Spline (N={N})")

    #连接最后一个点和第一个点
    

    plt.title("Curve Fitting for Different N Values")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.axis("equal")
    plt.legend()
    plt.grid()
    plt.savefig("../pics/E2"+output_image)

if __name__ == "__main__":
    # N values and corresponding filenames
    N_list = [10, 40, 160]
    output_filenames = ["r2_2_curve_N10.csv", "r2_2_curve_N40.csv", "r2_2_curve_N160.csv"]
    output_image = "r2_curve_comparison.png"

    # Plot the curves
    plot_r2_curves(N_list, output_filenames, output_image)

    output_filenames2 = ["r2_1_curve_N10.csv", "r2_1_curve_N40.csv", "r2_1_curve_N160.csv"]
    output_image2 = "r2_curve_comparison_2.png"

    # Plot the curves
    plot_r2_curves(N_list, output_filenames2, output_image2)
