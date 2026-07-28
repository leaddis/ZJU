import pandas as pd
import matplotlib.pyplot as plt

# 读取 CSV 数据
data = pd.read_csv("A_data.csv")

# 提取 x, f(x), g(x), h(x) 的值
x = data["x"]
f_vals = data["f(x)"]
g_vals = data["g(x)"]
h_vals = data["h(x)"]

# 绘制函数值的图像
plt.figure(figsize=(10, 6))

# 绘制 f(x)
plt.plot(x, f_vals, label="f(x)", linestyle='-', marker='o', markersize=4)

# 绘制 g(x)
plt.plot(x, g_vals, label="g(x)", linestyle='--', marker='x', markersize=4)

# 绘制 h(x)
plt.plot(x, h_vals, label="h(x)", linestyle='-.', marker='s', markersize=4)

# 设置图形标题和标签
plt.title("Comparison of Functions f(x), g(x), and h(x)", fontsize=16)
plt.xlabel("x", fontsize=12)
plt.ylabel("Function Values", fontsize=12)
plt.legend(fontsize=10)
plt.grid(True)

# # 放大 y 轴范围，观察细微差异
# plt.ylim(-1e-10, 1e-10)

# 显示图像
plt.tight_layout()
plt.savefig("../report/A.png")
