#include "EquationSolver.h"

using namespace std;

double Bisection_Method::solve() {
    double fa = f(a);
    double fb = f(b);
    if (fa * fb > 0) {
        throw "f(a) and f(b) must have opposite signs";
    }
    double c = a;
    double w,h;
    for (int i = 0; i < Maxiter; i++) {
        h = b - a;
        c = a + h / 2;
        w = f(c);
        if(fabs(h) < delta || fabs(w) < eps)
	        break;
        else if( w *fa <= 0){
            b = c;
            fb = w;
        }
        else{
            a = c;
            fa = w;
        }    
    }
    return c;
}


double Newton_Method::solve() {
    double x = x0;
    double u;
    double dy = df(x);
    // Newton's method failed: derivative is zero
    if (fabs(dy) < eps) {
        throw "Newton's method failed: derivative is zero";
    }
    for (int i = 0; i < Maxiter; i++) {
        u =f(x) ;
        if (fabs(u) < eps) {
            break;
        }
        x = x - u/dy;
    }
    return x;
}

double Secant_Method::solve() {
    double x_n =x1;
    double x_n1 = x0;
    double u = f(x_n);
    double v = f(x_n1);
    int k = 2;
    double s;
    for (k=2;k<=Maxiter;k++){
        s =(x_n - x_n1)/(u - v);
        x_n1 = x_n;
        v = u;
        x_n = x_n - u * s;
        u = f(x_n);
        if(fabs(x_n - x_n1) < delta || fabs(u) < eps)
            break;
    }
    return x_n;
}