import matplotlib.pyplot as plt

# 读取贝塞尔曲线生成的点
def read_points(filename):
    points = []
    with open(filename, 'r') as file:
        for line in file:
            x, y = map(float, line.strip().split())
            points.append((x, y))
    return points

# 绘制曲线
def plot_curve(points, label, color):
    x_vals = [point[0] for point in points]
    y_vals = [point[1] for point in points]
    if label:  # 只为有label的曲线设置标签
        plt.plot(x_vals, y_vals, label=label, color=color)
    else:
        plt.plot(x_vals, y_vals, color=color)

# 颜色列表，对应不同的 m 值
colors = ['red', 'blue', 'green']

# 读取三个不同m值下的贝塞尔曲线点并绘制
m_values = [10, 40, 160]

for idx, m in enumerate(m_values):
    color = colors[idx]  # 选择对应颜色
    filename1 = f"heart_bezier_m_{m}positive.txt"
    points1 = read_points(filename1)
    plot_curve(points1, f"m = {m}", color)  # 正曲线有标签
    
    filename2 = f"heart_bezier_m_{m}negative.txt"
    points2 = read_points(filename2)
    plot_curve(points2, None, color)  # 负曲线没有标签

plt.title("Bezier Curve Approximation of Heart Shape")
plt.xlabel("X")
plt.ylabel("Y")
plt.legend()
plt.grid(True)
figurename = "../report/F.png"
plt.savefig(figurename)
