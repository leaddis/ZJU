#include "Base_Spline.h"
/// @brief 在基样条的定义中执行单个递归步骤
/// @tparam M order of the current spline
/// @param lhs B_i^n(x)
/// @param rhs B_{i+1}^n(x)
/// @return the new base spline of order-(M+1)
Base_Spline upgrade(const Base_Spline &lhs, const Base_Spline &rhs)
{
    const int M = lhs.knots.size() - 2;
    Base_Spline res;
    for (int i = -1; i <= M; i++)
        res.knots.push_back(lhs.knot(i));
    res.knots.push_back(rhs.knot(M));
    for (int i = 0; i <= M + 1; i++)
    {
        Polynomial p = lhs.poly(i);
        p *= Polynomial{-res.knot(-1), 1.0};
        if ((res.knot(M) - res.knot(-1)) == 0)
            p = Polynomial{0};

        else
            p /= (res.knot(M) - res.knot(-1));
        Polynomial q = rhs.poly(i - 1);
        q *= Polynomial{res.knot(M + 1), -1.0};
        if ((res.knot(M + 1) - res.knot(0)) == 0)
            q = Polynomial{0};
        else
            q /= (res.knot(M + 1) - res.knot(0));
        res.pp.push_back(p + q);
    }
    return res;
}
/*
/// @brief 模板类 Base_Spline<0> 的显式具体化
template <>
class Base_Spline<0>
{
private:
    double knot1;
    double knot2;

public:
    Base_Spline() = default;
    Base_Spline(double k1, double k2) : knot1(k1), knot2(k2) {}
    double d(double x) const { return 0; }
    double dd(double x) const { return 0; }
    double operator()(double x) const
    {
        if (x <= knot1 || x > knot2)
            return 0;
        return 1;
    }
    double knot(int index) const
    {
        if (index == -1)
            return knot1;
        else if (index == 0)
            return knot2;
        else
            throw "结点下标有误！";
    }
};
*/

Base_Spline::Base_Spline(const std::vector<double> &sites)
{
    const int N = sites.size() - 2;
    if (sites.size() < 2)
        throw "too less sites for base splines.";
    if (N == 0)
    {
        Polynomial p0{1.0};
        pp.push_back(p0);
        knots.push_back(sites[0]);
        knots.push_back(sites[1]);
    }
    else
    {
        std::vector<double> left = sites;
        left.pop_back();
        std::vector<double> right;
        for (size_t i = 1; i < sites.size(); i++)
            right.push_back(sites[i]);
        *this = upgrade(Base_Spline(left), Base_Spline(right));
    }
}

Base_Spline::Base_Spline(const std::initializer_list<double> &l)
{
    std::vector<double> arg;
    for (auto x : l)
        arg.push_back(x);
    *this = Base_Spline(arg);
}

double Base_Spline::knot(int index) const
{
    const int N = knots.size() - 2;
    if (index < -1 || index > N)
        throw std::invalid_argument{"结点下标错误！"};
    return knots[index + 1];
}

const Polynomial Base_Spline::poly(int index) const
{
    const int N = knots.size() - 2;
    if (index < 0 || index > N)
        return Polynomial{0.0};
    return pp[index];
}

double Base_Spline::operator()(double x) const
{
    const int N = knots.size() - 2;
    if (x < knot(-1) || x > knot(N))
        return 0;
    else if (N == 0)
        return 1;
    /*
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
        {
            int j = i;
            while(knot(j-1) == knot(j) && j< N)
                j++;
            return pp[j](x);
        }
    }
    */
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
            return pp[i](x);
    }
    if (x == knot(N))
        return pp[N](x);
    return 0; // never reached
}

double Base_Spline::d(double x) const
{
    const int N = knots.size() - 2;
    if (x <= knot(-1) || x >= knot(N))
        return 0;
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
            return pp[i].diff(x);
    }
    if (x == knot(N))
        return pp[N].diff(x);
    return 0; // never reached
}

double Base_Spline::dd(double x) const
{
    const int N = knots.size() - 2;
    if (x <= knot(-1) || x >= knot(N))
        return 0;
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
            return pp[i].diff(2, x);
    }
    if (x == knot(N))
        return pp[N].diff(2, x);
    return 0; // never reached
}

double Base_Spline::d(int degree, double x) const
{
    const int N = knots.size() - 2;
    //    if (is_knot(x) && degree >= N)
    //        throw "样条曲线无法微分到所需阶次！";
    //    if (x <= knot(-1) || x >= knot(N))
    //        return 0;
    if (x < knot(-1) || x > knot(N))
        return 0;
    if (x == knot(-1))
    {
        for (int i = 0; i <= N; i++)
        {
            if (x < knot(i))
            {
                return pp[i].diff(degree, x);
            }
        }
    }
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
            return pp[i].diff(degree, x);
    }
    if (x == knot(N))
    {
        return pp[N].diff(degree, x);
    }
    return 0; // never reached
}

bool Base_Spline::is_knot(double x) const
{
    for (auto k : knots)
        if (fabs(k - x) <= 1e-12)
            return true;
    return false;
}
