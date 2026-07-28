#!/bin/bash

# 清除旧的日记文件
rm -f diary.txt

echo "测试 pdadd"
echo -e "今天完成了很多工作\n." | ./pdadd 2023-10-29
echo -e "测试第二天的日记内容\n." | ./pdadd 2023-10-30

echo "测试 pdlist"
./pdlist
./pdlist 2023-10-29 2023-10-30

echo "测试 pdshow"
./pdshow 2023-10-29
./pdshow 2023-10-30

echo "测试 pdremove"
./pdremove 2023-10-29
./pdlist  # 检查删除是否生效

#echo "测试删除不存在的条目"
#./pdremove 2023-10-31

