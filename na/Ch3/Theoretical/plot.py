import numpy as np
import matplotlib.pyplot as plt

# Define the B-spline basis function B^2_i(x) for different intervals
def B2_i(x, i):
    if i - 1 < x <= i:
        return ((x - i + 1) ** 2) / 2
    elif i < x <= i + 1:
        return ((x - i + 1) * (-x + i + 1) + (x - i) * (-x + i + 2)) / 2
    elif i + 1 < x <= i + 2:
        return ((-x + i + 2) ** 2) / 2
    else:
        return 0

# Define i values to plot multiple B-spline basis functions on the same plot
i_values = [1, 2, 3, 4]

# Set up the plot
plt.figure(figsize=(10, 6))

# Plot B-spline basis function for each i
for i in i_values:
    x_values = np.linspace(i - 1.5, i + 2.5, 400)
    y_values = [B2_i(x, i) for x in x_values]
    plt.plot(x_values, y_values, label=rf'$B^2_{{{i}}}(x)$')

# Add plot details
plt.title(r'Plot of B-Spline Basis Functions $B^2_i(x)$ for $i = 1, 2, 3, 4$')
plt.xlabel(r'$x$')
plt.ylabel(r'$B^2_i(x)$')
plt.legend()
plt.grid(True)
plt.show()