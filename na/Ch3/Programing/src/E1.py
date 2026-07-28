import numpy as np
import matplotlib.pyplot as plt

def load_curve_data(filename):
    """Load curve data from a file."""
    data = np.loadtxt(filename, delimiter=",")
    x, y = data[:, 0], data[:, 1]
    # Append the first point to the end to close the curve
    x = np.append(x, x[0])
    y = np.append(y, y[0])
    return x, y


def plot_heart_curves(N_list, output_filenames, output_image):
    """Plot heart curves for different N values."""
    plt.figure(figsize=(10, 6))

    # Plot fitted cubic splines for each N
    for N, filename in zip(N_list, output_filenames):
        x, y = load_curve_data(filename)
        plt.plot(x, y, label=f"Cubic Spline (N={N})")

    plt.title("Heart Curve Fitting for Different N Values")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.axis("equal")
    plt.legend()
    plt.grid()
    plt.savefig("../pics/E1_" + output_image)

if __name__ == "__main__":
    # N values and corresponding filenames
    N_list = [10, 40, 160]
    output_filenames = ["heart_1_curve_N10.csv", "heart_1_curve_N40.csv", "heart_1_curve_N160.csv"]
    output_image = "heart_curve_comparison.png"

    # Plot the curves
    plot_heart_curves(N_list, output_filenames, output_image)

    output_filenames2 = ["heart_2_curve_N10.csv", "heart_2_curve_N40.csv", "heart_2_curve_N160.csv"]
    output_image2 = "heart_curve_comparison_2.png"

    # Plot the curves
    plot_heart_curves(N_list, output_filenames2, output_image2)
