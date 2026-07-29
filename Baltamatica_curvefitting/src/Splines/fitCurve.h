#ifndef FITCURVE_H
#define FITCURVE_H

#include "Spline.h"

// 样条曲线类
template <int Order, SplineType t = SplineType::ppForm>
class SplineCurve {
private:
    std::vector<Spline<Order, t>> splines;
    std::vector<double> knots;
    int Dim;

public:
    SplineCurve() = default;
    SplineCurve(const std::vector<Spline<Order, t>> &_s, const std::vector<double> &_k)
        : splines(_s), knots(_k), Dim(_s.size()) {}
    Point CurveValue(double x) const;
    // 点的最大弦长
    double parameter_bound() const { return knots.back(); }
};

template <int Order, SplineType t>
Point SplineCurve<Order, t>::CurveValue(double x) const {
    assert(x >= 0 && x <= knots.back());
    for (size_t i = 0; i < knots.size() - 1; i++)
    {
        if (x <= knots[i + 1])
        {
            std::vector<double> res;
            for (size_t i = 0; i < splines.size(); i++)
                res.push_back(splines[i](x));
            return Point(res);
        }
    }
    return Point(); // never reached, quiet the compiler
}

// 通过分段线性样条进行曲线拟合
SplineCurve<1> fitCurve(const std::vector<Point> &points){
    int Dim = points.front().dim();
    // 计算输入点的累积弦长
    std::vector<double> chordalLength(1);
    for (size_t i = 1; i < points.size(); i++)
        chordalLength.push_back(chordalLength.back() + norm(points[i] - points[i - 1]));
    // there will be Dim splines to approximate the curve since the dimension of points is Dim
    std::vector<InterpCondition> conditions(Dim);
    std::vector<Spline<1, SplineType::ppForm>> splines(Dim);
    for (int j = 0; j < Dim; j++)
    {
        std::vector<double> coordinate;
        for (size_t i = 0; i < points.size(); i++)
            coordinate.push_back(points[i][j]);
        conditions[j].sites = chordalLength;
        conditions[j].function_values = coordinate;
        splines[j] = Spline<1, SplineType::ppForm>(conditions[j]);
    }
    return SplineCurve<1>(splines, chordalLength);
}

// 通过三次样条进行曲线拟合
SplineCurve<3> fitCurve(const std::vector<Point> &points, BCType t,
                        const Point &d1 = Point(), const Point &d2 = Point()){
    int Dim = points.front().dim();
    // 计算输入点的累积弦长
    std::vector<double> chordalLength(1);
    for (size_t i = 1; i < points.size(); i++)
        chordalLength.push_back(chordalLength.back() + norm(points[i] - points[i - 1]));
    // there will be Dim splines to approximate the curve since the dimension of points is Dim
    std::vector<InterpCondition> conditions(Dim);
    std::vector<Spline<3, SplineType::ppForm>> splines(Dim);
    for (int j = 0; j < Dim; j++) {
        std::vector<double> coordinate;
        for (size_t i = 0; i < points.size(); i++)
            coordinate.push_back(points[i][j]);
        conditions[j].sites = chordalLength;
        conditions[j].function_values = coordinate;
        conditions[j].bc = t;
        if (t == BCType::complete || t == BCType::specified_2nd)
        {
            conditions[j].derivative1 = d1[j];
            conditions[j].derivative2 = d2[j];
        }
        splines[j] = Spline<3, SplineType::ppForm>(conditions[j]);
    }
    return SplineCurve<3>(splines, chordalLength);
}
#else
// do nothing
#endif