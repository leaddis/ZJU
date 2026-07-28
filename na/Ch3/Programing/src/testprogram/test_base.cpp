#include <iostream>
#include <fstream>
#include <vector>
#include "Base_Bspline.h"

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

int main() {
    //1阶样条
    std::vector<double> knots = {0, 1, 2};
    Base_Bspline spline(knots);
    output_spline_data(spline, "spline.csv", 0, 2, 0.01);

    //2阶样条
    std::vector<double> knots1 = {0, 1, 2, 3};
    Base_Bspline spline1(knots1);
    output_spline_data(spline1, "spline1.csv", 0, 3, 0.01);

    // 3阶样条
    std::vector<double> knots2 = {-3,-2,-1,0,1};
    Base_Bspline spline2(knots2);
    output_spline_data(spline2, "spline2.csv", -3, 1, 0.01);

    // 4阶样条
    std::vector<double> knots3 = {1.0, 2.0, 3.0, 4.0, 5.0, 7.0};
    Base_Bspline spline3(knots3);
    output_spline_data(spline3, "spline3.csv", 0.0, 7.0, 0.01);

    // // 3阶样条
    // std::vector<double> knots3 = {2.0, 3.0, 4.0, 5.0, 7.0,8.0};
    // Base_Bspline spline4(knots3);

    // Base_Bspline spline5;
    // spline5 = spline3 + spline4;
    

    // 输出多项式
    std::cout << "Polynomials of spline3:" << std::endl;
    cout<<spline2.poly(0).toString()<<endl;
    cout<<spline2.poly(1).toString()<<endl;
    cout<<spline2.poly(2).toString()<<endl; 
    cout<<spline2.poly(3).toString()<<endl;
    cout<<spline2.poly(4).toString()<<endl;


    return 0;
}
