clear;clc;
load_plugin("spline");

% 测试 ns = fn2fm(s,form)
% 将样条函数s 转换为form 格式，返回转换后的样条函数ns.
% form 由字符向量或字符串标量形式指定，形式的选择分别为'B-'、'pp'，分别为 B 格式、pp 格式。
% B-form 将函数描述为给定结序列的给定阶k 的B 样条的加权和，pp 格式用其局部多项式系数来描述一个函数。

% test1   B 转 pp 正确测试
knots = [0, 0, 0, 0, 1, 2, 2, 2, 2];
x = [0, 1, 1, 1, 2];
y = [2, 0, 1, 2, -1];
s1 = spapi(knots, x, y)
s2 = fn2fm(s1,'pp')

% test2   B 转 B 测试（保持不变）
s3 = fn2fm(s1,'B-')

% test3   pp 转 pp 测试（保持不变）
x = [0 1 2 3];
y = [1 2 4 6];
s4 = csapi(x,y)
s5 = fn2fm(s4,'pp')

% test4   pp 转 B 测试
s6 = fn2fm(s4,'B-')

% test5   非法样条参数
% s7 = fn2fm('x','B-')

% test6   参数过少
% s8 = fn2fm(s1)

% test7   参数过多
% s9 = fn2fm(s1,'pp',6)

% test8   非法 form 参数
% s10 = fn2fm(s1,'233')


