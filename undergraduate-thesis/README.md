# 二维椭圆方程有限元求解器

### 项目简介

本项目提供了一个完整的框架，用于求解二维椭圆型偏微分方程：

```
    -∇·(a(x,y)∇u) = f(x,y)  在区域 Ω = [0,1]² 内
    u = 0                     在边界 ∂Ω 上
```

### 使用说明
##### 安装依赖
```
pip install ngsolve netgen matplotlib numpy scipy
```
##### 程序运行
``` bash
python run_simulation.py
```

### 文件结构
```
├── code/                    # 源代码目录
│   ├── run_simulation.py    # 主求解脚本
│   ├── user_config.py       # 用户配置文件
│   ├── main.py              # 核心求解器模块
│   ├── elliptic_solver.py   # 有限元求解器类
│   ├── visualize_solution.py # 可视化模块
│   └── plots/               # 生成的图像目录
└── paper/                   # 论文相关文件
```