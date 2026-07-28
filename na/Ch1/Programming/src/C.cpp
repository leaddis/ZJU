#include <iostream>
#include "EquationSolver.h"                                                            
using namespace std;

double f(double x)
{
    double y;
    y = x - tan(x);
    return y;
};

double df(double x)
{
    double y;
    y = 1 - 1 / (cos(x)*cos(x));
    return y;
};

int main(){
    Newton_Method solve1(4.5, 1e-7, 1000, f, df);
    cout << "solution is " << solve1.solve() << endl;
    cout << "f(x) is " << f(solve1.solve()) << endl;

    Newton_Method solve2(7.7, 1e-7, 1000, f, df);
    cout << "solution is " << solve2.solve() << endl;
    cout << "f(x) is " << f(solve2.solve()) << endl;
}