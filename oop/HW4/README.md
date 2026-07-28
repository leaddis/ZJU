# Personal Diary CLI Application

## 项目简介

Personal Diary 是一个基于命令行的日记管理工具，提供以下四个功能：

- `pdadd`：添加日记条目。支持覆盖指定日期的日记内容。
- `pdlist`：按日期列出所有日记条目，支持设定起止日期筛选。
- `pdshow`：显示指定日期的日记内容。
- `pdremove`：删除指定日期的日记条目。

所有数据存储在一个文件（默认名为 `diary.txt`）中。每个程序在执行前加载文件内容，执行后将内容写回文件。

## 文件说明

- **源代码**：
  - `Diary.cpp`：包含日记管理的主要类和函数实现。
  - `pdadd.cpp`、`pdlist.cpp`、`pdshow.cpp`、`pdremove.cpp`：分别对应四个命令的主程序。
- **脚本**：
  - `test_script.sh`：测试脚本，包含对所有功能的测试用例。
- **Makefile**：用于编译、测试和清理项目。

## 依赖要求

- C++17 编译器

## 编译和运行

### 使用 Makefile

1. **编译所有可执行文件**：

   ```bash
   make
   ```
    该命令会编译并生成四个可执行文件：pdadd、pdlist、pdshow、pdremove。
2. **运行测试**：
    使用以下命令运行 `test_script.sh` 脚本，并将测试结果输出到一个唯一编号的日志文件中（例如 `test_log_1.txt`, `test_log_2.txt` 等）：
    ```bash
   make testfile
   ```
   日志文件名将自动递增，防止覆盖之前的测试结果。运行后会显示日志文件的名称。
3. **清理文件**：
   使用以下命令删除所有生成的可执行文件以及 `diary.txt` 文件和所有测试日志：
    ```bash
   make clean
   ```