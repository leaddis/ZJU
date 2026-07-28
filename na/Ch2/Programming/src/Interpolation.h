#ifndef ITTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

class Interpolation
{
    protected:
        vector<double> x_values;
        vector<double> y_values;

    public:
        virtual void addPoint(double x, double y);
        virtual void removePoint(int index);
        virtual void printPoints(ofstream& file) const;  // 在file里输出数据点
        virtual void printPoints(ostream& os) const;  // 输出数据点
        virtual double interpolate(double x) = 0;
        virtual void isInrange(double x) const; //检查x是否在插值范围内
        virtual ~Interpolation() = default;
        virtual void displayPolynomialPoint(ofstream& file,int n) = 0;//输出n个插值结果的坐标
};

class Newton_Interpolation : public Interpolation
{
    private:
        std::vector<std::vector<double>> coeff; //二维差商表
        void calculate_diff_table();//计算插商表

    public:
        Newton_Interpolation() = default;
        double interpolate(double x) override;//计算插值点的值
        void displayPolynomial(ofstream& file);//输出Newton多项式的系数，从常数项到高阶项
        void displayPolynomialPoint(ofstream& file,int n) override;//输出n个插值结果的坐标
};

class Hermite_Interpolation : public Interpolation
{
    private:
        std::vector<std::vector<double>> coeff; //二维差商表
        void calculate_diff_table();//计算插商表
        vector<double> y_prime_values;  // 存储 y' 值（导数值）
        vector<double> x_cal; //把x的每个值重复两次，用于计算差商表

    public:
        Hermite_Interpolation() = default;
        void addPoint(double x, double y, double y_prime);  // 新的添加点函数
        void removePoint(int index);  // 新的删除点函数
        void printPoints(ostream& os) const override;  // 输出输入数据点函数
        void printPoints(ofstream& file) const override;  // 输出输入数据点函数
        double interpolate(double x) override; //计算插值点的值
        void displayPolynomial(ofstream& file);//输出Hermite多项式的系数，从常数项到高阶项
        void displayPolynomialPoint(ofstream& file,int n) override;//输出n个插值结果的坐标
};
#endif // INTERPOLATION_H 