# plot_spline.py

import matplotlib.pyplot as plt

# 读取样条插值数据
x_spline = []
y_spline = []
with open('spline_data_pp3.txt', 'r') as f:
    for line in f:
        xi, yi = map(float, line.strip().split())
        x_spline.append(xi)
        y_spline.append(yi)

x_points = []
y_points = []

# 读取文件内容
with open('spline_data_pp3_input.txt', 'r') as f:
    lines = f.readlines()
    # 跳过描述性文本，只保留实际数据行
    data_lines = [line for line in lines if not line.startswith("输入条件") and line.strip()]
    if len(data_lines) >= 2:  # 确保有至少两行数据
        x_points = list(map(float, data_lines[0].strip().split()))
        y_points = list(map(float, data_lines[1].strip().split()))
    else:
        raise ValueError("文件内容不足两行有效数据，无法解析 x 和 y 数据。")

# 绘制样条曲线和原始数据点
plt.figure(figsize=(8, 6))
plt.plot(x_spline, y_spline, label='Cubic Spline', color='blue')
plt.scatter(x_points, y_points, label='Data Points', color='red', zorder=5)
plt.title('Cubic Spline Interpolation')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True)
plt.savefig('spline_plot_pp3.png')
