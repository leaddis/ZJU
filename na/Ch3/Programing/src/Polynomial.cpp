#include "Polynomial.h"

//==============constructors==================================
Polynomial::Polynomial()
{
    coefs.push_back(0);
}

Polynomial::Polynomial(const std::vector<double> &_v)
{
    coefs = _v;
}

Polynomial::Polynomial(const std::vector<int> &_v)
{
    for (size_t i = 0; i < _v.size(); i++)
    {
        coefs.push_back(_v[i]);
    }
}

Polynomial::Polynomial(std::initializer_list<double> init)
{
    for (auto x : init)
        coefs.push_back(x);
}

Polynomial::Polynomial(double c[], int deg)
{
    for (int i = 0; i <= deg; i++)
    {
        coefs.push_back(c[i]);
    }
}

Polynomial::Polynomial(int c[], int deg)
{
    for (int i = 0; i <= deg; i++)
    {
        coefs.push_back(c[i]);
    }
}

Polynomial::Polynomial(double x0, const std::vector<double> &_v, bool ascend)
{
    if (!ascend)
    {
        std::vector<double> temp{};
        for (int i = _v.size() - 1; i >= 0; i--)
        {
            temp.push_back(_v[i]);
        }
        *this = Polynomial(x0, temp);
    }
    else
    {
        int N = _v.size() - 1; // order of this polynomial
        Polynomial res{_v[0]};
        for (int i = 1; i <= N; ++i)
        {
            Polynomial temp{-x0, 1};
            temp = temp.power(i);
            temp *= _v[i];
            res += temp;
        }
        *this = res;
    }
}

Polynomial::Polynomial(const Polynomial &rhs)
{
    coefs = rhs.coefs;
}

//===============fundamental methods=======================

double Polynomial::operator()(double x) const
{
    double res = coefs[deg()];
    for (int i = deg() - 1; i >= 0; i--)
    {
        res = res * x + coefs[i]; // qin jiu shao algorithm
    }
    return res;
}

double Polynomial::diff(double x) const
{
    double res = 0;
    for (int i = 1; i <= deg(); i++)
    {
        res += i * coefs[i] * pow(x, i - 1);
    }
    return res;
}

double Polynomial::diff(int degree, double x) const
{
    return this->Diff(degree)(x);
}

Polynomial Polynomial::Diff() const
{
    std::vector<double> res;
    for (int i = 0; i < this->deg(); i++)
    {
        res.push_back((i + 1) * coefs[i + 1]);
    }
    if (res.size() == 0)
        res.push_back(0.0); // assure no empty coefs
    return Polynomial(res);
}

Polynomial Polynomial::Diff(int degree) const
{
    if (degree < 0)
        throw std::invalid_argument("求导次数必须大于等于0!");
    Polynomial res(*this);
    for (int i = 0; i < degree; i++)
        res = res.Diff();
    return res;
}

Polynomial Polynomial::power(unsigned int k) const // p = p^k
{
    Polynomial res{1};
    Polynomial temp = *this;
    for (size_t i = 0; i < k; i++)
    {
        res *= temp;
    }
    return res;
}


//=====================Arithmetics==============================

Polynomial &Polynomial::operator=(const Polynomial &rhs)
{
    if (this == &rhs)
        return *this;
    coefs = rhs.coefs;
    return *this;
}

Polynomial &Polynomial::operator+=(const Polynomial &rhs)
{
    int max_order = (this->deg() > rhs.deg()) ? this->deg() : rhs.deg();
    int min_order = (this->deg() <= rhs.deg()) ? this->deg() : rhs.deg();
    for (int i = 0; i <= max_order; i++)
    {
        if (i <= min_order)
            coefs[i] += rhs.coefs[i];
        else
        {
            if (this->deg() < rhs.deg())
                coefs.push_back(rhs.coefs[i]);
        }
    }
    // 处理同阶多项式高阶项出现一堆0的情形
    while (coefs.back() == 0)
        coefs.pop_back();
    if (coefs.size() == 0)
        coefs.push_back(0);
    return *this;
}

const Polynomial Polynomial::operator+(const Polynomial &rhs) const
{
    return Polynomial(*this) += rhs;
}

Polynomial &Polynomial::operator+=(double rhs)
{
    coefs[0] += rhs;
    while (coefs.back() == 0)
        coefs.pop_back();
    if (coefs.size() == 0)
        coefs.push_back(0);
    return *this;
}

const Polynomial Polynomial::operator+(double rhs) const
{
    return Polynomial(*this) += rhs;
}

Polynomial &Polynomial::operator-=(const Polynomial &rhs)
{
    int max_order = (this->deg() > rhs.deg()) ? this->deg() : rhs.deg();
    int min_order = (this->deg() <= rhs.deg()) ? this->deg() : rhs.deg();
    for (int i = 0; i <= max_order; i++)
    {
        if (i <= min_order)
            coefs[i] -= rhs.coefs[i];
        else
        {
            if (this->deg() < rhs.deg())
                coefs.push_back(-rhs.coefs[i]);
        }
    }
    // 处理同阶多项式高阶项出现一堆0的情形
    while (coefs.back() == 0)
        coefs.pop_back();
    if (coefs.size() == 0)
        coefs.push_back(0);
    return *this;
}

const Polynomial Polynomial::operator-(const Polynomial &rhs) const
{
    return Polynomial(*this) -= rhs;
}

const Polynomial Polynomial::operator-() const
{
    std::vector<double> res(coefs);
    for (size_t i = 0; i < res.size(); i++)
    {
        res[i] = -res[i];
    }
    return Polynomial(res);
}

Polynomial &Polynomial::operator-=(double rhs)
{
    coefs[0] -= rhs;
    return *this;
}
const Polynomial Polynomial::operator-(double rhs) const
{
    return Polynomial(*this) -= rhs;
}

Polynomial &Polynomial::operator*=(const Polynomial &rhs)
{
    int order = deg() + rhs.deg();
    std::vector<double> res(order + 1);
    std::vector<double> lhs_copy(coefs);
    std::vector<double> rhs_copy(rhs.coefs);
    lhs_copy.resize(order + 1);
    rhs_copy.resize(order + 1);
    for (int i = 0; i <= order; i++)
        for (int j = 0; j <= i; j++)
            res[i] += lhs_copy[j] * rhs_copy[i - j];
    coefs = res;
    while (coefs.back() == 0)
        coefs.pop_back();
    if (coefs.size() == 0)
        coefs.push_back(0);
    return *this;
}
const Polynomial Polynomial::operator*(const Polynomial &rhs) const
{
    return Polynomial(*this) *= rhs;
}

Polynomial &Polynomial::operator*=(double rhs)
{
    for (int i = 0; i < static_cast<int>(coefs.size()); i++)
        coefs[i] *= rhs;
    while (coefs.back() == 0)
        coefs.pop_back();
    if (coefs.size() == 0)
        coefs.push_back(0);
    return *this;
}
const Polynomial Polynomial::operator*(double rhs) const
{
    return Polynomial(*this) *= rhs;
}

Polynomial &Polynomial::operator/=(double rhs)
{
    if (rhs == 0)
        throw std::invalid_argument("除数不能为0！");
    for (int i = 0; i < static_cast<int>(coefs.size()); i++)
        coefs[i] /= rhs;
    return *this;
}

const Polynomial Polynomial::operator/(double rhs) const
{
    return Polynomial(*this) /= rhs;
}

// friends

const Polynomial operator+(double lhs, const Polynomial &rhs)
{
    std::vector<double> res(rhs.coefs);
    res[0] += lhs;
    return Polynomial(res);
}
const Polynomial operator-(double lhs, const Polynomial &rhs)
{
    std::vector<double> res(rhs.coefs);
    for (size_t i = 0; i < res.size(); i++)
        res[i] *= -1;
    res[0] += lhs;
    return Polynomial(res);
}
const Polynomial operator*(double lhs, const Polynomial &rhs)
{
    std::vector<double> res(rhs.coefs);
    for (int i = 0; i < static_cast<int>(res.size()); i++)
        res[i] *= lhs;
    return Polynomial(res);
}

//====================out to ostream=========================
std::ostream &operator<<(std::ostream &os, const Polynomial &P)
{
    // 阶数大于2时先输出首项
    if (P.deg() > 1)
    {
        if (P[P.deg()] < 0)
        {
            if (P[P.deg()] == -1)
                os << "- x^" << P.deg();
            else
                os << "- " << -P[P.deg()] << "x^" << P.deg();
        }
        else
        {
            if (P[P.deg()] == 1)
                os << "x^" << P.deg();
            else
                os << P[P.deg()] << "x^" << P.deg();
        }
    }

    for (int i = P.deg() - 1; i > 1; i--)
    {
        if (P[i] == 0)
            continue;
        if (P[i] < 0)
        {
            if (P[i] == -1)
                os << " - x^" << i;
            else
                os << " - " << -P[i] << "x^" << i;
        }
        else
        {
            if (P[i] == 1)
                os << " + x^" << i;
            else
                os << " + " << P[i] << "x^" << i;
        }
    }
    // 输出1阶项
    if (P[1] == 0)
        ;
    else
    {
        if (P[1] < 0)
        {
            if (P[1] == -1)
                os << "- x";
            else
                os << "- " << -P[1] << "x";
        }
        else
        {
            if (P[1] == 1)
            {

                if (P.deg() > 1)
                    os << " + x";
                else
                    os << "x";
            }
            else
            {
                if (P.deg() > 1)
                    os << " + " << P[1] << "x";
                else
                    os << P[1] << "x";
            }
        }
    }
    // 输出0阶项
    if (P[0] == 0)
    {
        if (P.deg() == 0)
            os << " 0 ";
    }
    else
    {
        if (P[0] < 0)
            os << " - " << -P[0];
        else
            os << " + " << P[0];
    }
    return os;
}

std::string Polynomial::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4); // Set output precision
    bool isFirstTerm = true;

    for (int i = coefs.size() - 1; i >= 0; --i) {
        double coef = coefs[i];
        if (coef == 0) continue; // Skip zero coefficients

        // Handle the sign
        if (isFirstTerm) {
            if (coef < 0) oss << "-";
        } else {
            oss << (coef < 0 ? " - " : " + ");
        }

        // Handle absolute value of the coefficient
        coef = std::abs(coef);
        if (i == 0 || coef != 1) oss << coef;

        // Handle the power of x
        if (i > 0) {
            oss << "x";
            if (i > 1) oss << "^" << i;
        }

        isFirstTerm = false;
    }

    if (isFirstTerm) { // Case where all coefficients are zero
        oss << "0";
    }

    return oss.str();
}
