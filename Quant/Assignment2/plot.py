import pandas as pd
import matplotlib.pyplot as plt

def plot_cumulative_return(file_path):
    """
    读取CSV文件中的数据，计算累积收益率，并绘制累积收益率图表。

    参数:
    file_path (str): CSV文件的路径
    """
    # 读取数据
    data = pd.read_csv(file_path)

    # 将日期列转换为日期格式（自动推断格式）
    data['Date'] = pd.to_datetime(data['Date'], format='mixed')

    # 计算累积收益率
    data['Cumulative Return'] = (1 + data['Hedge Return']).cumprod()

    # 绘制累积收益率图表
    plt.figure(figsize=(10, 6))
    plt.plot(data['Date'], data['Cumulative Return'], marker='o', linestyle='-', color='b')

    # 设置图表标题和标签
    plt.title('Cumulative Hedge Return Over Time')
    plt.xlabel('Date')
    plt.ylabel('Cumulative Return')

    # 显示网格
    plt.grid(True)

    # 显示图表
    plt.show()

# 调用函数，传入CSV文件路径
plot_cumulative_return('hedge_return.csv')