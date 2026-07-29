clear;clc;
load_plugin("spline");

% 本脚本用于测试 fnder 函数。
% fnder 计算一个样条函数的导函数，并仍以样条函数的形式返回。

% Test 1. 测试 ds = fnder(s, order)
% 得到样条函数 s 的 order 阶导函数，s 可以是 pp 格式、B 格式。 
% order 是一个不超过样条函数次数的正整数。
% 该函数输出的形式与输入相同，仍以样条函数的格式返回，结果要么是 pp 格式，要么是 B 格式。

% Test 1.1-1.7 使用 csapi 生成的样条进行测试
s1 = csapi([0 1 2 3], [1 2 4 6])

% Test 1.1 正确输入
% 理论结果:
%       form: 'pp'
%     breaks: [0 1 2 3]
%      coefs: [3×2 double]
%     pieces: 3
%      order: 2
%        dim: 1
% 实际结果:
ds1 = fnder(s1,2)

% Test 1.2 order越界
% 理论结果: 报错
% 实际结果:
ds2 = fnder(s1,10)

% Test 1.3 输入参数过多
% 理论结果: 报错
% 实际结果:
% ds3 = fnder(s1,1,3)

% Test 1.4 order为字符串
% 理论结果: 报错
% 实际结果:
% ds4 = fnder(s1,"01")

% Test 1.5 order为矩阵
% 理论结果: 报错
% 实际结果:
% ds5 = fnder(s1,[1,2;2,3])

% Test 1.6 错误s，矩阵
% 理论结果: 报错
% 实际结果:
% ds6 = fnder([1,2;2,3],2)

% Test 1.7 错误s，字符串
% 理论结果: 报错
% 实际结果:
% ds7 = fnder("x",2)

% Test 1.8-1.9 测试 csape 生成的样条
s2 = csape([0 1 2 3], [1 2 4 6])

% Test 1.8 正确输入
% 理论结果:
%       form: 'pp'
%     breaks: [0 1 2 3]
%      coefs: [3×3 double]
%     pieces: 3
%      order: 3
%        dim: 1
% 实际结果:
ds8 = fnder(s2,1)

% Test 1.9 错误输入
% 理论结果: 报错
% 实际结果:
ds9 = fnder(s2,-1)

% Test 1.10-1.11 测试 ppmak 生成的样条
s3 = ppmak([1 5 7 8],[1 2 5 6 8 -3 24 -1 -3 ;3 4 7 8 9 12 73 -13 21;1 10 4 -9 -2 23 87 -99 100]);

% Test 1.10 正确输入
% 理论结果:
%       form: 'pp'
%     breaks: [1 5 7 8]
%      coefs: [9×2 double]
%     pieces: 3
%      order: 2
%        dim: 3
% 实际结果:
ds10 = fnder(s3,1)

% Test 1.11 错误输入
% 理论结果: 报错
% 实际结果:
% ds11 = fnder(s3,10)


% Test 2. 测试 ds = fnder(s)
% 等同 ds = fnder(s,1), 因此此处只需测试正确输入与ds8,ds10比较即可。

% Test 2.1 测试 csape 生成的样条的正确输入
% 理论结果:
%       form: 'pp'
%     breaks: [0 1 2 3]
%      coefs: [3×3 double]
%     pieces: 3
%      order: 3
%        dim: 1
% 实际结果:
ds12 = fnder(s2)

% Test 2.1 测试 ppmak 生成的样条的正确输入
% 理论结果:
%       form: 'pp'
%     breaks: [1 5 7 8]
%      coefs: [9×2 double]
%     pieces: 3
%      order: 2
%        dim: 3
% 实际结果:
ds13 = fnder(s3)