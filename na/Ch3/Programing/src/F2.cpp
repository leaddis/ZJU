#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "Base_Bspline.h"
using namespace std;

// 截断函数 (t - xi)_+
double truncated_linear(double t, double xi) {
    return t > xi ? (t - xi) : 0.0;
}

// 一阶分差
// [t_i, t_j]f = (f(t_j)-f(t_i)) / (t_j - t_i)
auto first_diff(double ti, double tj, double (*f)(double, double), double xi) {
    double val = (f(tj, xi) - f(ti, xi)) / (tj - ti);
    return [=](double t){ return val; }; 
    // 返回一个常数函数lambda（在[t_i, t_j]上是常数斜率），
    // 实际上对分差的直观理解，这个结果是一个常量，用于生成一条斜线段。
}

void output_spline_data(const Base_Bspline &spline, const std::string &filename, double x_start, double x_end, double step) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    file << "x,value,derivative\n"; // Header
    for (double x = x_start; x <= x_end; x += step) {
        double value = spline(x);
        double derivative = spline.d(1,x);
        file << x << "," << value << "," << derivative << "\n";
    }
    
    file.close();
    std::cout << "Data written to " << filename << std::endl;
}


// 二阶分差
// [t_i, t_j, t_k]f = ([t_j,t_k]f - [t_i, t_j]f) / (t_k - t_i)
auto second_diff(double ti, double tj, double tk, double (*f)(double,double), double xi) {
    double f_ij = (f(tj, xi) - f(ti, xi)) / (tj - ti);
    double f_jk = (f(tk, xi) - f(tj, xi)) / (tk - tj);
    double val = (f_jk - f_ij) / (tk - ti);
    return [=](double t){ return val*(t - ti); };
    // 二阶分差对于线性情况最终会给出一个分片线性函数的斜线。
    // 根据需要可调整为更精确的分段定义，这里只是示意。
}

// 三阶分差
// [t_i, t_j, t_k, t_l]f = ([t_j,t_k,t_l]f - [t_i, t_j, t_k]f) / (t_l - t_i)
auto third_diff(double ti, double tj, double tk, double tl, double (*f)(double,double), double xi) {
    double f_ijk = (f(tj, xi) - f(ti, xi)) / (tj - ti);
    double f_jkl = (f(tk, xi) - f(tj, xi)) / (tk - tj);
    double f_klm = (f(tl, xi) - f(tk, xi)) / (tl - tk);
    double val = (f_klm - f_jkl) / (tl - ti);
    return [=](double t){ return val*(t - ti); };
    // 三阶分差对于线性情况最终会给出一个分片线性函数的斜线。
    // 根据需要可调整为更精确的分段定义，这里只是示意。
}

// 输出函数数据用于绘图
template<typename Func>
void output_data(Func func, double start, double end, double step, const string &filename) {
    ofstream fout(filename);
    fout << "x,y\n";
    for (double x = start; x <= end; x+=step) {
        fout << x << "," << func(x) << "\n";
    }
    fout.close();
}

int main() {
    // 节点选择
    double t_im1 = 0.0; 
    double t_i   = 1.0;
    double t_ip1 = 2.0;
    double t_ip2 = 3.0;
    // 基础截断函数 f(t) = (t - t_i)_+ 对于 i=1 则 xi=1.0
    auto f0 = [&](double t){return truncated_linear(t, t_im1);};

    // 图1：原始截断函数 (t - 1)_+
    output_data(f0, -0.5, 3.5, 0.01, "fig1_0_original.csv");

    // 基础截断函数 f(t) = (t - t_i)_+ 对于 i=1 则 xi=1.0
    auto f1 = [&](double t){return truncated_linear(t, t_i);};

    // 图1：原始截断函数 (t - 1)_+
    output_data(f1, -0.5, 3.5, 0.01, "fig1_1_original.csv");

    // 基础截断函数 f(t) = (t - t_i)_+ 对于 i=1 则 xi=1.0
    auto f2 = [&](double t){return truncated_linear(t, t_ip1);};

    // 图1：原始截断函数 (t - 1)_+
    output_data(f2, -0.5, 3.5, 0.01, "fig1_2_original.csv");

    // 基础截断函数 f(t) = (t - t_i)_+ 对于 i=1 则 xi=1.0
    auto f3 = [&](double t){return truncated_linear(t, t_ip2);};

    // 图1：原始截断函数 (t - 1)_+
    output_data(f3, -0.5, 3.5, 0.01, "fig1_3_original.csv");

    // 图2：在 [t_i, t_{i+1}] 上的一阶分差
    // 分差后结果是一个常量(斜率)，要还原出图中的线段，需要手动构造线段
    double f_iip1 = (f1(t_ip1)-f1(t_i)) / (t_ip1 - t_i); // 计算分差值
    // 这个分差对应一条直线，从x=1到x=2，值为f_iip1*(t - t_i)。
    auto line_iip1 = [=](double t){
        if (t < t_i ) return 0.0;
        if (t > t_ip1) return 1.0;
        return f_iip1*(t - t_i);
    };
    output_data(line_iip1, -0.5, 3.5, 0.01, "fig2_2.csv");

    // 图3：在 [t_{i-1}, t_i] 上的一阶分差
    double f_im1i = (f0(t_i)-f0(t_im1)) / (t_i - t_im1);
    auto line_im1i = [=](double t){
        if (t < t_im1 ) return 0.0;
        if (t > t_i) return 1.0;
        return f_im1i*(t - t_im1);
    };
    output_data(line_im1i, -0.5, 3.5, 0.01, "fig2_1.csv");

    // 图3：在 [t_{i-1}, t_i] 上的一阶分差
    double f_im11i = (f2(t_ip2)-f2(t_ip1)) / (t_ip2 - t_ip1);
    auto line_im11i = [=](double t){
        if (t < t_ip1 ) return 0.0;
        if (t > t_ip2) return 1.0;
        return f_im11i*(t - t_im1)-2;
    };
    output_data(line_im11i, -0.5, 3.5, 0.01, "fig2_3.csv");

    // 图4：将图2与图3的结果相减（对应二阶分差）
    // 二阶分差 = ([t_i,t_{i+1}]f - [t_{i-1},t_i]f)/(t_{i+1}-t_{i-1})
    double sec_diff = (f_iip1 - f_im1i)/(t_ip1 - t_im1);
    // 二阶分差将给出在中间构造B样条的关键图形(中间那个小山)
    // B样条线性情况是分段构造的：
    auto b_spline_n1 = [=](double t) {
        // B_i^1(x) 在[0,1]上为(x - 0)/(1 - 0)=x,在[1,2]上为(2 - x)/(2 - 1)=2 - x,其他为0
        if (t >= t_im1 && t <= t_i) {
            return (t - t_im1) / (t_i - t_im1);
        } else if (t > t_i && t <= t_ip1) {
            return (t_ip1 - t) / (t_ip1 - t_i);
        }
        return 0.0;
    };
    output_data(b_spline_n1, -0.5, 3.5, 0.01, "fig3_1.csv");

    // 图4：将图2与图3的结果相减（对应二阶分差）
    // 二阶分差 = ([t_i,t_{i+1}]f - [t_{i-1},t_i]f)/(t_{i+1}-t_{i-1})
    double sec_diff2 = (f_iip1 - f_im1i)/(t_ip1 - t_im1);
    // 二阶分差将给出在中间构造B样条的关键图形(中间那个小山)
    // B样条线性情况是分段构造的：
    auto b_spline_n2 = [=](double t) {
        // B_i^1(x) 在[0,1]上为(x - 0)/(1 - 0)=x,在[1,2]上为(2 - x)/(2 - 1)=2 - x,其他为0
        if (t >= t_i && t <= t_ip1) {
            return (t - t_i) / (t_ip1 - t_i);
        } else if (t > t_ip1 && t <= t_ip2) {
            return (t_ip2 - t) / (t_ip2 - t_i);
        }
        return 0.0;
    };
    Base_Bspline bspline;
    std::vector<double> knots = {1, 2, 3};
    Base_Bspline spline(knots);
    output_spline_data(spline, "fig3_2.csv", 0, 3.5, 0.01);

     // 图4：将图2与图3的结果相减（对应二阶分差）
    // 二阶分差 = ([t_i,t_{i+1}]f - [t_{i-1},t_i]f)/(t_{i+1}-t_{i-1})
    double sec_diff3 = (f_iip1 - f_im1i)/(t_ip1 - t_im1);
    // 二阶分差将给出在中间构造B样条的关键图形(中间那个小山)
    // B样条线性情况是分段构造的：
    auto b_spline_n3 = [=](double t) {
        // B_i^1(x) 在[0,1]上为(x - 0)/(1 - 0)=x,在[1,2]上为(2 - x)/(2 - 1)=2 - x,其他为0
        if (t >= t_im1 && t <= t_i) {
            return (t - t_im1) / (t_i - t_im1);
        } else if (t > t_i && t <= t_ip1) {
            return (t_ip1 - t) / (t_ip1 - t_i);
        }
        return 0.0;
    };
    std::vector<double> knot = {0, 1, 2, 3};
    Base_Bspline spline1(knot);
    output_spline_data(spline1, "fig4.csv", 0, 3.5, 0.01);
    

    // 其他两张图可通过类似方式构造，将中间步骤的截断结果相加或相减，得到图3.2中展示的一系列中间过程图形。
    // 具体请对照书中步骤（例3.31中从截断幂函数逐步构造分差，得到中间线段，再组合出B样条），
    // 然后对中间的每一步骤（如(t - x)_+ 经过一次分差变成一条斜线，再与另一条斜线组合...），输出对应数据即可。

    cout << "Data generated. \n";

    return 0;
}
