import matplotlib.pyplot as plt
import pandas as pd

# Load data
data = pd.read_csv("C.csv")

# Plot
plt.figure(figsize=(12, 6))
plt.plot(data["x"], data["y_exact"], label="Exact Function", linestyle="--", color="black")
plt.plot(data["x"], data["y_interp_3_57"], label="Interpolation (Theorem 3.57)", color="blue")
plt.plot(data["x"], data["y_interp_3_58"], label="Interpolation (Theorem 3.58)", color="red")
plt.title("Cubic B-Spline Interpolation (Theorems 3.57 and 3.58)")
plt.xlabel("x")
plt.ylabel("f(x)")
plt.legend()
plt.grid()
plt.savefig("../pics/C_interpolation_plot.png")
