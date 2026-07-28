clear;clc;
load_plugin("spline");

% 本脚本用于测试 spmak 函数。
% 根据用户给出的信息创建一个 B 格式的样条曲线。

% sp = spmak(knots,coefs)
% 该函数返回值为一个结构体 (B 格式)。
%  knots 为 B 样条节点, 这里要求其为单增非降的一维向量。
% coefs 为 B 样条的系数, 要求其为一维向量,且长度必须小于 knots 的长度。
% 样条曲线的阶数 k 满足 k = length(knots) - length(coefs)。

% Test 1.1-1.7 对 sp = spmak(knots,coefs) 进行测试

% Test 1.1 正确输入
%给定 B 样条节点 {0, 0, 1, 2, 3, 4, 5, 5}, 可以得到三个 4 次的 B 样条。
%若这三个 B 样条的系数分别为 1,4 和-2,则我们可以得到对应的 B 格式样条曲线。
% 理论结果:
%       form: 'B-'
%     knots: [0 0 1 2 3 4 5 5]
%      coefs: [1 4 -2]
%     number: 3
%      order: 5
%        dim: 1
% 实际结果:
knots1 = [0 0 1 2 3 4 5 5];
coefs1 = [1 4 -2];
sp1 = spmak(knots1,coefs1)

% Test 1.2 第一个参数为矩阵不为向量
% 理论结果: 报错
% 实际结果:
%knots2= [0 0 1 2 3 4 5 5; 1 1 2 3 4 5 6 6];
%coefs2 = [1 4 -2];
%sp2 = spmak(knots2,coefs2);

% Test 1.3 第二个参数为矩阵不为向量
% 理论结果: 报错
% 实际结果:
%knots3 = [0 0 1 2 3 4 5 5];
%coefs3 = [1 4 -2; 2 5 -1];
%sp3 = spmak(knots3,coefs3);

% Test 1.4 节点个数小于系数个数
% 理论结果: 报错
% 实际结果:
%knots4 = [0 1];
%coefs4 = [1 4 -2];
%sp4 = spmak(knots4,coefs4);

% Test 1.5 节点没按单增非减顺序排列
% 理论结果: 报错
% 实际结果:
%knots5 = [0 0 1 2 3 4 5 2];
%coefs5 = [1 4 -2];
%sp5 = spmak(knots5,coefs5);

% Test 1.6 错误参数 字符串
% 理论结果: 报错
% 实际结果:
%knots6 = [0 0 1 2 3 4 5 5];
%coefs6 = "abc";
%sp6 = spmak(knots6,coefs6);

% Test 1.7 输入参数过多
% 理论结果: 报错
% 实际结果:
%knots7 = [0 0 1 2 3 4 5 5];
%coefs7 = [1 4 -2];
%sp7 = spmak(knots7,coefs7,[1 2 3]);