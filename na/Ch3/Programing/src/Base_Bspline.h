#ifndef BASE_BSPLINE_H
#define BASE_BSPLINE_H

#include "Polynomial.h"
#include <cmath>
#include <vector>

using namespace std;

class Base_Bspline
{
    private:
        vector<double> knots;
        vector<Polynomial> pp;
    
    public:
        Base_Bspline() = default;
        Base_Bspline(const vector<double> &);

    public:
        double operator()(double x) const;
        double d(double x) const;               //一阶导数值
        double d(int degree, double x) const; //degree次导数值
    
    public: 
        /// @brief 从两个基样条构造高一阶的样条。
        ///        这里要求LHS和RHS的结点必须相互交叉，如BSpline的定义所示，否则此函数的行为是未定义的。
        /// @tparam M 高一阶的基样条的次数
        /// @param lhs B_i^{M}(x)
        /// @param rhs B_{i+1}^{M}(x)
        /// @return B_{i}^{M+1}(x)
        /// @details 
        friend Base_Bspline calulate(const Base_Bspline &lhs, const Base_Bspline &rhs);
    
    public:
    /// @brief 返回带有对应index的结点。
    /// @param index is in [-1,N], which means that knot(-1) will return t_{-1}
    /// @return knot[index]
    double knot(int index) const;

    /// @brief 返回相应区间的多项式。
    /// @param index is in [0,N], which means thatploy(0) will return p(x) of [ t_{i-1} , t_{i} ]
    /// @return p(x) in [ t_{index-1}, t_{index} ]
    const Polynomial poly(int index) const;
};




#endif // BASE_BSPLINE_H