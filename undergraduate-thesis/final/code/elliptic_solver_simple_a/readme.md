
### 基本命令
```bash
# 运行主求解程序
make

# 或者明确指定
make run_simulation

# 快速启动
make quick

# 清理所有生成的文件
make clean

# 显示帮助
make help

# 显示当前状态
make status
```
### 命令说明

make 或 make run_simulation - 运行完整求解流程，包括可视化和收敛性分析

make clean - 清理所有生成的文件，包括：

    __pycache__ 目录（Python 缓存）

    所有 .pyc 文件

    plots/ 目录中的所有图像文件

    其他临时文件

make help - 显示所有可用命令的帮助信息

make status - 显示当前目录状态
`
