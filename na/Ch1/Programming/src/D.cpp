#include <iostream>
#include "EquationSolver.h"                                                             
using namespace std;

double f1(double x)
{
    double y;
    y = sin(x / 2) - 1;
    return y;
};

double f2(double x)
{
    double y;
    y = exp(x) - tan(x);
    return y;
};

double f3(double x)
{
    double y;
    y = x * x * x - 12 * x * x + 3 * x + 1;
    return y;
};

int main()
{
    Secant_Method solve1(-3*M_PI, -3.5*M_PI, 1e-7, 1e-7, 1000, f1);
    cout << "solution is " << solve1.solve() << endl;
    cout << "f(x) is " << f1(solve1.solve()) << endl;

    Secant_Method solve2(4, 5, 1e-7, 1e-7, 1000, f2);
    cout << "solution is " << solve2.solve() << endl;
    cout << "f(x) is " << f2(solve2.solve()) << endl;

    Secant_Method solve3(11, 12, 1e-7, 1e-7, 1000, f3);
    cout << "solution is " << solve3.solve() << endl;
    cout << "f(x) is " << f3(solve3.solve()) << endl;
}