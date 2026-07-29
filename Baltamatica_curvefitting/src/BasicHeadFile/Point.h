#ifndef POINT_H
#define POINT_H

#include <vector>
#include <initializer_list>
#include <cmath>
#include <iostream>
#include <cassert>

/**
 * @brief define a point in the Euclidean space,
 *        the point knows its length (dimension)
 */
class Point
{
private:
    std::vector<double> coordinate;
    int Dim;

public:
    Point() = default;

    Point(const std::vector<double> &v)
        : coordinate(v), Dim(v.size())
    {
    }
    Point(const std::vector<int> &v);
    Point(std::initializer_list<double> l);
    Point(std::initializer_list<int> l);

    Point &operator+=(const Point &rhs)
    {
        assert(Dim == rhs.dim());
        for (int i = 0; i < Dim; i++)
            coordinate[i] += rhs.coordinate[i];
        return *this;
    }

    const Point operator+(const Point &rhs) const
    {
        return Point(*this) += rhs;
    }

    Point &operator-=(const Point &rhs)
    {
        assert(Dim == rhs.dim());
        for (int i = 0; i < Dim; i++)
            coordinate[i] -= rhs.coordinate[i];
        return *this;
    }

    const Point operator-(const Point &rhs) const
    {
        return Point(*this) -= rhs;
    }

    double &operator[](int x) { return coordinate[x]; }
    double operator[](int x) const { return coordinate[x]; }
    int dim() const { return Dim; }
};

// length of the vector starting at the origin and ending at this point
inline double norm(const Point &p)
{
    double res = 0.0;
    for (int i = 0; i < p.dim(); i++)
        res += p[i] * p[i];
    return sqrt(res);
}

inline std::ostream &operator<<(std::ostream &os, const Point &p)
{
    os.precision(8);
    os << "[" << p[0];
    for (int d = 1; d < p.dim(); d++)
        os << ", " << p[d];
    os << "]";
    return os;
}

inline Point::Point(const std::vector<int> &v) : coordinate(), Dim(v.size())
{
    for (int i = 0; i < Dim; i++)
        coordinate.push_back(v[i]);
}

inline Point::Point(std::initializer_list<double> l) : coordinate(), Dim()
{
    auto j = l.begin();
    for (size_t d = 0; d < l.size(); ++d)
        coordinate.push_back(*j++);
    Dim = coordinate.size();
}

inline Point::Point(std::initializer_list<int> l) : coordinate(), Dim()
{
    auto j = l.begin();
    for (size_t d = 0; d < l.size(); ++d)
        coordinate.push_back(*j++);
    Dim = coordinate.size();
}

#else
 // do nothing
#endif
