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
    cond_3_57.left = periodic;
    cond_3_57.right = periodic;

    InterpCondition cond_3_58;
    cond_3_58.sites = nodes_3_58;
    cond_3_58.function_values = values_3_58;
    cond_3_58.left = periodic;
    cond_3_58.right = periodic;

    // Construct cubic B-splines
    Spline<3, SplineType::B_spline> spline_3_57(cond_3_57);
    Spline<2, SplineType::B_spline> spline_3_58(cond_3_58);

    // Generate interpolated values for plotting
    vector<double> x_plot;
    vector<double> y_exact;
    vector<double> y_interp_3_57;
    vector<double> y_interp_3_58;

    for (double x = -4.5; x <= 4.5; x += 0.1) {
        x_plot.push_back(x);
        y_exact.push_back(f(x));
        y_interp_3_57.push_back(spline_3_57(x));
        y_interp_3_58.push_back(spline_3_58(x));
    }

    // Calculate errors
    vector<double> error_3_57;
    vector<double> error_3_58;
    for (size_t i = 0; i < x_plot.size(); i++) {
        error_3_57.push_back(abs(y_exact[i] - y_interp_3_57[i]));
        error_3_58.push_back(abs(y_exact[i] - y_interp_3_58[i]));
    }

    // Output errors to a file
    ofstream error_file("errors.txt");
    error_file << fixed << setprecision(6);
    error_file << "x\tExact\tInterp_3_57\tError_3_57\tInterp_3_58\tError_3_58\n";
    for (size_t i = 0; i < x_plot.size(); i++) {
        error_file << x_plot[i] << "\t" << y_exact[i] << "\t" << y_interp_3_57[i] << "\t" << error_3_57[i] << "\t" 
                   << y_interp_3_58[i] << "\t" << error_3_58[i] << "\n";
    }
    error_file.close();

    // Print summary of maximum errors
    double max_error_3_57 = *max_element(error_3_57.begin(), error_3_57.end());
    double max_error_3_58 = *max_element(error_3_58.begin(), error_3_58.end());

    cout << "Maximum error for spline 3.57: " << max_error_3_57 << endl;
    cout << "Maximum error for spline 3.58: " << max_error_3_58 << endl;

}

int main() {
    interpolate_combined();
    return 0;
}