# user_config.py
"""
用户配置文件 - 只需修改这个文件即可运行不同的问题
"""

from ngsolve import x, y, sin, pi, exp

# ========== 基础求解参数 ==========
ORDER = 2           # 有限元阶数 (1=线性, 2=二次, 3=三次)
MESH_H = 0.1        # 网格尺寸 (值越小网格越密)

# ========== SVD模型降阶配置 ==========
RUN_SVD_ANALYSIS = True        # 是否运行SVD分析
A_MIN = 0.5                    # 参数a的最小值
A_MAX = 2.0                    # 参数a的最大值
NUM_TRAINING_SAMPLES = 50      # 训练样本数量
SVD_RANK = 10                  # SVD截断秩
ENERGY_THRESHOLD = 0.999       # 能量保留阈值

# ========== 问题定义 ==========
# 选择问题类型：取消对应问题的注释

# 问题1: 简单测试问题 (默认)
PROBLEM_TYPE = "simple"
a = 1.0
f = 1.0
exact_solution = None

# 问题2: 已知精确解的标准测试
# PROBLEM_TYPE = "known_solution"
# a = 1.0
# exact_solution = x*(1-x)*y*(1-y)
# f = 2*(y*(1-y) + x*(1-x))

# 问题3: 变系数问题
# PROBLEM_TYPE = "variable_coefficient"
# a = 1.0 + 0.5*x + 0.5*y
# f = 1.0
# exact_solution = None

# 问题4: 振荡解
# PROBLEM_TYPE = "oscillatory"
# a = 1.0
# exact_solution = sin(pi*x)*sin(pi*y)
# f = 2*pi**2*sin(pi*x)*sin(pi*y)

# 问题5: 边界层问题
# PROBLEM_TYPE = "boundary_layer"
# a = 1.0
# exact_solution = x*(1-x)*y*(1-y)*exp(-10*((x-0.5)**2+(y-0.5)**2))
# # 注意：这里f需要根据精确解计算，暂时设为1，实际使用需要计算对应的f
# f = 1.0

# ========== 高级选项 (可选) ==========
RUN_CONVERGENCE_STUDY = True      # 是否运行收敛性分析
CONV_ORDERS = [1, 2]              # 收敛性分析的阶数
CONV_MESH_SIZES = [0.2, 0.1, 0.05] # 收敛性分析的网格尺寸

# ========== 可视化选项 ==========
PLOT_SOLUTION = True              # 绘制解
PLOT_COMPARISON = True            # 比较精确解和数值解
PLOT_PROFILES = True              # 绘制解的剖面图

