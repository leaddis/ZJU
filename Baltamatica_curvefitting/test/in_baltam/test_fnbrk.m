clear;clc;
load_plugin("spline");

%测试 [out1,...,outn] = fnbrk(f,part1,...,partm)
%parti 表示第i个输入的样条信息，具体可见函数说明文档
%f 表示输入的样条函数； outi 输出第i个输入的样条信息
%case1 pp格式

%test1 正确测试
x = [  0,2,3, 5,6  ];
y = [1,0,3,1,-1,0,0];
f = csape(x,y,"complete");
[case1_test1_form,case1_test1_variables,case1_test1_dimension,case1_test1_coefs,case1_test1_interval,case1_test1_order,case1_test1_breaks,case1_test1_pieces]...
=fnbrk(f,"form","variables","dimension","coefficients","interval","order","breaks","pieces");

%test2 
%测试n<m时的情况

[case1_test2_form,case1_test2_variables]...
=fnbrk(f,"form","variables","dimension","coefficients","interval","order");

%测试截取样条功能

[s]=fnbrk(f,[4,5]);

%test3 错误测试
%测试n>m时的情况
%[case1_test3_form,case1_test3_variables]=fnbrk(f,"form");

%test4 错误测试
%一些非合理的参数输入

%f不是样条结构体,或没有f
%[case1_test4_variables]=fnbrk(1,"variables");
%[case1_test4_variables]=fnbrk("abc","variables");
%[case1_test4_variables]=fnbrk([1,2;3,4],"variables");
%[case1_test4_variables]=fnbrk("variables");

%test5
%parti参数输入不合理
%在matlab中不会报错，可能只看首字母？
%[case1_test5_variables]=fnbrk(f,"variable");

%在matlab中不会报错，可能是新用法，目前未实现
%[case1_test5_variables]=fnbrk(f,[1,2]);

%case2 测试B样条

knots = [0,0,0,0,1,2,2,2,2];
    x = [0,1,1,1,2];
    y = [2,0,1,2,-1];
    f = spapi(knots,x,y);
    [form,variables,dimension,coefs,interval,order,knots,number] = ...
    fnbrk(f,'form','variables','dimension','coefficients','interval','order','knots','number');
    
    
    






