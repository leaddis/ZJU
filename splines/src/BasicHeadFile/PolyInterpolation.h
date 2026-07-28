#ifndef POLYINTERPOLATION_H
#define POLYINTERPOLATION_H

#include "Polynomial.h"
#include <limits>
#include <initializer_list>
const static double EPS = std::numeric_limits<double>::epsilon();

/**
 * @brief define what is polynomial interpolation(an abstract base class)
 * @details
 * 1. class "PolyInterpolation" is an abstract base class;
 * 2. It specifies what polynomial interpolation is;
 * 3. each polynomial interpolation method is inherited from it.
 */
class PolyInterpolation
{
protected:
    Polynomial interPoly; ///< stores the interpolation polynomial

    /**
     * each interpolation method should define their
     * polynomial generating method of their own.
     */
    virtual void generatePoly() = 0;

public:
    /**
     * @brief construct a new default Poly Interpolation object
     * @details set the interPloy 0
     */
    PolyInterpolation() : interPoly() {}

    /**
     * @brief return the value of interpolation polynomial at given point x;
     * @param x a given value at which the interpolation value will be returned
     * @return the value of the interpolation polynomial at x
     */
    virtual double operator()(double x) const { return interPoly(x); }

    /**
     * @brief show the interpolation polynomial
     * @details each interpolation method has this interface and should not override it.
     *
     */
    void showPoly() const { std::cout << interPoly; }

    /**
     * @brief get the interpolation polynomial
     *
     * @return interpolation Polynomial
     */
    Polynomial getPoly() const { return interPoly; }
};

/**
 * @brief a helper class for Hermite interpolation
 * @details
 * 1. Class "InterpolationInfo" is a helper class for Hermite Interpolation;
 * 2. It stores the interpolation information at one single site, i,e. values of derivatives of each degree;
 * 3. It knows its highest degree m_i;
 * 4. A vector<InterpolationInfo> is used as the only legal input of Hermite interpolation
 */
class InterpolationInfo;

/**
 * @brief Hermite interpolation
 *
 */
class HermiteInterpolation : public PolyInterpolation
{
protected:
    std::vector<std::vector<double>> tableOfDividedDiff; ///< the divided difference table

    /**
     * @brief generate the divided difference table
     *
     * @param site a vector of coordinated of the sites
     * @param InterCondition a vector of derivatives info at each site
     */
    void generateTable(const std::vector<double> &site,
                       const std::vector<InterpolationInfo> &InterCondition);

    /**
     * @brief generate the interpolation polynomial based on the divided difference table
     *
     */
    void generatePoly();

public:
    /**
     * @brief Construct a new default Hermite Interpolation object
     *
     */
    HermiteInterpolation() : PolyInterpolation(), tableOfDividedDiff() {}

    /**
     * @brief Construct a new Hermite Interpolation object
     *
     * @param site a vector of coordinated of the sites
     * @param InterCondition a vector of derivatives info at each site
     */
    HermiteInterpolation(const std::vector<double> &site,
                         const std::vector<InterpolationInfo> &InterCondition);


    /**
     * @brief return the derivative of the interpolation polynomial at x
     *
     * @param x a given point
     * @return derivative of the interpolation polynomial at x
     */
    double diff(double x) const { return interPoly.diff(x); }
};

//------------------------------------------------------------------------------

/**
 * @brief Newton interpolation
 *
 */
class NewtonInterpolation : public HermiteInterpolation
{
private:

    /**
     * @brief generate the divided difference table
     *
     * @param sites a vector of coordinates of the sites
     * @param values a vector of value at each site
     */
    void generateTable(const std::vector<double> &sites,
                       const std::vector<double> &values);

public:
    /**
     * @brief Construct a new Newton Interpolation object
     *
     * @param sites a vector of coordinates of the sites
     * @param values a vector of value at each site
     */
    NewtonInterpolation(const std::vector<double> &sites,
                        const std::vector<double> &values);
};
//-----------------------------------------------------------------------------

/**
 * @brief a helper class which stores the derivative of one site
 * @details used by Hermite interpolation as a constructor parameter
 *
 */
class InterpolationInfo
{
private:
    std::vector<double> info; ///< derivatives of different degree

public:
    /**
     * @brief Construct a new default Interpolation Info object
     *
     */
    InterpolationInfo() : info(std::vector<double>()) {}

    /**
     * @brief Construct a new Interpolation Info object
     *
     * @param init initializer list of double
     */
    InterpolationInfo(std::initializer_list<double> init);

    /**
     * @brief Construct a new Interpolation Info object
     *
     * @param _v a vector of derivatives of different degree
     */
    InterpolationInfo(const std::vector<double> &_v) { info = _v; }

    int size() const { return info.size(); }

    /**
     * @brief return k'th derivative of this site
     *
     * @param k degree
     * @return k'th derivative
     */
    double operator[](int k) const { return info[k]; }

    /**
     * @brief show the derivative info
     *
     */
    void showInfo() const;
};

/**
 * @brief
 *
 * @param _v
 * @param x
 * @return * find
 */
int find_index(const std::vector<double> &_v, double x);

/**
 * @brief
 *
 * @param n
 * @return int
 */
int factorial(int n);

//***********************************something weird******************************

/**
 * @brief  Lagrange interpolation
 *
 */
class LagrangeInterpolation : public PolyInterpolation
{
private:
    std::vector<double> _sites;
    std::vector<double> _coefs;
    void generatePoly();

public:

    /**
     * @brief Construct a new Lagrange Interpolation object
     *
     * @param sites a vector of coordinates of the sites
     * @param values a vector of value at each site
     */
    LagrangeInterpolation(const std::vector<double> &sites,
                          const std::vector<double> &values);

    /**
     * @brief can be implemented via the gravity formula, but left to the future
     *
     * @param x a given value at which the interpolation value will be returned
     * @return the value of the interpolation polynomial at x
     */
    double operator()(double x) const;
};
#else
//...
#endif