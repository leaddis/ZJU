#include "Spline.h"
#include <fstream>
// Exact function
double f(double x) {
    return 1.0 / (1.0 + 25 * x * x);
}

void generateNodes(int N, vector<double>& nodes, vector<double>& values) {
    double step = 2.0 / (N - 1);
    nodes.resize(N);
    values.resize(N);
    for (int i = 0; i < N; ++i) {
        nodes[i] = -1.0 + i * step;
        values[i] = f(nodes[i]);
    }
}

void computeError(const Spline<3, SplineType::ppForm>& spline, const vector<double>& nodes, double& max_error) {
    max_error = 0.0;
    for (size_t i = 0; i < nodes.size() - 1; ++i) {
        double midpoint = 0.5 * (nodes[i] + nodes[i + 1]);
        double error = fabs(f(midpoint) - spline(midpoint));
        if (error > max_error) {
            max_error = error;
        }
    }
}

void saveToCSV(const string& filename, const vector<double>& x, const vector<double>& y) {
    ofstream file(filename);
    file << "x,y\n";
    for (size_t i = 0; i < x.size(); ++i) {
        file << x[i] << "," << y[i] << "\n";
    }
    file.close();
}

int main() {
    vector<int> N_values = {6, 11, 21, 41, 81};
    vector<double> errors, convergence_rates;

    cout << std::fixed << std::setprecision(6);
    cout << "N\tMax Error\tConvergence Rate" << endl;

    double previous_error = 0.0;
     for (int N : N_values) {
        // Generate nodes and function values
        vector<double> nodes, values;
        generateNodes(N, nodes, values);

        // Set interpolation conditions
        InterpCondition cond;
        cond.sites = nodes;
        cond.function_values = values;
        cond.left = natural;
        cond.right = natural;

        // Construct spline
        Spline<3, SplineType::ppForm> spline(cond);
        Spline<3, SplineType::B_spline> spline1(cond);

        // Compute error
        double max_error;
        computeError(spline, nodes, max_error);
        errors.push_back(max_error);

        // Compute convergence rate if applicable
        if (!errors.empty() && errors.size() > 1) {
            double rate = log(previous_error / max_error) / log(2.0);
            convergence_rates.push_back(rate);
        } else {
            convergence_rates.push_back(0.0);
        }
        previous_error = max_error;

 // Generate dense points for interpolation
        vector<double> dense_x, interpolated_values, interpolated_values2;
        int dense_points = 100; // Number of dense points
        double dense_step = 2.0 / (dense_points - 1);
        for (int i = 0; i < dense_points; ++i) {
            double x = -1.0 + i * dense_step;
            dense_x.push_back(x);
            interpolated_values.push_back(spline(x));
            interpolated_values2.push_back(spline1(x));
        }

        // Save dense interpolated values to CSV
        saveToCSV("ppForm_N" + to_string(N) + ".csv", dense_x, interpolated_values);
        saveToCSV("BForm_N" + to_string(N) + ".csv", dense_x, interpolated_values2);

        // Output results
        cout << N << "\t" << max_error << "\t";
        if (errors.size() > 1) {
            cout << convergence_rates.back() << endl;
        } else {
            cout << "-" << endl;
        }
    }
    return 0;
}

