import pandas as pd
import numpy as np
import os

def generate_factor_test_results(rank_ic_history_path, portfolio_return_history_path, output_dir='factor_test_results'):
    """
    生成每个因子的 RankIC 和 5 分层收益率序列，并保存为 CSV 文件
    
    参数:
    rank_ic_history_path: rank_ic_history.csv 文件路径
    portfolio_return_history_path: portfolio_return_history.csv 文件路径
    output_dir: 输出文件目录
    """
    # 确保输出目录存在
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 读取 rank_ic_history 和 portfolio_return_history
    rank_ic_history = pd.read_csv(rank_ic_history_path, index_col=0, parse_dates=True)
    portfolio_return_history = pd.read_csv(portfolio_return_history_path, index_col=0, parse_dates=True)
    
    # 遍历每个因子
    for factor_name in rank_ic_history.columns:
        # 创建一个新的 DataFrame，包含 RankIC 和 5 分层收益率
        factor_test_results = pd.DataFrame(index=rank_ic_history.index)
        
        # 添加 RankIC 数据
        factor_test_results['RankIC'] = rank_ic_history[factor_name]
        
        # 添加 5 分层收益率数据
        for i in range(5):
            portfolio_col = f'Portfolio_{i+1}'
            if portfolio_col in portfolio_return_history.columns:
                factor_test_results[portfolio_col] = portfolio_return_history[portfolio_col]
            else:
                # 如果列不存在，则填充 NaN
                factor_test_results[portfolio_col] = np.nan
        
        # 保存为 CSV 文件
        output_path = os.path.join(output_dir, f'{factor_name}_test.csv')
        factor_test_results.to_csv(output_path)
        print(f"Saved {output_path}")

# 主程序
if __name__ == "__main__":
    # 定义输入文件路径
    rank_ic_history_path = 'output/rank_ic_history.csv'  # rank_ic_history.csv 文件路径
    portfolio_return_history_path = 'output/portfolio_return_history.csv'  # portfolio_return_history.csv 文件路径
    
    # 生成因子测试结果
    generate_factor_test_results(rank_ic_history_path, portfolio_return_history_path)
    print("All factor test results generated!")