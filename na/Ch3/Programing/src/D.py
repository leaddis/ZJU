import numpy as np
import matplotlib.pyplot as plt

# Load data from errors.txt
data = np.loadtxt("errors.txt", skiprows=1)
x = data[:, 0]
y_exact = data[:, 1]
y_interp_3_57 = data[:, 2]
y_interp_3_58 = data[:, 4]
error_3_57 = data[:, 3]
error_3_58 = data[:, 5]

# Plot exact and interpolated values
plt.figure(figsize=(10, 6))
plt.plot(x, y_exact, label="Exact", color="black", linewidth=2)
plt.plot(x, y_interp_3_57, label="3rd-degree Spline", linestyle="--")
plt.plot(x, y_interp_3_58, label="2nd-degree Spline", linestyle="-.")
plt.title("Exact vs Interpolated Values")
plt.xlabel("x")
plt.ylabel("y")
plt.legend()
plt.grid(True)
plt.savefig("../pics/D_interpolation_plot.png")

# Plot errors
plt.figure(figsize=(10, 6))
plt.plot(x, error_3_57, label="Error (3rd-degree Spline)", color="blue")
plt.plot(x, error_3_58, label="Error (2nd-degree Spline)", color="red")
plt.title("Interpolation Errors")
plt.xlabel("x")
plt.ylabel("Error")
plt.legend()
plt.grid(True)
plt.savefig("../pics/D_error_plot.png")