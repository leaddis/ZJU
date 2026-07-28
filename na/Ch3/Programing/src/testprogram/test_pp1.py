# plot_spline.py

import matplotlib.pyplot as plt

# 读取样条插值数据
x_spline = []
y_spline = []
with open('spline_data_pp1.txt', 'r') as f:
    for line in f:
        xi, yi = map(float, line.strip().split())
        x_spline.append(xi)
        y_spline.append(yi)

# 原始数据点
x_points = [0.0, 1.0, 2.0, 3.0, 6.0]
y_points = [0.0, 1.0, 4.0, 0.0, 7.0]

# 绘制样条曲线和原始数据点
plt.figure(figsize=(8, 6))
plt.plot(x_spline, y_spline, label='Linear Spline', color='blue')
plt.scatter(x_points, y_points, label='Data Points', color='red', zorder=5)
plt.title('Linear Spline Interpolation')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True)
plt.savefig('spline_plot_pp1.png')
