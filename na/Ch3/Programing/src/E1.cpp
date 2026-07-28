#include "CurveFitting.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

void outputFittedData(const std::string& filename, const Spline<3, B_spline>& splineX, const Spline<3, B_spline>& splineY, double step = 0.002) {
    std::ofstream file(filename);
    file << std::fixed << std::setprecision(6);
    for (double t = 0.0; t <= 1.0; t += step) {
        double x = splineX(t);
        double y = splineY(t);
        file << x << "," << y << "\n";
    }
    file.close();
}

void outputHeartCurveResults(int N, const std::string& filename) {
    CurveFitting curveFitter;

    // Generate heart curve points
    std::vector<double> x, y, t;
    curveFitter.generateHeartCurveR1(N, x, y);

    // Calculate chordal length parameterization
    curveFitter.calculateChordLengthParam(x, y, t);

    // Fit cubic B-spline
    Spline<3, B_spline> splineX, splineY;
    curveFitter.fitBSpline2D(t, x, y, splineX, splineY, BCType::natural, BCType::natural);

    // Output fitted data
    outputFittedData(filename, splineX, splineY);
}

void outputHeartCurveResults2(int N, const std::string& filename) {
    CurveFitting curveFitter;

    // Generate heart curve points
    std::vector<double> x, y, t;
    curveFitter.generateHeartCurveR1(N, x, y);

    // Calculate chordal length parameterization
    curveFitter.calculateSameDistanceParam(x, t);

    // Fit cubic B-spline
    Spline<3, B_spline> splineX, splineY;
    curveFitter.fitBSpline2D(t, x, y, splineX, splineY, BCType::natural, BCType::natural);

    // Output fitted data
    outputFittedData(filename, splineX, splineY);
}

int main() {
    try {
        // Generate and output results for N = 10, 40, 160
        outputHeartCurveResults(10, "heart_1_curve_N10.csv");
        outputHeartCurveResults(40, "heart_1_curve_N40.csv");
        outputHeartCurveResults(160, "heart_1_curve_N160.csv");

        outputHeartCurveResults(10, "heart_2_curve_N10.csv");
        outputHeartCurveResults(40, "heart_2_curve_N40.csv");
        outputHeartCurveResults(160, "heart_2_curve_N160.csv");

        std::cout << "Heart curve data written to CSV files." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}