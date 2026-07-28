import pandas as pd
import os
import numpy as np
import matplotlib.pyplot as plt
import warnings
from scipy.stats import spearmanr
import statsmodels.api as sm #回归
import warnings

window = 21
############################################################################
#################################转变日期格式#################################
############################################################################
def convert_date_format():
    """
    转变日期格式
    """
    # 处理日期在第一行的情况
    for key in ['amt_day', 'close_adj_day', 'close_day', 'cs_indus_code_day', 
                'csiall_day', 'float_a_shares_day', 'pb_lf_day', 
                'share_totala_day', 'turn_day']:
        if key in data:
            data[key].columns = pd.to_datetime(data[key].columns)

    data['day']['date'] = pd.to_datetime(data['day']['date'])
    data['month']['date'] = pd.to_datetime(data['month']['date'])
    data['IPO_date_info']['IPO_date'] = pd.to_datetime(data['IPO_date_info']['IPO_date'])
    data['delist_date_info']['delist_date'] = pd.to_datetime(data['delist_date_info']['delist_date']).fillna(pd.Timestamp.max)
    
    

############################################################################
################################# 筛选股票池 #################################
############################################################################
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
    start_index = max(0, cross_section_index - window + 1)

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


############################################################################
################################# 计算因子值 #################################
############################################################################
def calculate_factors(data, eligible_stocks, cross_section_date, window=21):
    """
    计算因子值
    
    Parameters:
    data: 所有数据
    eligible_stocks: 股票池
    cross_section_date: 截面日期
    window: 窗口期长度
    
    Returns:
    因子值DataFrame
    """
    factors = pd.DataFrame(index=eligible_stocks)
    
    # 计算 return_1m
    cross_section_index = data['close_adj_day'].columns.get_loc(cross_section_date)
    start_index = max(0, cross_section_index - window + 1)
    close_adj_last = data['close_adj_day'].loc[eligible_stocks, data['close_adj_day'].columns[start_index]]
    close_adj = data['close_adj_day'].loc[eligible_stocks, cross_section_date]

    # # 打印 close_adj_last 和 close_adj 中的 NaN
    # print("close_adj_last 中的 NaN：")
    # print(close_adj_last[close_adj_last.isnull()])

    # print("close_adj 中的 NaN：")
    # print(close_adj[close_adj.isnull()])

    # # 检查 close_adj_last 是否为 0
    # print("close_adj_last 中为 0 的值：")
    # print(close_adj_last[close_adj_last == 0])

    return_1m = (close_adj - close_adj_last) / close_adj_last
    factors['return_1m'] = return_1m
    
    # 计算 turn_1m
    turn_index = data['turn_day'].columns.get_loc(cross_section_date)
    turn_start_index = max(0, turn_index - window + 1)
    turn_1m = data['turn_day'].loc[eligible_stocks, data['turn_day'].columns[turn_start_index]:cross_section_date].mean(axis=1)
    factors['turn_1m'] = turn_1m
    # # 检查 turn_1m 中的缺失值
    # missing_values_turn_1m = turn_1m.isnull()

    # # 打印缺失值的股票和对应的日期范围
    # if missing_values_turn_1m.any():
    #     print("以下股票在时间窗口内有缺失值：")
    #     print(turn_1m[missing_values_turn_1m])
    # else:
    #     print("没有缺失值。")
    # # print(turn)
    
    # 计算 std_1m
    returns = data['close_adj_day'].pct_change(axis=1)
    return_index = returns.columns.get_loc(cross_section_date)
    return_start_index = max(0, return_index - window + 1)
    recent_returns = returns.iloc[:, return_start_index:return_index + 1]
    std_1m = recent_returns.std(axis=1)
    factors['std_1m'] = std_1m

    # 计算 std_FF3factor_1m
    std_FF3factor_1m_start_index = max(0, return_start_index - 1)
    close_adj = data['close_adj_day'].loc[eligible_stocks, data['close_adj_day'].columns[std_FF3factor_1m_start_index]:cross_section_date]
    csiall_day_series = data['csiall_day'].iloc[0] 
    market_return = csiall_day_series.pct_change().dropna()
    smb, hml = calculate_SMB_HML(data, cross_section_date)
    std_FF3factor_1m = calculate_std_FF3factor_1m(close_adj.pct_change(axis=1).dropna(axis=1), market_return, smb, hml)
    factors['std_FF3factor_1m'] = std_FF3factor_1m.iloc[:, -1]

    # # 删除任意一行存在缺失值的行
    # factors_cleaned = factors.dropna()
    return factors


def calculate_SMB_HML(data, cross_section_date):
    """
    计算 SMB 和 HML 因子日收益率
    
    Parameters:
    data: 所有数据
    cross_section_date: 截面日期
    
    Returns:
    SMB 和 HML 因子日收益率 Series
    """
    # 计算 SMB 因子
    market_value = data['float_a_shares_day'].loc[:, cross_section_date]
    small_cap = market_value.quantile(0.3)
    large_cap = market_value.quantile(0.7)
    small_stocks = market_value[market_value <= small_cap].index
    large_stocks = market_value[market_value >= large_cap].index
    
    smb = (
        data['close_adj_day'].loc[small_stocks].pct_change(axis=1).mean() -
        data['close_adj_day'].loc[large_stocks].pct_change(axis=1).mean()
    )
    
    # 计算 HML 因子
    pb_ratio = data['pb_lf_day'].loc[:, cross_section_date]
    high_pb = pb_ratio.quantile(0.7)
    low_pb = pb_ratio.quantile(0.3)
    high_stocks = pb_ratio[pb_ratio >= high_pb].index
    low_stocks = pb_ratio[pb_ratio <= low_pb].index
    
    hml = (
        data['close_adj_day'].loc[high_stocks].pct_change(axis=1).mean() -
        data['close_adj_day'].loc[low_stocks].pct_change(axis=1).mean()
    )
    return smb, hml

def calculate_std_FF3factor_1m(stock_return, market_return, smb, hml):
    """
    计算 std_FF3factor_1m 因子
    
    Parameters:
    stock_return: 股票日收益率 DataFrame(股票代码为索引, 日期为列名)
    market_return: 中证全指日收益率 Series
    smb: SMB 因子日收益率 Series
    hml: HML 因子日收益率 Series
    window: 窗口期长度
    
    Returns:
    std_FF3factor_1m 因子值 DataFrame
    """
    residuals = pd.DataFrame(index=stock_return.index, columns=stock_return.columns)
    
    for stock in stock_return.index:
        X = pd.concat([market_return, smb, hml], axis=1).dropna()
        X.columns = ['market_return', 'smb', 'hml']
        y = stock_return.loc[stock].dropna()

        # 检查共同时间索引
        common_index = X.index.intersection(y.index)
        if len(common_index) == 0:
            print(f"股票 {stock} 无共同时间索引，跳过该股票。")
            continue

        # 截取共同时间索引的数据
        X = X.loc[common_index]
        y = y.loc[common_index]

        # 拟合线性模型
        model = sm.OLS(y, sm.add_constant(X))
        results = model.fit()

        # 检查残差是否有效
        if results.resid.isnull().all():
            print(f"股票 {stock} 的残差全为 NaN，请检查输入数据。")
        else:
            residuals.loc[stock, common_index] = results.resid

    # 检查 residuals 是否全为 NaN
    if residuals.isnull().all().all():
        raise ValueError("residuals 数据全为 NaN，请检查模型拟合部分。")
    
    # 确保 residuals 是数值类型
    residuals = residuals.apply(pd.to_numeric, errors='coerce')
    # 计算滚动标准差
    window = min(21, residuals.shape[1])
    std_residual = residuals.rolling(window = window,axis=1).std().dropna(axis=1)
    return std_residual

############################################################################
################################# 因子预处理 #################################
############################################################################
def preprocess_factors(factors,eligible_stocks, data, cross_section_date):
    """
    因子预处理
    
    Parameters:
    factors: 原始因子值 DataFrame，每一列是一个因子
    data: 所有数据
    cross_section_date: 截面日期
    
    Returns:
    预处理后的因子值 DataFrame
    """
    # 去极值
    factors_winsorized = factors.apply(lambda x: winsorize_factor(x))
    # 检查 factor 中的缺失值
    missing_values_factor = factors.isnull().sum()

    # 检查 factors_winsorized 中的缺失值
    missing_values_factors_winsorized = factors_winsorized.isnull().sum()

    # print("factor 中的缺失值数量:", missing_values_factor)
    # print("factors_winsorized 中的缺失值数量:", missing_values_factors_winsorized)
    
    # 行业市值中性化
    industry_dummies = pd.get_dummies(data['cs_indus_code_day'].loc[eligible_stocks, cross_section_date], dtype=int)
    market_value = np.log(data['float_a_shares_day'].loc[eligible_stocks, cross_section_date])
    market_value_winsorized = winsorize_factor(market_value)
    market_value_zscore = (market_value_winsorized - market_value_winsorized.mean()) / market_value_winsorized.std()
    # 对每一列因子进行行业市值中性化
    factors_neutralized = factors_winsorized.apply(
        lambda x: industry_market_neutralization(x, industry_dummies, market_value_zscore)
    )
    
    # 标准化
    factors_standardized = factors_neutralized.apply(lambda x: (x - x.mean()) / x.std())
    
    return factors_standardized

def winsorize_factor(factor_values, winsorize_ratio=0.05):
    """
    去极值
    
    Parameters:
    factor_values: 因子值 Series
    winsorize_ratio: 去极值比例
    
    Returns:
    去极值后的因子值 Series
    """
    upper = factor_values.quantile(1 - winsorize_ratio)
    lower = factor_values.quantile(winsorize_ratio)
    factor_values[factor_values > upper] = upper
    factor_values[factor_values < lower] = lower
    return factor_values

def industry_market_neutralization(factor_values, industry_dummies, market_value):
    """
    行业市值中性化
    """
    # 确保输入数据是数值型
    factor_values = pd.to_numeric(factor_values, errors='coerce')
    market_value = pd.to_numeric(market_value, errors='coerce')
    industry_dummies = industry_dummies.astype(int) 

    # 检查缺失值
    if factor_values.isnull().any() or market_value.isnull().any() or industry_dummies.isnull().any().any():
        raise ValueError("输入数据包含缺失值，请检查数据。")

    # 构建回归模型
    X = sm.add_constant(pd.concat([market_value, industry_dummies], axis=1))
    # 检查 X 和 factor_values 的形状是否匹配
    if X.shape[0] != factor_values.shape[0]:
        raise ValueError("X 和 factor_values 的行数不匹配。")
    
    model = sm.OLS(factor_values, X)
    results = model.fit()
    factor_neutral = results.resid
    return factor_neutral

############################################################################
################################# 因子检验 #################################
############################################################################
def calculate_RankIC(factors, next_month_return):
    """
    计算 RankIC
    
    Parameters:
    factors: 因子值 DataFrame
    next_month_return: 下个自然月收益率 Series
    
    Returns:
    RankIC 值
    """
    return factors.corrwith(next_month_return, method='spearman')

def calculate_portfolio_return(factors, next_month_return, n_portfolio=5):
    """
    计算分层收益率
    
    参数:
    factors: 因子值 DataFrame
    next_month_return: 下个月收益率 Series
    n_portfolio: 分层数量
    
    返回:
    分层收益率的 Series，索引为因子名称
    """
    # 初始化一个空的字典，用于存储每个因子的分层收益率
    portfolio_returns = {}

    # 遍历 factors 的每一列（每个因子）
    for factor_name in factors.columns:
        # 确保当前因子是一维的 Series
        factor_data = factors[factor_name]
        if isinstance(factor_data, pd.DataFrame):
            factor_data = factor_data.squeeze()  # 将 DataFrame 转换为 Series
        
        # 对当前因子进行排序并分层
        # 使用 pd.qcut 将因子值分为 n_portfolio 层
        portfolios = pd.qcut(factor_data.rank(method='first'), n_portfolio, labels=False)
        
        # 按 portfolios 分组并计算每组的平均收益率
        portfolio_return = next_month_return.groupby(portfolios).mean()
        
        # 将当前因子的分层收益率存储到字典中
        portfolio_returns[factor_name] = portfolio_return.mean()  # 取所有分层的平均收益率

    # 将字典转换为 Series 并返回
    return pd.Series(portfolio_returns)

############################################################################
################################# 结果保存和可视化 #################################
############################################################################
def save_factors(factors, cross_section_date, output_dir, all_stocks):
    """
    将未处理的因子值追加到对应的 CSV 文件中，并确保行索引包含所有股票
    
    参数:
    factors: 未处理的因子值 DataFrame
    cross_section_date: 当前截面日期
    output_dir: 输出文件目录
    all_stocks: 所有股票的列表（作为行索引）
    """
    # 确保输出目录存在
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 遍历每个因子
    for factor_name in factors.columns:
        # 构造文件路径
        output_path = os.path.join(output_dir, f"{factor_name}.csv")
        
        # 如果文件已存在，读取现有数据
        if os.path.exists(output_path):
            factor_data = pd.read_csv(output_path, index_col=0)
        else:
            # 如果文件不存在，创建一个空的 DataFrame，行索引为所有股票
            factor_data = pd.DataFrame(index=all_stocks)
        
        # 添加当前日期的因子值
        date_str = cross_section_date.strftime('%Y-%m-%d')
        
        # 创建一个临时 Series，索引为所有股票，值为当前日期的因子值（缺失值用 NaN 填充）
        current_factor_values = pd.Series(index=all_stocks, name=date_str)
        current_factor_values.update(factors[factor_name])  # 更新当前日期的因子值
        
        # 将当前日期的数据添加到 factor_data
        factor_data[date_str] = current_factor_values
        
        # 保存为 CSV 文件
        factor_data.to_csv(output_path)
        print(f"Updated {output_path} with data for {date_str}")

def update_rank_ic_history(rank_ic_history, rank_ic, current_date):
    """
    更新 RankIC 历史数据，并计算累积 RankIC
    
    参数:
    rank_ic_history: 历史 RankIC 数据 DataFrame
    rank_ic: 当前日期的 RankIC 数据 Series
    current_date: 当前日期
    
    返回:
    累积的 rank_ic_cumulative
    """
    # 将当前日期的 RankIC 数据转换为 DataFrame
    rank_ic_df = rank_ic.to_frame().T  # 将 Series 转换为 DataFrame
    rank_ic_df.index = [current_date]  # 设置行索引为当前日期
    
    # 将当前日期的数据添加到历史数据中
    rank_ic_history = pd.concat([rank_ic_history, rank_ic_df])
    
    return rank_ic_history

def update_portfolio_return_history(portfolio_return_history, portfolio_return, current_date):
    """
    更新分层收益率历史数据
    
    参数:
    portfolio_return_history: 历史分层收益率数据 DataFrame
    portfolio_return: 当前日期的分层收益率数据 Series
    current_date: 当前日期
    
    返回:
    更新后的 portfolio_return_history
    """
    # 将当前日期的分层收益率数据转换为 DataFrame
    portfolio_return_df = portfolio_return.to_frame().T  # 将 Series 转换为 DataFrame
    portfolio_return_df.index = [current_date]  # 设置行索引为当前日期
    
    # 将当前日期的数据添加到历史数据中
    portfolio_return_history = pd.concat([portfolio_return_history, portfolio_return_df])

    return portfolio_return_history

def plot_rank_ic_history(rank_ic_history):
    """
    绘制 RankIC 历史数据的变化图，每个因子一张图
    """
    output_dir = 'plot'
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    # 遍历每个因子
    for factor in rank_ic_history.columns:
        plt.figure(figsize=(10, 6))  # 创建新的图形窗口
        
        # 过滤掉小于 -2 的值
        filtered_data = rank_ic_history[factor].copy()  # 复制数据以避免修改原始数据
        filtered_data[filtered_data < -2] = None  # 将小于 -2 的值设为 NaN
        
        # 绘制当前因子的变化图
        plt.plot(filtered_data.index, filtered_data, label=factor, color='blue')
        
        # 添加标题和标签
        plt.title(f'{factor} RankIC Over Time')
        plt.xlabel('Date')
        plt.ylabel('RankIC')
        plt.legend()
        plt.grid(True)
        
        # 保存图像到文件
        output_path = os.path.join(output_dir, f'{factor}_rankic.png')
        plt.savefig(output_path)
        print(f"Saved {output_path}")
        
        # 关闭当前图形窗口
        plt.close()

def plot_portfolio_return_history(portfolio_return_history):
    """
    绘制分层收益率历史数据的变化图，每个因子一张图
    """
    output_dir = 'plot'
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 遍历每个因子
    for factor in portfolio_return_history.columns:
        plt.figure(figsize=(10, 6))  # 创建新的图形窗口
        
        # 过滤掉小于 -2 的值
        filtered_data = portfolio_return_history[factor].copy()  # 复制数据以避免修改原始数据
        filtered_data[filtered_data < -2] = None  # 将小于 -2 的值设为 NaN
        
        # 绘制当前因子的变化图
        plt.plot(filtered_data.index, filtered_data, label=factor, color='blue')
        
        # 添加标题和标签
        plt.title(f'{factor} Portfolio Return Over Time')
        plt.xlabel('Date')
        plt.ylabel('Portfolio Return')
        plt.legend()
        plt.grid(True)
        
        # 保存图像到文件
        output_path = os.path.join(output_dir, f'{factor}_portfolio_return.png')
        plt.savefig(output_path)
        print(f"Saved {output_path}")
        
        # 关闭当前图形窗口
        plt.close()

def save_to_csv(data, filename, output_dir='output'):
    """
    将 DataFrame 保存为 CSV 文件
    
    参数:
    data: 要保存的 DataFrame
    filename: 保存的文件名（不需要后缀）
    output_dir: 保存文件的目录，默认为 'output'
    """
    # 如果输出目录不存在，则创建
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 构建完整的文件路径
    filepath = os.path.join(output_dir, f'{filename}.csv')
    
    # 保存为 CSV 文件
    data.to_csv(filepath, index=True)  # 保留索引（通常是日期）
    print(f"Saved {filepath}")

if __name__ == "__main__":
    # 忽略日期解析警告
    warnings.filterwarnings("ignore", category=UserWarning)
    rank_ic_history = pd.DataFrame()  # 用于存储 RankIC 历史数据
    portfolio_return_history = pd.DataFrame()  # 用于存储分层收益率历史数据
    # 定义数据路径
    data_path = "../Reference/ass_1_Data/data"

    # 加载所有CSV文件
    data_files = {
        "amt_day": "amt_day.csv",
        "close_adj_day": "close_adj_day.csv",
        "close_day": "close_day.csv",
        "cs_indus_code_day": "cs_indus_code_day.csv",
        "csiall_day": "csiall_day.csv",
        "day": "day.csv",
        "delist_date_info": "delist_date_info.csv",
        "float_a_shares_day": "float_a_shares_day.csv",
        "IPO_date_info": "IPO_date_info.csv",
        "month": "month.csv",
        "pb_lf_day": "pb_lf_day.csv",
        "share_totala_day": "share_totala_day.csv",
        "stock_code_info": "stock_code_info.csv",
        "turn_day": "turn_day.csv"
    }

    # 读取所有CSV文件
    data = {}
    for key, file in data_files.items():
        file_path = os.path.join(data_path, file)
        data[key] = pd.read_csv(file_path, parse_dates=True, index_col=0)
        print(f"{file} reading finished!")

    #转换日期格式
    convert_date_format()
    all_stocks = data['amt_day'].index.tolist()
    # 定义因子名称
    factor_names = ['return_1m', 'turn_1m', 'std_1m', 'std_FF3factor_1m']

    # 定义时间范围
    start_date = pd.to_datetime('2010-12-31')  # 调整为数据文件中存在的日期
    end_date = pd.to_datetime('2024-10-31')
    month = data['month']
    monthly = month[(month['date'] >= start_date) & (month['date'] <= end_date)]
    monthly_dates = pd.to_datetime(monthly['date'].dt.strftime('%Y-%m-%d').unique())
    # print(type(monthly_dates))
    # print(monthly_dates)
    for cross_section_date in monthly_dates:
        # 筛选股票池
        eligible_stocks = filter_stock_pool(data, cross_section_date)
        # print(type(cross_section_date))
        # print(cross_section_date)

        # 计算因子值
        factors_raw = calculate_factors(data, eligible_stocks, cross_section_date)
        save_factors(factors_raw, cross_section_date, 'factors_raw', all_stocks)
        
        # 因子预处理
        factors_processed = preprocess_factors(factors_raw,eligible_stocks, data, cross_section_date)
        save_factors(factors_processed, cross_section_date, 'factors_processed', all_stocks)

        # 计算下个月收益率
        month_next_index = month.loc[month['date'] == cross_section_date].index[0] + 1
        month_next = pd.to_datetime(month.values[month_next_index][0])
        next = data['close_adj_day'].loc[eligible_stocks, month_next]
        b4 = data['close_adj_day'].loc[eligible_stocks, cross_section_date]
        next_month_return = (next - b4) / b4
        
        # 计算 RankIC 和分层收益率
        rank_ic = calculate_RankIC(factors_processed, next_month_return)
        rank_ic_history = update_rank_ic_history(rank_ic_history, rank_ic, cross_section_date)
        portfolio_return = calculate_portfolio_return(factors_processed, next_month_return)
        portfolio_return_history = update_portfolio_return_history(portfolio_return_history, portfolio_return, cross_section_date)
    
    # 绘制 RankIC 历史数据的变化图
    plot_rank_ic_history(rank_ic_history)
    # 绘制分层收益率历史数据的变化图
    plot_portfolio_return_history(portfolio_return_history)
    # 保存 rank_ic_history 为 CSV
    save_to_csv(rank_ic_history, 'rank_ic_history')

    # 保存 portfolio_return_history 为 CSV
    save_to_csv(portfolio_return_history, 'portfolio_return_history')
print("All processing completed!")

