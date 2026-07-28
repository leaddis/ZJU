#include "Bezier.h"

Point Curve::getPoint(int j) const {
    return points[j];
}

    // 获取第j个点的切向量
Point Curve::getTangent(int j) const {
    return tangents[j];
}

Point BezierCurve::calculatePoint(double t) {
    double x = std::pow(1 - t, 3) * control_points[0].x +
                   3 * t * std::pow(1 - t, 2) * control_points[1].x +
                   3 * std::pow(t, 2) * (1 - t) * control_points[2].x +
                   std::pow(t, 3) * control_points[3].x;
        double y = std::pow(1 - t, 3) * control_points[0].y +
                   3 * t * std::pow(1 - t, 2) * control_points[1].y +
                   3 * std::pow(t, 2) * (1 - t) * control_points[2].y +
                   std::pow(t, 3) * control_points[3].y;
        return Point(x, y);
}

void BezierCurve::approximateCurveWithBezier(const Curve& curve, int m, int num_points_per_segment, const std::string& filename) {
    std::ofstream file(filename);

    for (int j = 0; j < m; ++j) {
        Point p_j = curve.getPoint(j);
        Point p_j1 = curve.getPoint(j + 1);
        Point tangent = curve.getTangent(j);

        Point q_0 = p_j;
        Point q_1 = Point(p_j.x + tangent.x / 3.0, p_j.y + tangent.y / 3.0);
        Point q_2 = Point(p_j1.x - tangent.x / 3.0, p_j1.y - tangent.y / 3.0);
        Point q_3 = p_j1;

        BezierCurve bezier({q_0, q_1, q_2, q_3});

        for (int i = 0; i <= num_points_per_segment; ++i) {
            double t = static_cast<double>(i) / num_points_per_segment;
            Point p = bezier.calculatePoint(t);
            file << p.x << " " << p.y << std::endl;
        }
    }
    
    file.close();
}