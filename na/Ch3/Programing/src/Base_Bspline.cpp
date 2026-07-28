#include "Base_Bspline.h"

Base_Bspline::Base_Bspline(const vector<double> &x)
{
    const int N = x.size() - 2;
    if (x.size() < 2)
        throw "结点数量过少！";
        if (N == 0)
        {
            Polynomial p0{1.0};
            pp.push_back(p0);
            knots.push_back(x[0]);
            knots.push_back(x[1]);
        }
        else
        {
            std::vector<double> left = x;
            left.pop_back();
            std::vector<double> right;
            for (size_t i = 1; i < x.size(); i++)
                right.push_back(x[i]);
            *this = calulate(Base_Bspline(left), Base_Bspline(right));
        }
}

double Base_Bspline::d(double x) const{
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
    return 0.0; 
};

double Base_Bspline::d(int degree, double x) const{
    const int N = knots.size() - 2;
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
    return 0.0; 
};

double Base_Bspline::knot(int index) const{

    const int N = knots.size() - 2;
    if (index < -1 || index > N)
        throw std::invalid_argument{"结点下标错误！"};
    return knots[index + 1];
};

const Polynomial Base_Bspline::poly(int index) const{
    const int N = knots.size() - 2;
    if (index < 0 || index > N)
        return Polynomial{0.0};
    return pp[index];
};

double Base_Bspline::operator()(double x) const
{
    const int N = knots.size() - 2;
    if (x < knot(-1) || x > knot(N))
        return 0;
    else if (N == 0)
        return 1;
    for (int i = 0; i <= N; i++)
    {
        if (x < knot(i))
            return pp[i](x);
    }
    if (x == knot(N))
        return pp[N](x);
    return 0.0; 
}

Base_Bspline calulate (const Base_Bspline &lhs, const Base_Bspline &rhs)
{
    Base_Bspline res;
    const int N = lhs.knots.size() - 2;
    for (int i = -1; i <= N; i++)
    {
        res.knots.push_back(lhs.knot(i));
    }
    res.knots.push_back(rhs.knot(N));
    for (int i = 0; i <= N + 1; i++)
    {
        Polynomial p = lhs.poly(i);
        p *= Polynomial{-res.knot(-1), 1.0};
        if ((res.knot(N) - res.knot(-1)) == 0)
            p = Polynomial{0};

        else
            p /= (res.knot(N) - res.knot(-1));
        Polynomial q = rhs.poly(i - 1);
        q *= Polynomial{res.knot(N + 1), -1.0};
        if ((res.knot(N + 1) - res.knot(0)) == 0)
            q = Polynomial{0};
        else
            q /= (res.knot(N + 1) - res.knot(0));
        res.pp.push_back(p + q);
    }
    return res;

}
