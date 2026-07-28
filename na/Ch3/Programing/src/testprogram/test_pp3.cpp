// spline_test.cpp

#include "../Spline.h"
#include <iostream>
#include <fstream>

int main()
{
    // 定义插值数据点
    std::vector<double> x = {0.0, 2.0, 2.5, 3.0, 6.0};
    std::vector<double> y = {7.0, 2.0, 4.0, 0.0, 7.0};

    // 设置插值条件
    InterpCondition cond;
    cond.sites = x;
    cond.function_values = y;
    cond.left = BCType::natural;   // 左边界条件：自然边界条件
    cond.right = BCType::natural;  // 右边界条件：自然边界条件

    
    // cond.left = BCType::complete;  // 左边界条件：二阶导数指定
    // cond.right = BCType::complete; // 右边界条件：二阶导数指定
    // cond.derivative1 = 2.0;        // 左边界一阶导数
    // cond.derivative2 = 2.0;        // 右边界一阶导数

    // cond.left = BCType::specified_2nd;  // 左边界条件：二阶导数指定
    // cond.right = BCType::specified_2nd; // 右边界条件：二阶导数指定
    // cond.derivative1 = 2.0;             // 左边界二阶导数
    // cond.derivative2 = 2.0;             // 右边界二阶导数

    // cond.left = BCType::not_a_knot;  // 左边界条件：非结点边界条件
    // cond.right = BCType::not_a_knot; // 右边界条件：非结点边界条件

    // cond.left = BCType::periodic;  // 左边界条件：周期边界条件
    // cond.right = BCType::periodic; // 右边界条件：周期边界条件



    // 构造三次样条
    Spline<3, SplineType::ppForm> spline(cond);

    //输出输入条件
    std::ofstream outfile1("spline_data_pp3_input.txt");
    if (!outfile1)
    {
        std::cerr << "无法打开输出文件。" << std::endl;
        return 1;
    }
    outfile1 << "输入条件：" << std::endl;
    for (size_t i = 0; i < x.size(); i++)
    {
        outfile1 << x[i] << " ";
    }
    outfile1 << std::endl;
    for (size_t i = 0; i < y.size(); i++)
    {
        outfile1 << y[i] << " ";
    }
    outfile1.close();


    // 打印所有分片多项式
    spline.printPolys();
    // 打开文件用于写入样条插值结果
    std::ofstream outfile("spline_data_pp3.txt");
    if (!outfile)
    {
        std::cerr << "无法打开输出文件。" << std::endl;
        return 1;
    }

    // 生成插值点并计算样条值
    double xmin = x.front();
    double xmax = x.back();
    int num_points = 500;  // 插值点数量，数值越大曲线越平滑
    double step = (xmax - xmin) / (num_points - 1);

    for (int i = 0; i < num_points; ++i)
    {
        double xi = xmin + i * step;
        double yi = spline(xi);
        outfile << xi << " " << yi << std::endl;
    }

    outfile.close();

    std::cout << "样条插值结果已写入 spline_data_pp3.txt 文件。" << std::endl;

    return 0;
}
