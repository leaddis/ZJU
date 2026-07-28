clear;clc;
load_plugin("spline");

% 测试 pp = ppmak(breaks,coefs)
% 该函数根据用户给出的信息创建若干 pp 格式的分段多项式(多维)
% breaks 为节点横坐标构成的一维向量,要求其已完成单增排列
% coefs 为二维矩阵,由分片多项式的系数构成
%  pp 为一结构体

% test1  正确测试
case1_test1_pp1 = ppmak([1 3 4],[1 2 5 6;3 4 7 8])
%fnval(case1_test1_pp,2);
%此处调用 fnval 会报错，可能是因为该分段多项式是2维的，fnval 暂不支持
% 使用 ppmak 构造在[0,2]区间构造一分段多项式，多项式片数为2,第一段为x^2 + 2x + 3,第二段为4(x – 1)^2 + 5(x–1) + 6
% 可以验证该分段多项式恰好是连续的，对于1维情形，可调用fnval画出其图像
case1_test1_pp2 = ppmak([0:2],[1:6]);
figure(1);
plot(linspace(-1,3,1000),fnval(case1_test1_pp2,linspace(-1,3,1000)));

% test2  错误测试
% 输入参数过少
%case1_test2_pp1 = ppmak();
%case1_test2_pp2 = ppmak([1 5 7 8]);

% test3 错误测试
% 输入参数过多 3个参数时调用的是 case3 的情形
%case1_test3_pp = ppmak([1 3 4],[1 2 5 6],[3 4 7 8],4)

% test4 错误测试
%参数错误 节点数目与系数矩阵列数不匹配
% 节点必须满足 length(breaks)-1 能够整除 coefs 矩阵的列数
%case1_test4_pp = ppmak([1 3 4],[1 2 3;3 4 7]);

% test5 错误测试
% 节点不满足单增要求
%case1_test5_pp = ppmak([1 4 3],[1 2 5 6;3 4 7 8]);

% test6 错误测试
% 一些非合理的参数输入

% 参数为标量
%case1_test6_pp1 = ppmak(1,[1 2 5 6]);
%case1_test6_pp2 = ppmak([1 3 4],1);

% 第一个参数为二维矩阵
% 能运行，但应直接报错，待修改
%case1_test6_pp3 = ppmak([[1 3 4;6 7 8]],[1 2 5 6]);

% 参数为字符串或char
% 能运行，应报错
%case1_test6_pp4 = ppmak('abc/',[1 2 3 4 5 6]);
%case1_test6_pp5 = ppmak([1 3 4],['a' 'b' 'c' 'd']);

% 测试 [pp1,pp2,...,ppm] = ppmak(breaks,coefs)
%该函数与上面的函数类似,但其返回值为多个结构体 (pp 格式),每个结构体的维数都是 1 维
%即将第一种用法中的输出结果拆分为多个 1 维的情形
%输出参数的个数必须等于 coefs 矩阵的行数

% test1 正确测试
[case2_test1_pp1,case2_test1_pp2] = ppmak([1 3 4],[1 2 5 6;3 4 7 8]);

% test2 错误测试
% 输入参数过少
%case2_test2_pp1 = ppmak();
%case2_test2_pp2 = ppmak([1 5 7 8]);

% test3 错误测试
% 输入参数过多 3个参数时调用的是 case3 的情形
%case2_test3_pp = ppmak([1 3 4],[1 2 5 6],[3 4 7 8],4)

% test4 错误测试
% 输出参数过多或过少
%[case2_test4_pp11,case2_test4_pp12] = ppmak([1 3 4],[1 2 5 6;3 4 7 8;-2 -9 0 3]);
%[case2_test4_pp21,case2_test4_pp22,case2_test4_pp23,case2_test4_pp24] = ppmak([1 3 4],[1 2 5 6;3 4 7 8;-2 -9 0 3]);

% test5 错误测试
%参数错误 节点数目与系数矩阵列数不匹配
% 节点必须满足 length(breaks)-1 能够整除 coefs 矩阵的列数
%[case2_test5_pp1,case2_test5_pp2] = ppmak([1 3 4],[1 2 3;3 4 7]);

% test6 错误测试
% 节点不满足单增要求
% 仍能运行，此处应报错，待修改
%[case2_test6_pp1,case2_test6_pp2] = ppmak([1 4 3],[1 2 5 6;3 4 7 8]);

% test7 错误测试
% 一些非合理的参数输入

% 参数为标量
%[case2_test7_pp1,case2_test7_pp2] = ppmak(1,[1 2 5 6;3 4 7 8]);

% 第一个参数为二维矩阵
% 能运行，但应直接报错，待修改
%[case2_test7_pp3,case2_test7_pp4] = ppmak([[1 3 4;6 7 8]],[1 2 5 6;2 4 7 8]);

% 参数为字符串或char
% 能运行，应报错
%[case2_test7_pp5,case2_test7_pp6] = ppmak('abc/',[1 2 3 4 5 6;7 8 9 10 11 12]);
%[case2_test7_pp7,case2_test7_pp8] = ppmak([1 3 4],['a' 'b' 'c' 'd';'e' 'f' 'g' 'h']);

% 测试 pp = ppmak(breaks,coefs,d)
%该函数与 case1 中用法类似,返回值为一个结构体 (pp 格式),但其输入的 coefs 矩阵为内部存储的形式
% breaks 为节点横坐标构成的一维向量,要求其已完成单增排列
%coefs 为二维矩阵,由分片多项式的系数构成
%d 为一标量,表示分段多项式的维数
%pp 为一结构体

% test1  正确测试
case3_test1_pp = ppmak([1 3 4],[1 2;3 4;5 6;7 8],2);
% 使用 ppmak 构造在[0,2]区间构造一分段多项式，多项式片数为2,第一段为x^2 + 2x + 3,第二段为4(x – 1)^2 + 5(x–1) + 6
% 可以验证该分段多项式恰好是连续的，对于1维情形，可调用fnval画出其图像
case3_test1_pp2 = ppmak([0:2],[1 2 3;4 5 6],1);
figure(2);
plot(linspace(-1,3,1000),fnval(case3_test1_pp2,linspace(-1,3,1000)));
% 得到与 case1 中相同的图像

% test2  错误测试
% 输入参数过少
%case3_test2_pp1 = ppmak();
%case3_test2_pp2 = ppmak([1 5 7 8]);

% test3 错误测试
% 输入参数过多 
%case3_test3_pp = ppmak([1 3 4],[1 2;3 4;5 6;7 8],[3 4 7 8],4)

% test4 错误测试
% 参数错误 节点数目与维数d不匹配，此处第三个参数应为1
%case3_test4_pp = ppmak([1 3 4],[1 2 3;3 4 7],2);

% test5 错误测试
% 节点不满足单增要求
% 仍能运行，此处应报错，待修改
%case3_test5_pp = ppmak([1 4 3],[1 2;3 4;5 6;7 8],2);

% test6 错误测试
% 一些非合理的参数输入

% 参数为标量
%case3_test6_pp1 = ppmak(1,[1 2;3 4;5 6;7 8],2);
%case3_test6_pp2 = ppmak([1 3 4],2,2);

% 第一个参数为二维矩阵
% 能运行，但应直接报错，待修改
%case3_test6_pp3 = ppmak([[1 3 4;6 7 8]],[1 2;3 4;5 6;7 8],2);

% 参数为字符串或char
% 能运行，应报错
%case3_test6_pp4 = ppmak('abc',[1 2;3 4;5 6;7 8]);
%case3_test6_pp5 = ppmak([1 3 4],['a' 'b';'c' 'd';'e' 'f';'g' 'h'],2);
