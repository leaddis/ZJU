#include "CurveFitting.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

void outputFittedData3D(const std::string& filename, const Spline<3, B_spline>& splineX, const Spline<3, B_spline>& splineY, const Spline<3, B_spline>& splineZ, double step = 0.002) {
    std::ofstream file(filename);
    file << std::fixed << std::setprecision(6);
    for (double t = 0.0; t <= 1.0; t += step) {
        double x = splineX(t);
        double y = splineY(t);
        double z = splineZ(t);
        file << x << "," << y << "," << z << "\n";
    }
    file.close();
}

void outputR3CurveResults(int N, const std::string& filename) {
    CurveFitting curveFitter;

    // Generate r3 curve points
    std::vector<double> x, y, z, t;
    curveFitter.generateCurveR3(N, x, y, z);

    // Calculate chordal length parameterization
    curveFitter.calculateChordLengthParam3D(x, y,z, t);

    // Fit cubic B-spline
    Spline<3, B_spline> splineX, splineY, splineZ;
    curveFitter.fitBSpline3D(t, x, y, z, splineX, splineY, splineZ, BCType::natural, BCType::natural);

    // Output fitted data
    outputFittedData3D(filename, splineX, splineY, splineZ);
}

void outputR3CurveResults2(int N, const std::string& filename) {
    CurveFitting curveFitter;

    // Generate r3 curve points
    std::vector<double> x, y, z, t;
    curveFitter.generateCurveR3(N, x, y, z);

    // Calculate chordal length parameterization
    curveFitter.calculateSameDistanceParam(x, t);

    // Fit cubic B-spline
    Spline<3, B_spline> splineX, splineY, splineZ;
    curveFitter.fitBSpline3D(t, x, y, z, splineX, splineY, splineZ, BCType::natural, BCType::natural);

    // Output fitted data
    outputFittedData3D(filename, splineX, splineY, splineZ);
}

int main() {
    try {
        // Generate and output results for N = 10, 40, 160
        outputR3CurveResults(10, "r3_3_curve_N10.csv");
        outputR3CurveResults(40, "r3_3_curve_N40.csv");
        outputR3CurveResults(160, "r3_3_curve_N160.csv");

        outputR3CurveResults2(10, "r3_1_curve_N10.csv");
        outputR3CurveResults2(40, "r3_1_curve_N40.csv");
        outputR3CurveResults2(160, "r3_1_curve_N160.csv");

        std::cout << "R3 curve data written to CSV files." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
