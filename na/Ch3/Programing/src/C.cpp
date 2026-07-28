#include "Spline.h"
#include <fstream>

using namespace std;


// Exact function
double f(double x) {
    return 1.0 / (1.0 + x * x);
}

// Interpolate using cubic cardinal B-splines (Theorems 3.57 and 3.58)
void interpolate_combined() {
    vector<double> nodes_3_57, values_3_57;
    nodes_3_57.reserve(11);
    values_3_57.resize(11);
    for (int i = 0; i <= 10; i++) {
        nodes_3_57.push_back(-5.0 + i);
        values_3_57[i] = f(nodes_3_57[i]);
    }

    vector<double> nodes_3_58, values_3_58;
    nodes_3_58.reserve(10);
    values_3_58.resize(10);
    for (int i = 1; i <= 10; i++) {
        nodes_3_58.push_back(i - 11.0 / 2.0);
        values_3_58[i - 1] = f(nodes_3_58[i - 1]);
    }

    // Set interpolation conditions for complete boundary conditions
    InterpCondition cond_3_57;
    cond_3_57.sites = nodes_3_57;
    cond_3_57.function_values = values_3_57;
    cond_3_57.left = complete;
    cond_3_57.right = complete;

    InterpCondition cond_3_58;
    cond_3_58.sites = nodes_3_58;
    cond_3_58.function_values = values_3_58;
    cond_3_58.left = Thm3_58;
    cond_3_58.right = Thm3_58;

    // Construct cubic B-splines
    Spline<3, SplineType::B_spline> spline_3_57(cond_3_57);
    Spline<2, SplineType::B_spline> spline_3_58(cond_3_58);

    // Generate interpolated values for plotting
    vector<double> x_plot;
    vector<double> y_exact;
    vector<double> y_interp_3_57;
    vector<double> y_interp_3_58;

    double step = 0.1;
    for (double x = -4.5; x <= 4.5; x += step) {
        x_plot.push_back(x);
        y_exact.push_back(f(x));
        y_interp_3_57.push_back(spline_3_57(x));
        y_interp_3_58.push_back(spline_3_58(x));
    }

    // Save results to file for Python plotting
    ofstream file("C.csv");
    file << "x,y_exact,y_interp_3_57,y_interp_3_58\n";
    for (size_t i = 0; i < x_plot.size(); ++i) {
        file << x_plot[i] << "," << y_exact[i] << "," << y_interp_3_57[i] << "," << y_interp_3_58[i] << "\n";
    }
    file.close();

    cout << "Results saved to C.csv" << endl;
}

int main() {
    interpolate_combined();
    return 0;
}