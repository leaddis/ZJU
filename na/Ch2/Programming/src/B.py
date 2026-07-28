import matplotlib.pyplot as plt
import numpy as np

def read_data(filename):
    x_values = []
    y_values = []
    with open(filename, 'r') as file:
        # 跳过第一行
        next(file)
        for line in file:
            x, y = map(float, line.split())
            x_values.append(x)
            y_values.append(y)
    return x_values, y_values

def plot_data(filenames):
    plt.figure(figsize=(10, 6))
    
    # 绘制函数 y = 1 / (1 + x^2)
    x_func = np.linspace(-5, 5, 500)
    y_func = 1 / (1 + x_func**2)
    plt.plot(x_func, y_func, label='y = 1 / (1 + x^2)', linestyle='--', color='black')
    
    # 绘制文件中的数据点
    for filename in filenames:
        x_values, y_values = read_data(filename)
        # 从文件名中提取 n 的值
        n = filename.split('_')[1].split('.')[0]
        label = f'n={n}'
        plt.plot(x_values, y_values, label=label)
    
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Data Points from Files and Function y = 1 / (1 + x^2)')
    plt.legend()
    plt.grid(True)
    figurename = "../report/B.png"
    plt.savefig(figurename)

if __name__ == "__main__":
    filenames = ['B_2.txt', 'B_4.txt', 'B_6.txt', 'B_8.txt']
    plot_data(filenames)