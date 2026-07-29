#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <initializer_list>
#include <cassert>
#include <algorithm>

/**
 * @brief scalar polynomial
 * @details
 * p(x) = a_0 + a_1*x + ... + a_n*x^n
 */
class Polynomial
{
private:
    // int degree;                ///< poly degree n
    std::vector<double> coefs; ///< coefficients, begin with a_0

public:
    //----------------constructors-----------------------

    Polynomial();
    Polynomial(const std::vector<double> &_v);
    Polynomial(const std::vector<int> &_v);
    Polynomial(std::initializer_list<double> init);
    Polynomial(double c[], int deg);
    Polynomial(int c[], int deg);
    Polynomial(double x0,const std::vector<double> & _v,bool ascend = true);
    Polynomial(const Polynomial &rhs);

    //-------------fundamental methods---------------------

    int deg() const { return coefs.size() - 1; }
    double operator[](int k) const { return coefs[k]; }
    double &operator[](int k) { return coefs[k]; }
    double operator()(double x) const;
    double diff(double x) const;
    double diff(int degree, double x) const;
    Polynomial Diff() const;
    Polynomial Diff(int) const;
    Polynomial Integrate() const;
    Polynomial Integrate(int) const;
    Polynomial power(unsigned int) const;

    // -----------reformulation----------------

    const Polynomial reformulate(double x0) const;

    //------------Arithmetics-----------------

    Polynomial &operator=(const Polynomial &rhs);

    Polynomial &operator+=(const Polynomial &rhs);
    Polynomial &operator+=(double rhs);
    const Polynomial operator+(const Polynomial &rhs) const;
    const Polynomial operator+(double rhs) const;

    Polynomial &operator-=(const Polynomial &rhs);
    Polynomial &operator-=(double rhs);
    const Polynomial operator-() const;
    const Polynomial operator-(const Polynomial &rhs) const;
    const Polynomial operator-(double rhs) const;

    Polynomial &operator*=(const Polynomial &rhs);
    Polynomial &operator*=(double rhs);
    const Polynomial operator*(double rhs) const;
    const Polynomial operator*(const Polynomial &rhs) const;

    Polynomial &operator/=(double rhs);
    const Polynomial operator/(double rhs) const;

    friend const Polynomial operator-(double lhs, const Polynomial &rhs);
    friend const Polynomial operator+(double lhs, const Polynomial &rhs);
    friend const Polynomial operator*(double lhs, const Polynomial &rhs);

    //----------out to ostream-----------------
    /**
     * @brief print a polynomial via ostream
     *
     * @param os the ostream object
     * @param P the polynomial to be printed
     * @return std::ostream&
     */
    friend std::ostream &operator<<(std::ostream &os, const Polynomial &P);
};

#else
// do nothing
#endif