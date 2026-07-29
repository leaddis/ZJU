clear;clc;
load_plugin("spline");

% 测试 pp = csape(x,y),即为pp=csapi(x,y)
x = [1 1.5 2 4.1 5 7];
y = [1 -1 1 -1 1 8];
axis = 1:0.1:7
case1_test1_pp = csape(x,y)
plot(axis,fnval(case1_test1_pp,axis))

% 测试 pp=csape(x,[e1,y,e2],cond)
% 根据给定的插值点列和边界条件信息生成对应的三次样条函数
% 当cond为字符串时
case1_test2_pp=csape(x,[1,y,1],"complete")
case1_test3_pp=csape(x,[1,y,1],"periodic")
plot(axis,fnval(case1_test2_pp,axis))
plot(axis,fnval(case1_test3_pp,axis))

% 当cond为1*2阶的int矩阵时
% int值只能取0，1，2,表示"periodic"\"complete"\"second"
case2_test1_pp=csape(x,[1,y,1],[1,2]) %生成左边界导数值为1，右边界值二阶导为1的样条
plot(axis,fnval(case2_test1_pp,axis))

% 测试其他插值点
x2 = [1 2 2.5 3 4.5 6];
y2 = [2 1 4 -2 4 3];
axis2 = 1:0.1:6
case3_test1_pp = csape(x,y)
plot(axis,fnval(case1_test1_pp,axis))

case3_test2_pp = csape(x,[1,y,1],"complete")

case3_test3_pp=csape(x,[1,y,1],[1,2])