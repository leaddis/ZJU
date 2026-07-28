#include "PolyInterpolation.h"

//**************************************************************************
//--------------------Hermite interpolation---------------------------------
//**************************************************************************

/**
 * @brief generate the divided difference table
 *
 * @param site interpolation sites vector
 * @param InterCondition a vector of derivatives at each site
 */
void HermiteInterpolation::generateTable(const std::vector<double> &site,
                                         const std::vector<InterpolationInfo> &InterCondition)
{
    if (site.size() != InterCondition.size())
        throw std::invalid_argument{"插值节点向量和坐标点向量的大小必须相等！"};
    // N = deg(P(x)) + 1
    int N = site.size();
    std::vector<int> m; // record the max derivative degree of each site
    for (const auto &info : InterCondition)
    {
        N += info.size() - 1;
        m.push_back(info.size());
    }
    // set appropriate size
    tableOfDividedDiff.resize(N);
    for (auto &rows : tableOfDividedDiff)
        rows.resize(N + 1);
    // initialize the first two columns
    int i = 0; // i is the row index
    int X = 0; // the X'th site
    while (i < N)
    {
        for (int q : m)
        {
            // repeat q = m_X times for each site
            for (int k = 0; k < q; k++)
            {
                tableOfDividedDiff[i][0] = site[X];
                tableOfDividedDiff[i][1] = InterCondition[X][0];
                i++;
            }
            X++; // next site
        }
    }
    // compute other values in the table
    // i is the row index
    // j is the column index
    for (std::vector<double>::size_type j = 2; j < tableOfDividedDiff[0].size() + 1; j++)
    {
        for (auto i = j - 1; i < tableOfDividedDiff.size(); i++)
        {
            double denominator = tableOfDividedDiff[i][0] - tableOfDividedDiff[i + 1 - j][0];
            if (fabs(denominator) <= EPS) // multiple site
            {
                int index = find_index(site, tableOfDividedDiff[i][0]);
                tableOfDividedDiff[i][j] = InterCondition[index][j - 1] / (double)factorial(j - 1);
            }
            else
                tableOfDividedDiff[i][j] =
                    (tableOfDividedDiff[i][j - 1] - tableOfDividedDiff[i - 1][j - 1]) / denominator;
        }
    }
    return;
}

void HermiteInterpolation::generatePoly()
{
    Polynomial res;
    Polynomial tmp(std::vector<double>{1});
    for (size_t i = 0; i < tableOfDividedDiff.size(); i++)
    {
        res = res + tableOfDividedDiff[i][i + 1] * tmp;
        // std::cout << res << std::endl;
        Polynomial step(std::vector<double>{-tableOfDividedDiff[i][0], 1});
        tmp = (tmp * step);
    }
    interPoly = res;
}

HermiteInterpolation::HermiteInterpolation(const std::vector<double> &site,
                                           const std::vector<InterpolationInfo> &InterCondition)
{
    generateTable(site, InterCondition);
    generatePoly();
}


//**************************************************************************
//--------------------InterpolationInfo-------------------------------------
//**************************************************************************

InterpolationInfo::InterpolationInfo(std::initializer_list<double> init)
{
    for (auto x : init)
        info.push_back(x);
}

void InterpolationInfo::showInfo() const
{
    for (auto x : info)
        std::cout << x << " ";
}

int find_index(const std::vector<double> &_v, double x)
{
    for (std::vector<double>::size_type i = 0; i < _v.size(); i++)
    {
        if (fabs(_v[i] - x) <= EPS)
            return i;
    }
    return -1;
}

int factorial(int n)
{
    if (n < 0)
        throw std::invalid_argument{"只能对非负整数计算阶乘！"};
    if (n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}

//*********************************************************************************
//------------------------------Newton Interpolation-------------------------------
//*********************************************************************************


void NewtonInterpolation::generateTable(const std::vector<double> &sites,
                                        const std::vector<double> &values)
{
    // set appropriate size
    tableOfDividedDiff.resize(sites.size());
    for (auto &rows : tableOfDividedDiff)
        rows.resize(sites.size() + 1);
    // initialize the first two columns
    for (size_t i = 0; i < tableOfDividedDiff.size(); i++)
    {
        tableOfDividedDiff[i][0] = sites[i];
        tableOfDividedDiff[i][1] = values[i];
    }
    // compute the divided difference
    // i is the row index
    // j is the column index
    for (size_t j = 2; j < sites.size() + 1; j++)
    {
        for (size_t i = j - 1; i < sites.size(); i++)
        {
            tableOfDividedDiff[i][j] =
                (tableOfDividedDiff[i][j - 1] - tableOfDividedDiff[i - 1][j - 1]) /
                (tableOfDividedDiff[i][0] - tableOfDividedDiff[i + 1 - j][0]);
        }
    }
    return;
}

//================= public interfaces=========================================


NewtonInterpolation::NewtonInterpolation(const std::vector<double> &sites,
                                         const std::vector<double> &values)
{
    if (sites.size() != values.size())
        throw std::invalid_argument{"插值节点向量和坐标点向量的大小必须相等！"};
    generateTable(sites, values);
    generatePoly();
}

//*********************************************************************************
//----------------------------Lagrange Interpolation-------------------------------
//*********************************************************************************

LagrangeInterpolation::LagrangeInterpolation(const std::vector<double> &sites,
                                             const std::vector<double> &values)
    : _sites(sites), _coefs(values)
{
    if (sites.size() != values.size())
        throw std::invalid_argument{"插值节点向量和坐标点向量的大小必须相等！"};
    generatePoly();
}


double LagrangeInterpolation::operator()(double x) const
{
    return interPoly(x);
}

void LagrangeInterpolation::generatePoly()
{
    Polynomial res;
    for (size_t i = 0; i < _sites.size(); i++)
    {
        Polynomial base{1};
        double denominator = 1.0;
        for (std::vector<double>::size_type j = 0; j < i; j++)
        {
            Polynomial inter{-_sites[j], 1.0};
            base = base * inter;
            denominator *= _sites[i] - _sites[j];
        }
        for (std::vector<double>::size_type j = i + 1; j < _sites.size(); j++)
        {
            Polynomial inter{-_sites[j], 1.0};
            base = base * inter;
            denominator *= _sites[i] - _sites[j];
        }
        base = (_coefs[i] / denominator) * base;
        res = res + base;
    }
    interPoly = res;
}