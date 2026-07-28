import pandas as pd
import matplotlib.pyplot as plt

# Load data
spline_data = pd.read_csv("spline.csv")
spline1_data = pd.read_csv("spline1.csv")
spline2_data = pd.read_csv("spline2.csv")
spline3_data = pd.read_csv("spline3.csv")

# Plot 1st-order spline
plt.figure(figsize=(10, 6))
plt.plot(spline_data['x'], spline_data['value'], label="1st-order spline", linewidth=2)
plt.plot(spline_data['x'], spline_data['derivative'], linestyle='--', label="1st-order derivative")
plt.title("1st-order B-Spline")
plt.xlabel("x")
plt.ylabel("Value / Derivative")
plt.legend()
plt.grid(True)
plt.savefig('base_spline_plot_1.png')

# Plot 2nd-order spline
plt.figure(figsize=(10, 6))
plt.plot(spline1_data['x'], spline1_data['value'], label="2nd-order spline", linewidth=2)
plt.plot(spline1_data['x'], spline1_data['derivative'], linestyle='--', label="2nd-order derivative")
plt.title("2nd-order B-Spline")
plt.xlabel("x")
plt.ylabel("Value / Derivative")
plt.legend()
plt.grid(True)
plt.savefig('base_spline_plot_2.png')

# Plot 3rd-order spline
plt.figure(figsize=(10, 6))
plt.plot(spline2_data['x'], spline2_data['value'], label="2nd-order spline", linewidth=2)
plt.plot(spline2_data['x'], spline2_data['derivative'], linestyle='--', label="2nd-order derivative")
plt.title("3nd-order B-Spline")
plt.xlabel("x")
plt.ylabel("Value / Derivative")
plt.legend()
plt.grid(True)
plt.savefig('base_spline_plot_3.png')

# Plot 4th-order spline
plt.figure(figsize=(10, 6))
plt.plot(spline3_data['x'], spline3_data['value'], label="3rd-order spline", linewidth=2)
plt.plot(spline3_data['x'], spline3_data['derivative'], linestyle='--', label="3rd-order derivative")
plt.title("4rd-order B-Spline")
plt.xlabel("x")
plt.ylabel("Value / Derivative")
plt.legend()
plt.grid(True)
plt.savefig('base_spline_plot_4.png')
