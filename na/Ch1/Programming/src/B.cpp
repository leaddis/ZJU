#include <iostream>
#include "EquationSolver.h"
using namespace std;

double function1(double x)
{
    double y;
    y = 1 / x - tan(x);
    return y;
};

double function2(double x)
{
    double y;
    y = 1 / x - pow(2, x);
    return y;
};

double function3(double x)
{
    double y;
    y = pow(2, -x) + exp(x) + 2 * cos(x) - 6;
    return y;
};

double function4(double x)
{
    double y;
    y = (x * x * x + 4 * x * x + 3 * x + 5) / (2 * x * x * x - 9 * x * x + 18 * x - 2);
    return y;
};

int main()
{
    Bisection_Method solve1(0, M_PI/2, 1e-7, 1e-7, 1000, function1);
    cout << "solution is " << solve1.solve() << endl;
    cout << "f(x) is " << function1(solve1.solve()) << endl;

    Bisection_Method solve2(0, 1, 1e-7, 1e-7, 1000, function2);
    cout << "solution is " << solve2.solve() << endl;
    cout << "f(x) is " << function2(solve2.solve()) << endl;

    Bisection_Method solve3(1, 3, 1e-7, 1e-7, 1000, function3);
    cout << "solution is " << solve3.solve() << endl;
    cout << "f(x) is " << function3(solve3.solve()) << endl;

    Bisection_Method solve4(0, 4, 1e-7, 1e-7, 1000, function4);
    cout << "solution is " << solve4.solve() << endl;
    cout << "f(x) is " << function4(solve4.solve()) << endl;
}