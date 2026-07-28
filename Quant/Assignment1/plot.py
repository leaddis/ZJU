import pandas as pd
import matplotlib.pyplot as plt
import os

# 创建保存图表的文件夹
output_folder = 'over time plot'
os.makedirs(output_folder, exist_ok=True)  # 如果文件夹不存在则创建

# 读取CSV文件，假设第一行是列名
df = pd.read_csv('output/portfolio_return_history.csv', parse_dates=[0], index_col=0)

# 累加数据
df_cumsum = df.cumsum()

# 为每个因子绘制单独的图表并保存
for factor in df.columns:
    plt.figure(figsize=(10, 5))  # 创建新的图表
    df_cumsum[factor].plot(title=f'{factor} Cumulative Sum Over Time')
    plt.xlabel('Date')
    plt.ylabel('Cumulative Sum')
    plt.grid(True)  # 添加网格线
    plt.tight_layout()  # 调整布局

    # 保存图表到文件夹
    output_path = os.path.join(output_folder, f'{factor}_cumsum.png')
    plt.savefig(output_path)  # 保存图表
    plt.close()  # 关闭图表以释放内存

print(f"pr图表已保存到 '{output_folder}' 文件夹中。")

# 读取 rank_ic_history.csv 文件，假设第一行是列名
df_rank_ic = pd.read_csv('output/rank_ic_history.csv', parse_dates=[0], index_col=0)

# 累加数据
df_rank_ic_cumsum = df_rank_ic.cumsum()

# 为每个因子绘制单独的图表并保存
for factor in df_rank_ic.columns:
    plt.figure(figsize=(10, 5))  # 创建新的图表
    df_rank_ic_cumsum[factor].plot(title=f'{factor} Cumulative Rank IC Over Time')
    plt.xlabel('Date')
    plt.ylabel('Cumulative Rank IC')
    plt.grid(True)  # 添加网格线
    plt.tight_layout()  # 调整布局

    # 保存图表到文件夹
    output_path = os.path.join(output_folder, f'{factor}_rank_ic_cumsum.png')
    plt.savefig(output_path)  # 保存图表
    plt.close()  # 关闭图表以释放内存

print(f"Rank IC 图表已保存到 '{output_folder}' 文件夹中。")