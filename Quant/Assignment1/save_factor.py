import pandas as pd
import os
import numpy as np

def calculate_portfolio_return(factor_values, next_month_return, n_portfolio=5):
    """
    计算分层收益率
    
    参数:
    factor_values: 当前日期的因子值 Series（索引为股票编号）
    next_month_return: 下个月收益率 Series（索引为股票编号）
    n_portfolio: 分层数量
    
    返回:
    分层收益率的 Series，索引为分层编号
    """
    # 对因子值进行排序并分层
    portfolios = pd.qcut(factor_values.rank(method='first'), n_portfolio, labels=False)
    
    # 按 portfolios 分组并计算每组的平均收益率
    portfolio_return = next_month_return.groupby(portfolios).mean()
    
    return portfolio_return

def process_factors(factors_dir, rank_ic_history_path, output_dir='factor_test_results'):
    """
    处理因子文件，计算分层收益率并保存结果
    
    参数:
    factors_dir: 因子文件所在的目录
    rank_ic_history_path: rank_ic_history.csv 文件路径
    output_dir: 输出文件目录
    """
    # 确保输出目录存在
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 读取 rank_ic_history
    rank_ic_history = pd.read_csv(rank_ic_history_path, index_col=0, parse_dates=True)
    
    # 获取所有因子文件
    factor_files = [f for f in os.listdir(factors_dir) if f.endswith('.csv')]
    
    # 遍历每个因子文件
    for factor_file in factor_files:
        # 读取因子数据
        factor_path = os.path.join(factors_dir, factor_file)
        factor_data = pd.read_csv(factor_path, index_col=0)
        
        # 初始化一个空的 DataFrame，用于存储所有日期的分层收益率
        all_portfolio_returns = pd.DataFrame()
        
        # 遍历每个日期
        for date in factor_data.columns:
            # 获取当前日期的因子值
            factor_values = factor_data[date]
            
            # 假设下个月收益率已经计算并存储在一个 DataFrame 中
            # 这里需要根据实际情况获取下个月收益率
            # 例如：next_month_return = next_month_returns[date]
            # 以下为示例代码，假设下个月收益率与因子值相同（需根据实际情况修改）
            next_month_return = factor_values  # 示例：假设下个月收益率与因子值相同
            
            # 计算分层收益率
            portfolio_returns = calculate_portfolio_return(factor_values, next_month_return)
            
            # 将当前日期的分层收益率添加到结果中
            portfolio_returns.name = date  # 设置 Series 的名称为当前日期
            all_portfolio_returns = pd.concat([all_portfolio_returns, portfolio_returns], axis=1)
        
        # 转置 DataFrame，使得列为日期，行为分层编号
        all_portfolio_returns = all_portfolio_returns.T
        
        # 设置行索引名称为日期
        all_portfolio_returns.index.name = 'Date'
        
        # 添加 RankIC 数据
        factor_name = os.path.splitext(factor_file)[0]  # 获取因子名称
        if factor_name in rank_ic_history.columns:
            all_portfolio_returns['RankIC'] = rank_ic_history[factor_name]
        else:
            # 如果因子没有对应的 RankIC 数据，则填充 NaN
            all_portfolio_returns['RankIC'] = np.nan
        
        # 保存分层收益率结果
        output_path = os.path.join(output_dir, f'{factor_name}_test.csv')
        all_portfolio_returns.to_csv(output_path)
        print(f"Saved {output_path}")

# 主程序
if __name__ == "__main__":
    # 定义因子文件目录和 rank_ic_history 文件路径
    factors_dir = 'factors_processed'  # 因子文件所在的目录
    rank_ic_history_path = 'output/rank_ic_history.csv'  # rank_ic_history.csv 文件路径
    output_dir = 'factor_test_results'  # 输出文件目录
    
    # 处理因子文件并计算分层收益率
    process_factors(factors_dir, rank_ic_history_path, output_dir)
    print("All portfolio returns calculated and saved!")