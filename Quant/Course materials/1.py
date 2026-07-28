import yfinance as yf
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# 获取数据
symbol = "AAPL"  # 你可以替换为任何股票代码
data = yf.download(symbol, start="2020-01-01", end="2023-01-01")

# 设置短期和长期均线窗口
short_window = 40
long_window = 100

# 计算均线
data['Short_MA'] = data['Close'].rolling(window=short_window, min_periods=1).mean()
data['Long_MA'] = data['Close'].rolling(window=long_window, min_periods=1).mean()

# 定义买入和卖出信号
data['Signal'] = 0.0
data['Signal'][short_window:] = np.where(data['Short_MA'][short_window:] > data['Long_MA'][short_window:], 1.0, 0.0) 
data['Position'] = data['Signal'].diff()

# 可视化
plt.figure(figsize=(14,7))
plt.plot(data['Close'], label='Close Price', alpha=0.5)
plt.plot(data['Short_MA'], label=f'{short_window} Days MA', alpha=0.7)
plt.plot(data['Long_MA'], label=f'{long_window} Days MA', alpha=0.7)

# 标记买卖点
plt.plot(data[data['Position'] == 1].index, data['Short_MA'][data['Position'] == 1], '^', markersize=10, color='g', lw=0, label='Buy Signal')
plt.plot(data[data['Position'] == -1].index, data['Short_MA'][data['Position'] == -1], 'v', markersize=10, color='r', lw=0, label='Sell Signal')

plt.title(f"{symbol} - Moving Average Crossover Strategy")
plt.xlabel('Date')
plt.ylabel('Price')
plt.legend()
plt.show()

# 计算策略收益
data['Strategy_Return'] = data['Close'].pct_change() * data['Signal'].shift(1)
data['Cumulative_Return'] = (1 + data['Strategy_Return']).cumprod()

print("策略累计收益率：", data['Cumulative_Return'].iloc[-1] - 1)
