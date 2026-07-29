clear;clc;
load_plugin("spline");

% 测试 pp = csapi(x,y)
% 该函数根据给定的插值点列使用非扭结边界条件 (not-a-knot)创建一个 pp 格式的三次样条曲线，是 csape 函数的一种特殊情况。
% x y 分别为样条插值坐标点的横纵坐标构成的一维向量,长度必须相同, 且要求 x 已经完成单增排列
%  pp 为返回的样条结构体，为 pp 格式

% test1  正确测试
% 在点列 (1, 1),(1.5,-1),(2, 1),(4.1, -1),(5, 1),(7, 8) 上进行三次样条插值，边界条件采用 not-a-knot 条件
case1_test1_x = [1 1.5 2 4.1 5 7];
case1_test1_y = [1 -1 1 -1 1 8];
case1_test1_pp = csapi(case1_test1_x,case1_test1_y)
% 在完成样条函数的生成后，为了实现结果的可视化，可调用fnval 函数。fnval函数可以得到样条函数在指定点处的值。
% 画出上面生成的样条函数在插值区间的函数如下：
case1_test1_t = 0.5:0.001:8;
figure(1);
plot(case1_test1_t,fnval(case1_test1_pp,case1_test1_t),'k-',case1_test1_x,case1_test1_y,'ro');

% test2 错误测试
% 输入参数过少
%case1_test2_pp1 = csapi();
%case1_test2_pp2 = csapi([1 2 3 4]);

% test3 错误测试
% 输入参数过多 这里需注意当输入参数如3个的时候调用的是 values = csapi(x,y,xx), 具体可见下面的 case2 情形
%case1_test3_pp = csapi([1 2 3 5],[1 -1 1 -1],[9 8 2 3],[1 8 7 2])

% test4 错误测试
% 输入参数不匹配，x 的列数小于 y 的列数
%case1_test4_pp1 = csapi([1 1.5],[1 -1 1 -1 1]);
% 输入参数不匹配，x 的列数大于 y 的列数
%case1_test4_pp2 = csapi([1 1.5 3 6 9 10],[1 -1]);

% test5 错误测试
% x 未满足单增要求
%case1_test5_pp1 = csapi([1 2 3 5 8 7],[1 -1 1 -1 1 -1]);
% 对重节点情况应直接报错，待修改
%case1_test5_pp2 = csapi([1 2 3 5 8 8],[1 -1 1 -1 1 -1]);

% test6 错误测试
% 提供的插值节点过少
case1_test6_pp = csapi([1 2 3],[1 2 3]);

% test7 错误测试
% 一些非合理的参数输入

% 参数为标量
%case1_test7_pp1 = csapi(1,[1 2 3 4 5]);

% 参数为二维矩阵
% 能运行，应报错，带修改
%case1_test7_pp2 = csapi([1 2 3 4;5 6 7 8],[1 -1 1 -1]);
%case1_test7_pp3 = csapi([1 2 3 4],[1 -1 1 -1;1 -1 1 -1]);

% 参数为字符串
%case1_test7_pp4 = csapi('abc',[1 2 3 4 5]);
%case1_test7_pp5 = csapi('abcde',[1 2 3 4 5]);
%case1_test7_pp6 = csapi([1 2 3 4 5],'abcde');

% 测试 values = csapi(x,y,xx)
% 该函数返回生成样条函数在 xx 处的值
% x y 分别为样条插值坐标点的横纵坐标构成的一维向量,长度必须相同, 且要求 x 已经完成单增排列
% xx 为待求值点横坐标构成的一维向量
% value 为一维向量,返回待求值点的函数值

% test1  正确测试
% 仍在点列 (1, 1),(1.5,-1),(2, 1),(4.1, -1),(5, 1),(7, 8) 上进行三次样条插值，边界条件采用 not-a-knot 条件
% 这次不调用fnval 函数，而是使用 values = csapi(x,y,xx) 直接求得所需要的值。
% 画出样条函数在插值区间的函数如下：
case2_test1_x = [1 1.5 2 4.1 5 7];
case2_test1_y = [1 -1 1 -1 1 8];
case2_test1_t = 0.5:0.001:8;
case2_test1_values = csapi(case2_test1_x,case2_test1_y,case2_test1_t);
figure(2);
plot(case2_test1_t,case2_test1_values,'k-',case2_test1_x,case2_test1_y,'ro');
% 对比 case1 和 case2 中的两张图可以发现结果相同.

% test2 错误测试
% 输入参数过少 注意2个参数时调用的是第一种功能
%case2_test2_value1 = csapi();
%case2_test2_value2 = csapi([1 2 3 4]);

% test3 错误测试
% 输入参数过多 
%case2_test3_value = csapi([1 2 3 5],[1 -1 1 -1],[9 8 2 3],[1 8 7 2])

% test4 错误测试
% 输入参数不匹配，x 的列数小于 y 的列数
%case2_test4_value1 = csapi([1 1.5],[1 -1 1 -1 1],linspace(1,1.5,100));
% 输入参数不匹配，x 的列数大于 y 的列数
%case2_test4_value2 = csapi([1 1.5 3 6 9 10],[1 -1],linspace(1,10,1000));

% test5 错误测试
% x 未满足单增要求
% 此例应报错或者调整csapi的函数实现，使其能够对输入参数重新排序 
%case2_test5_value1 = csapi([1 2 3 5 8 7],[1 -1 1 -1 1 -1],linspace(0,8,1000));
%figure(3);
%plot(linspace(0,8,1000),case2_test5_value1,[1 2 3 5 8 7],[1 -1 1 -1 1 -1],'ro');
% 对重节点情况应直接报错，待修改
%case2_test5_value2 = csapi([1 2 3 5 8 8],[1 -1 1 -1 1 -1],linspace(0,9,1000));

% test6 错误测试
% 提供的插值节点过少
% 少于4个节点应报错，待修改 
%case2_test6_value = csapi([1 2 3],[-1 2 -3],linspace(1,3,100));
%figure(4);
%plot(linspace(1,3,100),case2_test6_value,[1 2 3],[-1 2 -3],'ro');

% test7 错误测试
% 一些非合理的参数输入

% 参数为标量
%case2_test7_value1 = csapi(1,[1 2 3 4 5],linspace(0,2,100));

% 参数为二维矩阵
% 能运行，应报错，待修改
%case2_test7_value2 = csapi([1 2 3 4;9 6 7 8],[1 -1 1 -1],linspace(0,9,1000));
%case2_test7_value3 = csapi([1 2 3 4],[1 -1 1 -1;1 -1 1 -1],linspace(0,5,1000));