#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

double f(double x)
{
    return (x*x*x*x*x*x*x*x - 8*x*x*x*x*x*x*x + 28*x*x*x*x*x*x - 56*x*x*x*x*x
            + 70*x*x*x*x - 56*x*x*x + 28*x*x - 8*x + 1);
}

double g(double x)
{
    return ((((((((x - 8)*x + 28)*x - 56)*x + 70)*x - 56)*x + 28)*x - 8)*x + 1);
}

double h(double x)
{
    return ((x-1) * (x-1) * (x-1) * (x-1) * (x-1) * (x-1) * (x-1) * (x-1));
}

int main()
{
    double eps = (1.01 - 0.99) / 100;
    vector<double> x_vals, f_vals, g_vals, h_vals;

    // Open file for output
    ofstream fout;
    fout.open("A_data.csv");
    fout << "x,f(x),g(x),h(x)\n";

    // Compute values and write to file
    for (int i = 0; i <= 100; ++i)
    {
        double x = 0.99 + i * eps;
        double fx = f(x);
        double gx = g(x);
        double hx = h(x);

        x_vals.push_back(x);
        f_vals.push_back(fx);
        g_vals.push_back(gx);
        h_vals.push_back(hx);

        fout << fixed << setprecision(35) << x << "," << fx << "," << gx << "," << hx << "\n";
    }

    fout.close();
    cout << "Data has been written to A_data.csv" << endl;

    return 0;
}

