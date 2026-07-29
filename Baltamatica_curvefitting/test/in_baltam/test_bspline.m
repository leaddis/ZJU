clear;clc;
load_plugin("spline");

%测试 p=bspline(t)

%test1 正确测试
case1_test1_p1=bspline([1,2,3,4,5]);

case1_test1_p2=bspline([1,2.5,3.6,4.9,5.8]);

%重节点情况
case1_test3_p2=bspline([1,4,4,4,5,6]);

%test2 错误测试
%输入参数数量不匹配
%matlab输入参数过多只取第一个参数
%case1_test2_p1=bspline([1,2,3,4],[2,3,4]);
%case1_test2_p2=bspline();

%test3 错误测试
%一些非合理的参数输入

%x 未满足单增要求
%与matlab不同，matlab的机制未知
%case1_test3_p1=bspline([4,2,1,7]);


%提供的插值节点过少（1个）
%这个直接闪退，应该需要修改
%case1_test3_p3=bspline([1]);

%参数为二维矩阵
%case1_test3_p4=bspline([1,2,3,4;2,3,4,5]);

%参数为字符串
%与matlab输出结果不同
%case1_test3_p5=bspline('abc');

