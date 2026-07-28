#include "Bezier.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

// 心形曲线方程的实现
double calculateY(double x) {
    return (2.0 / 3.0) * (sqrt(3 - x * x) + sqrt(fabs(x)));
}

double calculateYnegative(double x) {
    return (2.0 / 3.0) * (-sqrt(3 - x * x) + sqrt(fabs(x)));
}

// 生成心形曲线上的点
std::vector<Point> generateHeartPointspossitive(int num_points) {
    std::vector<Point> points;
    double x_min = -sqrt(3);
    double x_max = sqrt(3);

    for (int i = 0; i <= num_points; ++i) {
        double x = x_min + (x_max - x_min) * i / num_points;  // 均匀分布的x坐标
        double y = calculateY(x);  // 通过方程计算对应的y
        points.push_back(Point(x, y));
    }
    return points;
}

std::vector<Point> generateHeartPointsnegative(int num_points) {
    std::vector<Point> points;
    double x_min = -sqrt(3);
    double x_max = sqrt(3);

    for (int i = 0; i <= num_points; ++i) {
        double x = x_max - (x_max - x_min) * i / num_points;  // 均匀分布的x坐标
        double y = calculateYnegative(x);  // 通过方程计算对应的y
        points.push_back(Point(x, y));
    }
    return points;
}

// 生成心形曲线的切向量（使用相邻点之间的差值来估算）
std::vector<Point> generateHeartTangents(const std::vector<Point>& points) {
    std::vector<Point> tangents;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        double dx = points[i + 1].x - points[i].x;
        double dy = points[i + 1].y - points[i].y;
        tangents.push_back(Point(dx, dy));
    }
    tangents.push_back(tangents.back()); // 最后一项重复前一项
    return tangents;
}

int main() {
    int m_values[] = {10, 40, 160};  // 3 种不同的分段数

    for (int m : m_values) {
        // 生成心形曲线上的点和切向量
        std::vector<Point> heartpointpositive = generateHeartPointspossitive(m);
        std::vector<Point> heartTangentspositive = generateHeartTangents(heartpointpositive);
        std::vector<Point> heartpointnegative = generateHeartPointsnegative(m);
        std::vector<Point> heartTangentsnegative = generateHeartTangents(heartpointnegative);
        // 创建心形曲线
        Curve heartCurvepositive(heartpointpositive, heartTangentspositive);
        Curve heartCurvenegative(heartpointnegative, heartTangentsnegative);
        // 生成贝塞尔曲线近似
        BezierCurve bezier_1({});//初始化贝塞尔曲线
        std::string filename1 = "heart_bezier_m_" + std::to_string(m)+ "positive" + ".txt";

        bezier_1.approximateCurveWithBezier(heartCurvepositive, m, 20, filename1);

        BezierCurve bezier_2({});//初始化贝塞尔曲线
        std::string filename2 = "heart_bezier_m_" + std::to_string(m)+ "negative" + ".txt";
        bezier_2.approximateCurveWithBezier(heartCurvenegative, m, 20, filename2);

        std::cout << "Bezier curve points for m=" << m << " are written" << std::endl;
    }

    return 0;
}
