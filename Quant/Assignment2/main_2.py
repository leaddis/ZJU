import pandas as pd
import os
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import warnings
from scipy.stats import spearmanr
import statsmodels.api as sm #回归
import warnings


def convert_date_format():
    """
    转变日期格式
    """
    # 处理日期在第一行的情况
    for key in ['amt_day', 'close_adj_day', 'turn_day']:
        if key in data:
            data[key].columns = pd.to_datetime(data[key].columns)

    data['day']['date'] = pd.to_datetime(data['day']['date'])
    data['month']['date'] = pd.to_datetime(data['month']['date'])
    data['IPO_date_info']['IPO_date'] = pd.to_datetime(data['IPO_date_info']['IPO_date'])
    data['delist_date_info']['delist_date'] = pd.to_datetime(data['delist_date_info']['delist_date']).fillna(pd.Timestamp.max)
    
def filter_stock_pool(data, cross_section_date):
    """
    筛选股票池
    
    Parameters:
    data: 所有数据
    cross_section_date: 截面日期
    
    Returns:
    符合条件的股票列表
    """
    cross_section_date = pd.to_datetime(cross_section_date)

    # 获取上市和退市日期信息
    ipo_date = data['IPO_date_info']
    delist_date = data['delist_date_info']
    
    # 1. 剔除上市不满252个交易日的股票
    traded_days = (cross_section_date - ipo_date['IPO_date']).dt.days
    eligible_stocks = ipo_date.index[traded_days >= 252].tolist()
    #print(f"After filtering by trading days (>252): {len(eligible_stocks)} stocks remaining.")
    
    # 2. 剔除已退市的股票
    delisted_stocks = delist_date[delist_date['delist_date'] <= cross_section_date].index.tolist()
    eligible_stocks = [stock for stock in eligible_stocks if stock not in delisted_stocks]
    #print(f"After removing delisted stocks: {len(eligible_stocks)} stocks remaining.")
    
    # 3. 剔除当月末截面日停牌股票（成交额为0）
    date_str = cross_section_date.strftime('%Y-%m-%d')
    if date_str in data['amt_day'].columns:
        turnover_series = data['amt_day'][date_str]
        eligible_stocks = [stock for stock in eligible_stocks if stock in turnover_series and turnover_series[stock] != 0]
        #print(f"After removing non-trading stocks on {date_str}: {len(eligible_stocks)} stocks remaining.")
    else:
        print(f"Warning: Date {cross_section_date} not found in amt_day columns.")
    
    # 4. 剔除 data['turn_day'] 中当月没有数据的股票
    if date_str in data['turn_day'].columns:
        turn_series = data['turn_day'][date_str]
        eligible_stocks = [stock for stock in eligible_stocks if stock in turn_series and not pd.isnull(turn_series[stock])]
        #print(f"After removing stocks with missing turn_day data on {date_str}: {len(eligible_stocks)} stocks remaining.")
    else:
        print(f"Warning: Date {cross_section_date} not found in turn_day columns.")

    #5. 剔除
    cross_section_index = data['close_adj_day'].columns.get_loc(cross_section_date)
    start_index = max(0, cross_section_index - 20)

    # 获取 start_date 对应的日期
    start_date = data['close_adj_day'].columns[start_index]

    # 剔除在 start_date 和 cross_section_date 中存在 NaN 的股票
    if start_date in data['close_adj_day'].columns:
        close_adj_last_series = data['close_adj_day'][start_date]
        eligible_stocks = [stock for stock in eligible_stocks if stock in close_adj_last_series and not pd.isnull(close_adj_last_series[stock])]
    else:
        print(f"Warning: Date {start_date} not found in close_adj_day columns.")

    if cross_section_date in data['close_adj_day'].columns:
        close_adj_series = data['close_adj_day'][cross_section_date]
        eligible_stocks = [stock for stock in eligible_stocks if stock in close_adj_series and not pd.isnull(close_adj_series[stock])]
    else:
        print(f"Warning: Date {cross_section_date} not found in close_adj_day columns.")
    return eligible_stocks



def calculate_markov_factor(data, eligible_stocks, cross_section_date, window=21):
    """
    计算马尔可夫转移因子
    
    Parameters:
    data: 所有数据
    eligible_stocks: 符合条件的股票列表
    cross_section_date: 截面日期
    window: 滚动窗口长度（默认21个交易日）
    
    Returns:
    markov_factor: 马尔可夫转移因子（Series）
    """
    # 获取截面日期对应的数据
    cross_section_index = data['close_adj_day'].columns.get_loc(cross_section_date)
    start_index = max(0, cross_section_index - window + 1)
    start_date = data['close_adj_day'].columns[start_index]

    # 提取窗口内的后复权收盘价
    close_adj_window = data['close_adj_day'].iloc[:, start_index:cross_section_index + 1]

    # 计算收益率
    returns = close_adj_window.pct_change(axis=1).iloc[:, 1:]

    # 定义价格状态（1: 上涨，0: 下跌）
    states = (returns >= 0).astype(int)

    # 初始化马尔可夫转移因子
    markov_factor = pd.Series(index=eligible_stocks, dtype=int)
    valid_stocks = []
    # 对每只股票计算转移概率
    # 用于存储符合条件的股票

    for stock in eligible_stocks:
        if stock in states.index:
            stock_states = states.loc[stock]
            
            # 检查是否包含状态 0 和 1
            if 0 in stock_states.value_counts().index and 1 in stock_states.value_counts().index:
                if len(stock_states) >= 2:  # 至少需要两个状态才能计算转移概率
                    # 计算转移概率矩阵
                    transition_matrix = pd.crosstab(
                        stock_states.shift(),  # 前一个状态
                        stock_states,         # 当前状态
                        normalize='index'     # 按行归一化
                    ).fillna(0)

                    # 提取转移概率
                    try:
                        P00 = transition_matrix.loc[0, 0]  # 下跌后继续下跌的概率
                    except KeyError:
                        P00 = 0  # 如果状态不存在，设置默认值

                    try:
                        P11 = transition_matrix.loc[1, 1]  # 上涨后继续上涨的概率
                    except KeyError:
                        P11 = 0  # 如果状态不存在，设置默认值

                    # 构建转移概率因子
                    markov_factor[stock] = P11 - P00

                    # 将该股票加入有效股票列表
                    valid_stocks.append(stock)
            else:
                print(f"Stock {stock} will be removed from eligible_stocks.")
        else:
            print(f"Stock {stock} is not in states index and will be removed from eligible_stocks.")         
        
    # 只保留有效股票的马尔可夫转移因子
    valid_markov_factor = markov_factor[valid_stocks]
    return valid_stocks ,valid_markov_factor

def backtest_markov_factor(data, eligible_stocks, cross_section_date, markov_factor):
    """
    对马尔可夫转移因子进行分层回测
    
    Parameters:
    data: 所有数据
    eligible_stocks: 符合条件的股票列表
    cross_section_date: 截面日期
    markov_factor: 马尔可夫转移因子（Series）
    
    Returns:
    group_returns: 分组收益
    hedge_return: 多空对冲收益
    """
    # 获取截面日期的后复权收盘价
    close_adj_series = data['close_adj_day'][cross_section_date]

    # 计算下期收益率
    next_date = data['close_adj_day'].columns[data['close_adj_day'].columns.get_loc(cross_section_date) + 1]
    next_close_adj_series = data['close_adj_day'][next_date]
    next_returns = (next_close_adj_series - close_adj_series) / close_adj_series

    # 对齐索引：只保留 markov_factor 和 next_returns 中共同存在的股票
    common_stocks = markov_factor.index.intersection(next_returns.index)
    markov_factor = markov_factor[common_stocks]
    next_returns = next_returns[common_stocks]

    # 按马尔可夫转移因子分组
    markov_factor = markov_factor.dropna()  # 剔除缺失值
    groups = pd.qcut(markov_factor, 5, labels=False)  # 分为5组

    # 计算分组收益
    group_returns = next_returns.groupby(groups).mean()
    print("分组收益:\n", group_returns)

    # 计算多空对冲收益
    long_return = next_returns[groups == 4].mean()  # Top组
    short_return = next_returns[groups == 0].mean()  # Bottom组
    hedge_return = long_return - short_return
    print("多空对冲收益:", hedge_return)

    # 可视化
    visualize_results(group_returns, hedge_return, next_returns, groups, cross_section_date)

    return group_returns, hedge_return

def visualize_results(group_returns, hedge_return, next_returns, groups, cross_section_date):
    """
    Visualize the backtest results of grouped returns and save plots to specified folders.
    
    Parameters:
    group_returns: Grouped returns (Series)
    hedge_return: Hedge return (float)
    next_returns: Next period returns (Series)
    groups: Group labels (Series)
    cross_section_date: Current cross-section date (used for file naming)
    """
    # Set plot style
    sns.set(style="whitegrid")

    # Create directories if they don't exist
    os.makedirs("Bar plot", exist_ok=True)
    os.makedirs("Box plot", exist_ok=True)

    # 1. Bar plot for group returns
    plt.figure(figsize=(10, 6))
    group_returns.plot(kind='bar', color='skyblue')
    plt.title(f"Group Returns - {cross_section_date}")
    plt.xlabel("Group")
    plt.ylabel("Average Return")
    plt.xticks(rotation=0)
    
    # Save bar plot
    bar_plot_path = os.path.join("Bar plot", f"{cross_section_date}.png")
    plt.savefig(bar_plot_path, bbox_inches="tight")
    plt.close()  # Close the figure to free memory

    # 2. Box plot for group return distribution
    plt.figure(figsize=(10, 6))
    sns.boxplot(x=groups, y=next_returns, hue=groups, palette="Set2", legend=False)
    plt.title(f"Group Return Distribution - {cross_section_date}")
    plt.xlabel("Group")
    plt.ylabel("Return")
    
    # Save box plot
    box_plot_path = os.path.join("Box plot", f"{cross_section_date}.png")
    plt.savefig(box_plot_path, bbox_inches="tight")
    plt.close()  # Close the figure to free memory


def save_hedge_return(cross_section_date, hedge_return):
    """
    保存 Hedge Return 到 CSV 文件。
    
    Parameters:
    cross_section_date: 回测日期
    hedge_return: 多空对冲收益
    """
    # 如果文件不存在，创建一个空的 DataFrame
    if not os.path.exists(HEDGE_RETURN_FILE):
        df = pd.DataFrame(columns=["Date", "Hedge Return"])
    else:
        df = pd.read_csv(HEDGE_RETURN_FILE)

    # 添加新的 Hedge Return
    new_row = pd.DataFrame({"Date": [cross_section_date], "Hedge Return": [hedge_return]})
    df = pd.concat([df, new_row], ignore_index=True)

    # 保存到 CSV 文件
    df.to_csv(HEDGE_RETURN_FILE, index=False)

def load_hedge_return():
    """
    从 CSV 文件加载 Hedge Return 历史数据。
    
    Returns:
    hedge_return_history: Hedge Return 历史数据（DataFrame）
    """
    if os.path.exists(HEDGE_RETURN_FILE):
        return pd.read_csv(HEDGE_RETURN_FILE)
    else:
        return pd.DataFrame(columns=["Date", "Hedge Return"])








if __name__ == "__main__":
    warnings.filterwarnings("ignore", category=UserWarning)
    HEDGE_RETURN_FILE = "hedge_return.csv"
    HEDGE_RETURN_PLOT_DIR = "Hedge Return Plots"
    # 定义需要用到的CSV文件
    data_files = {
        "amt_day": "amt_day.csv",
        "close_adj_day": "close_adj_day.csv",
        "turn_day": "turn_day.csv",
        "IPO_date_info": "IPO_date_info.csv",
        "delist_date_info": "delist_date_info.csv",
        "day": "day.csv",
        "month": "month.csv"
    }

    # 数据路径
    data_path = "../Reference/ass_1_Data/data"
    hedge_return_history = pd.Series(dtype=float)
    # 读取需要用到的CSV文件
    data = {}
    for key, file in data_files.items():
        file_path = os.path.join(data_path, file)
        data[key] = pd.read_csv(file_path, parse_dates=True, index_col=0)
        print(f"{file} reading finished!")

    # 转换日期格式
    convert_date_format()

    # 定义时间范围
    start_date = pd.to_datetime('2013-12-31')  # 调整为数据文件中存在的日期
    end_date = pd.to_datetime('2017-02-01')
    month = data['month']
    monthly = month[(month['date'] >= start_date) & (month['date'] <= end_date)]
    monthly_dates = pd.to_datetime(monthly['date'].dt.strftime('%Y-%m-%d').unique())

    # 遍历每个截面日期，筛选股票池并计算马尔可夫因子
    for cross_section_date in monthly_dates:
        print(f"\nProcessing cross-section date: {cross_section_date}")
        eligible_stocks = filter_stock_pool(data, cross_section_date)
        # # 打印eligible_stocks长度
        # print(len(eligible_stocks))
        # 计算马尔可夫转移因子
        eligible_stocks, markov_factor = calculate_markov_factor(data, eligible_stocks, cross_section_date)
        # print(len(eligible_stocks))
        # print(markov_factor)
        # 对马尔可夫转移因子进行分层回测
        group_returns, hedge_return = backtest_markov_factor(data, eligible_stocks, cross_section_date, markov_factor)
        # 保存 Hedge Return
        save_hedge_return(cross_section_date, hedge_return)
