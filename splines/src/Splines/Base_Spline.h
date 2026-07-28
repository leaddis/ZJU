#ifndef BASE_SPLINE_H
#define BASE_SPLINE_H

#include <cmath>
#include <initializer_list>
#include <vector>
#include <any>
#include "BasicHeadFile/Polynomial.h"

// N阶基样条函数
class Base_Spline
{
private:
    std::vector<double> knots;  // 基样条的结点, t_{i-1} to t_{i+N}
    std::vector<Polynomial> pp; // 样条多项式
    // pp[i] 指区间 [t_{i-1}, t_{i}] 上的多项式
public:
    Base_Spline() = default;
    Base_Spline(const std::vector<double> &);
    Base_Spline(const std::initializer_list<double> &);

public:
    double operator()(double x) const;
    double d(double x) const;
    double dd(double x) const;
    double d(int degree, double x) const;

    /// @brief 从两个基样条构造高一阶的样条。###
    ///        这里要求LHS和RHS的结点必须相互交叉，如BSpline的定义所示，否则此函数的行为是未定义的。
    /// @tparam M order of the two lower order base splines
    /// @param lhs B_i^{M}(x)
    /// @param rhs B_{i+1}^{M}(x)
    /// @return B_{i}^{M+1}(x)
    /// @details 
    friend Base_Spline upgrade(const Base_Spline &lhs, const Base_Spline &rhs);

public:
    /// @brief 返回带有对应index的结点。
    /// @param index is in [-1,N], which means that knot(-1) will return t_{-1}
    /// @return knot[index]
    double knot(int index) const;

    /// @brief 返回相应区间的多项式。
    /// @param index is in [0,N], which means thatploy(0) will return p(x) of [ t_{i-1} , t_{i} ]
    /// @return p(x) in [ t_{index-1}, t_{index} ]
    const Polynomial poly(int index) const;

private:
    /// @brief 判定x是否是此基样条的结点。
    /// @param x input coordinate
    /// @return bool
    bool is_knot(double x) const;
};


#else
// do nothing
#endif
 