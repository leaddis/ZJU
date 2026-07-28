import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# 精确函数
def f(x):
    return 1 / (1 + 25 * x ** 2)

# 加载数据
def load_data(file_prefix, node_counts):
    data = {}
    for N in node_counts:
        filename = f"{file_prefix}_N{N}.csv"
        df = pd.read_csv(filename)
        data[N] = {
            "x": df["x"].values,
            "y": df["y"].values
        }
    return data

# 绘制样条插值
def plot_splines(data, title, exact_function=True):
    x_exact = np.linspace(-1, 1, 1000)
    y_exact = f(x_exact)
    
    plt.figure(figsize=(10, 6))
    
    if exact_function:
        plt.plot(x_exact, y_exact, label="Exact Function", color="black", linewidth=1.5, linestyle="--")
    
    for N, values in data.items():
        plt.plot(values["x"], values["y"], label=f"N={N}")
    
    plt.title(title)
    plt.xlabel("x")
    plt.ylabel("f(x)")
    plt.legend()
    plt.grid()
    plt.savefig("../pics/A_"+f"{title}.png")

# 主函数
if __name__ == "__main__":
    node_counts = [6, 11, 21, 41, 81]
    
    # 加载 ppForm 数据
    pp_data = load_data("ppForm", node_counts)
    plot_splines(pp_data, "ppForm Spline Interpolation")
    
    # 加载 B-spline 数据（假设已保存）
    b_spline_data = load_data("BForm", node_counts)
    plot_splines(b_spline_data, "B-Spline Interpolation")
