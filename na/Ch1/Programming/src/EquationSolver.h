#ifndef EQUATION_SOLVER_H
#define EQUATION_SOLVER_H

#include <cmath>
#include <iostream>

using namespace std;

class EquationSolver {
    public:
        virtual double solve() = 0 ;
        virtual ~EquationSolver() = default;
};

class Bisection_Method: public EquationSolver {
    private:
        double a, b;
        double eps, delta;
        int Maxiter;
        double (*f)(double); //函数指针
    public:
        Bisection_Method( double a, double b, double delta, double eps, int Maxiter, double (*f)(double)) : a(a), b(b), eps(eps), delta(delta), Maxiter(Maxiter),f(f) {}
        double solve() override;
};

class Newton_Method: public EquationSolver{
    private:
        double x0;
        double eps;
        int Maxiter;
        double (*f)(double); //函数
        double (*df)(double); //一阶导数
    public:
        Newton_Method(double x0, double eps, int Maxiter, double (*f)(double), double (*df)(double)) : x0(x0), eps(eps), Maxiter(Maxiter), f(f), df(df) {}
        double solve() override;  
};

class Secant_Method: public EquationSolver{
    private:
        double x0, x1;
        double eps,delta;
        int Maxiter;
        double (*f)(double); //函数
    public:
        Secant_Method(double x0, double x1, double eps, double delta, int Maxiter, double (*f)(double)) : x0(x0), x1(x1), eps(eps), delta(delta), Maxiter(Maxiter), f(f) {}
        double solve() override;  
};






#endif // EQUATION_SOLVER_H