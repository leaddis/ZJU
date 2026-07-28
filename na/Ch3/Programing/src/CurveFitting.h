#ifndef CURVE_FITTING_H
#define CURVE_FITTING_H

#include "Spline.h"
#include <fstream>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

class CurveFitting {
public:
    CurveFitting() = default;

    double calculateY(double x) {
        return (2.0 / 3.0) * (sqrt(3 - x * x) + sqrt(fabs(x)));
    }

    double calculateYnegative(double x) {
        return (2.0 / 3.0) * (-sqrt(3 - x * x) + sqrt(fabs(x)));
    }
    // Generate heart curve r1(t)
   void generateHeartCurveR1(int N, std::vector<double>& x, std::vector<double>& y) {
        x.clear();
        y.clear();
        double x_min = -sqrt(3);
        double x_max = sqrt(3);
        for (int i = 0; i < N/2; ++i) {
            double current_x = x_min + (4 * sqrt(3)* i)/ N;  // 均匀分布的x坐标
            double current_y = calculateY(current_x);  // 通过方程计算对应的y
            x.push_back(current_x);
            y.push_back(current_y);
        }
        x.push_back(x_max);
        y.push_back(calculateY(x_max));

        // Mirror the curve for the bottom half
        int size = x.size();
        for (int i = size - 2; i >= 0; --i) {
            x.push_back(x[i]);
            y.push_back(calculateYnegative(x[i]));
        }
    }

    // Generate r2(t) = (x(t), y(t))
    void generateCurveR2(int N, std::vector<double>& x, std::vector<double>& y) {
        x.clear();
        y.clear();
        double step = 6.0 * M_PI / (N - 1);
        for (int i = 0; i < N; ++i) {
            double t = i * step;
            x.push_back(sin(t) + t * cos(t));
            y.push_back(cos(t) - t * sin(t));
        }
    }

    // Generate r3(t) = (x(t), y(t), z(t))
    void generateCurveR3(int N, std::vector<double>& x, std::vector<double>& y, std::vector<double>& z) {
        x.clear();
        y.clear();
        z.clear();
        double step = 2.0 * M_PI / (N - 1);
        for (int i = 0; i < N; ++i) {
            double t = i * step;
            double u = cos(t);
            double v = sin(t);
            x.push_back(sin(u) * cos(v));
            y.push_back(sin(u) * sin(v));
            z.push_back(cos(u));
        }
    }

    //calculate same distance parameterization
    void calculateSameDistanceParam(const std::vector<double>& x, std::vector<double>& t) {
        t.clear();
        t.push_back(0.0);
        for (int i = 1 ; i < x.size(); ++i) {
            t.push_back(t[i - 1] + 1);
        }
        for (double& val : t) {
            val /= t.back(); // Normalize to [0, 1]
        }
    }

    // Calculate chordal length parameterization
    void calculateChordLengthParam(const std::vector<double>& x, const std::vector<double>& y, std::vector<double>& t) {
        t.clear();
        t.push_back(0.0);
        double totalLength = 0.0;
        for (size_t i = 1; i < x.size(); ++i) {
            double dx = x[i] - x[i - 1];
            double dy = y[i] - y[i - 1];
            double segmentLength = std::sqrt(dx * dx + dy * dy);
            totalLength += segmentLength;
            t.push_back(totalLength);
        }
        for (double& val : t) {
            val /= totalLength; // Normalize to [0, 1]
        }
    }

    void calculateChordLengthParam3D(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, std::vector<double>& t) {
        t.clear();
        t.push_back(0.0);
        double totalLength = 0.0;
        for (size_t i = 1; i < x.size(); ++i) {
            double dx = x[i] - x[i - 1];
            double dy = y[i] - y[i - 1];
            double dz = z[i] - z[i - 1];
            double segmentLength = std::sqrt(dx * dx + dy * dy + dz * dz);
            totalLength += segmentLength;
            t.push_back(totalLength);
        }
        for (double& val : t) {
            val /= totalLength; // Normalize to [0, 1]
        }
    }

    // Fit B-spline with user-defined boundary conditions
    void fitBSpline2D(const std::vector<double>& t, const std::vector<double>& x, const std::vector<double>& y,
                      Spline<3, B_spline>& splineX, Spline<3, B_spline>& splineY, BCType leftBC, BCType rightBC) {
        InterpCondition condX, condY;
        condX.sites = t;
        condX.function_values = x;
        condX.left = leftBC;
        condX.right = rightBC;

        condY.sites = t;
        condY.function_values = y;
        condY.left = leftBC;
        condY.right = rightBC;

        splineX = Spline<3, B_spline>(condX);
        splineY = Spline<3, B_spline>(condY);
    }

    // Fit B-spline for r3(t) with user-defined boundary conditions
    void fitBSpline3D(const std::vector<double>& t, const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                      Spline<3, B_spline>& splineX, Spline<3, B_spline>& splineY, Spline<3, B_spline>& splineZ,
                      BCType leftBC, BCType rightBC) {
        InterpCondition condX, condY, condZ;
        condX.sites = t;
        condX.function_values = x;
        condX.left = leftBC;
        condX.right = rightBC;

        condY.sites = t;
        condY.function_values = y;
        condY.left = leftBC;
        condY.right = rightBC;

        condZ.sites = t;
        condZ.function_values = z;
        condZ.left = leftBC;
        condZ.right = rightBC;

        splineX = Spline<3, B_spline>(condX);
        splineY = Spline<3, B_spline>(condY);
        splineZ = Spline<3, B_spline>(condZ);
    }

    // Compare results for different boundary conditions
    void compareResults(const std::vector<double>& x, const std::vector<double>& y, const Spline<3, B_spline>& splineX, const Spline<3, B_spline>& splineY, const std::string& filename) {
        std::ofstream file(filename);
        file << std::fixed << std::setprecision(6);

        double step = 0.01;
        for (double t = 0.0; t <= 1.0; t += step) {
            double interpolatedX = splineX(t);
            double interpolatedY = splineY(t);
            file << interpolatedX << "," << interpolatedY << "\n";
        }

        file.close();
        std::cout << "Results written to " << filename << std::endl;
    }
};

#endif // CURVE_FITTING_H