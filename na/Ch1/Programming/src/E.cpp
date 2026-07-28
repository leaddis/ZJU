#include <iostream>
#include "EquationSolver.h"                                                             
using namespace std;

double f(double x)
{
    double y;
    y = -12.4+10*(0.5 * M_PI - asin(x) - x * sqrt(1 - x * x));
    return y;
};

double df(double x){
    double y;
    y = -20*sqrt(1-x*x);
    return y;
}

int main()
{
    Bisection_Method solve1(0, 1, 1e-7, 1e-7, 1000, f);
    cout << "solution is " << solve1.solve() << endl;
    cout << "f(x) is " << f(solve1.solve()) << endl;

    Newton_Method solve2(0, 1e-7, 1000, f, df);
    cout << "solution is " << solve2.solve() << endl;
    cout << "f(x) is " << f(solve2.solve()) << endl;

    Secant_Method solve3(0, 1, 1e-7, 1e-7, 1000, f);
    cout << "solution is " << solve3.solve() << endl;
    cout << "f(x) is " << f(solve3.solve()) << endl;
}