#include <iostream>
#include "EquationSolver.h"                                                             
using namespace std;

double f1(double x)
{
    double y;
    double a = 11.5*M_PI/180;
    double A = 89*sin(a);
    double B = 89*cos(a);
    double C = (49 + 0.5*55)*sin(a) - 0.5*55*tan(a);
    double E = (49 + 0.5*55)*cos(a) - 0.5*55;
    y = A*sin(x)*cos(x) + B*sin(x)*sin(x) - C*cos(x) - E*sin(x);
    return y;
};

double df1(double x)
{
    double y;
    double a = 11.5*M_PI/180;
    double A = 89*sin(a);
    double B = 89*cos(a);
    double C = (49 + 0.5*55)*sin(a) - 0.5*55*tan(a);
    double E = (49 + 0.5*55)*cos(a) - 0.5*55;
    y = A*(cos(x) * cos(x) - sin(x) * sin(x)) + B*2*sin(x)*cos(x) + C*sin(x) - E*cos(x);
    return y;
};

double f2(double x)
{
  double y;
  double b = 11.5*M_PI/180;
  double A = 89*sin(b);
  double B = 89*cos(b);
  double C = (49 + 0.5*30)*sin(b) - 0.5*30*tan(b);
  double E = (49 + 0.5*30)*cos(b) - 0.5*30;
  y = A*sin(x)*cos(x) + B*sin(x)*sin(x) - C*cos(x) - E*sin(x);
  return y;
};

double df2(double x)
{
    double y;
    double b = 11.5*M_PI/180;
    double A = 89*sin(b);
    double B = 89*cos(b);
    double C = (49 + 0.5*30)*sin(b) - 0.5*30*tan(b);
    double E = (49 + 0.5*30)*cos(b) - 0.5*30;
    y = A*(cos(x) * cos(x) - sin(x) * sin(x)) + B*2*sin(x)*cos(x) + C*sin(x) - E*cos(x);
    return y;
};
int main()
{
    Newton_Method solve1(33*M_PI/180, 1e-7, 1000, f1, df1);
    cout << "solution is " << solve1.solve()/M_PI*180 << endl;
    cout << "f(x) is " << f1(solve1.solve()) << endl;

    Newton_Method solve2(33*M_PI/180, 1e-7, 1000, f2, df2);
    cout << "solution is " << solve2.solve()/M_PI*180 << endl;
    cout << "f(x) is " << f2(solve2.solve()) << endl;

    //0
    Secant_Method solve3(0,10*M_PI/180, 1e-7 ,1e-7, 1000, f1);
    cout << "solution is " << solve3.solve()/M_PI*180 << endl;
    cout << "f(x) is " << f1(solve3.solve()) << endl;

    //90
    Secant_Method solve4(80*M_PI/180,90*M_PI/180, 1e-7 ,1e-7, 1000, f1);
    cout << "solution is " << solve4.solve()/M_PI*180 << endl;
    cout << "f(x) is " << f1(solve4.solve()) << endl;
    return 0;
}