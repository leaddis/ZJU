import matplotlib.pyplot as plt
import numpy as np

# 读取同一目录下的cloud.txt文件
data = np.loadtxt('cloud.txt')  # 自动解析二维数据
x = data[:, 0]  # 提取第一列作为X轴数据
y = data[:, 1]  # 提取第二列作为Y轴数据

# 绘制散点图
plt.figure(figsize=(10, 6))
plt.scatter(x, y, s=15, alpha=0.7, color='blue', edgecolors='w')  # 添加白色描边提升辨识度

# 图表美化
plt.title('Data from cloud.txt', fontsize=14, fontweight='bold', pad=20)
plt.xlabel('X Values', fontsize=12, labelpad=10)
plt.ylabel('Y Values', fontsize=12, labelpad=10)
plt.grid(True, linestyle='--', alpha=0.6, linewidth=0.8)  # 更细的网格线

# 智能坐标轴范围（预留5%边距）
x_margin = (max(x) - min(x)) * 0.05
y_margin = (max(y) - min(y)) * 0.05
plt.xlim(min(x)-x_margin, max(x)+x_margin)
plt.ylim(min(y)-y_margin, max(y)+y_margin)

# 添加数据点数量统计（左下角）
point_count = len(x)
plt.text(0.02, 0.02, f'Total Points: {point_count}', transform=plt.gca().transAxes,
         fontsize=10, bbox=dict(facecolor='white', alpha=0.8))

plt.tight_layout()
plt.show()