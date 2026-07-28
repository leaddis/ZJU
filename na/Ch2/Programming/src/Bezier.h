#ifndef BEZIER_H
#define BEZIER_H
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
// 定义点结构体
struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

class Curve {
protected:
    std::vector<Point> points;
    std::vector<Point> tangents;
public:
    Curve(const std::vector<Point>& pts, const std::vector<Point>& tangs): points(pts), tangents(tangs) {}
    // 获取第j个点
    Point getPoint(int j) const; 
    // 获取第j个点的切向量
    Point getTangent(int j) const; 
};

class BezierCurve{
    protected:
        std::vector<Point> control_points;
    public:
        BezierCurve(const std::vector<Point>& points) : control_points(points) {}
        Point calculatePoint(double t) ;
        void approximateCurveWithBezier(const Curve& curve, int m, int num_points_per_segment, const std::string& filename) ;
};

#endif //BEZIER_H