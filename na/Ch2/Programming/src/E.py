import matplotlib.pyplot as plt
import numpy as np

def read_data(filename):
    x_values = []
    y_values = []
    with open(filename, 'r') as file:
        # 跳过第一行
        for _ in range(4):
            next(file)
        for line in file:
            x, y = map(float, line.split())
            x_values.append(x)
            y_values.append(y)
    return x_values, y_values

def plot_data(filenames):
    plt.figure(figsize=(10, 6))
    
    # 绘制文件中的数据点
    for filename in filenames:
        x_values, y_values = read_data(filename)
        # 从文件名中提取 n 的值
        n = filename.split('_')[1].split('.')[0]
        label = f'n={n}'
        plt.plot(x_values, y_values, label=label)

    plt.legend()
    plt.grid(True)
    figurename = "../report/E.png"
    plt.savefig(figurename)

if __name__ == "__main__":
    filenames = ['E_1.txt', 'E_2.txt']
    plot_data(filenames)