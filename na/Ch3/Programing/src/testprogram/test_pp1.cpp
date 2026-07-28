// spline_test.cpp

#include "../Spline.h"
#include <iostream>
#include <fstream>

int main()
{
    // 定义插值数据点
    std::vector<double> x = {0.0, 1.0, 2.0, 3.0, 6.0};
    std::vector<double> y = {0.0, 1.0, 4.0, 0.0, 7.0};

    // 设置插值条件
    InterpCondition cond;
    cond.sites = x;
    cond.function_values = y;

    // 构造三次样条
    Spline<1, SplineType::ppForm> spline(cond);

    // 打开文件用于写入样条插值结果
    std::ofstream outfile("spline_data_pp1.txt");
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

    std::cout << "样条插值结果已写入 spline_data_pp1.txt 文件。" << std::endl;

    return 0;
}
