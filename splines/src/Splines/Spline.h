/**
 * @file Spline.h
 * @author Shaozhen (2458754434@qq.com)
 * @brief Spline project
 * @version 0.1
 * @date 2022-11-06
 * @details cubic splines interpolation
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SPLINE_H
#define SPLINE_H

#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "ba_obj/structure.h"
#include "splines_common.h"
#include "BasicHeadFile/Polynomial.h"
#include "BasicHeadFile/Point.h"
#include "Base_Spline.h"
#include <vector>
#if defined _WIN32
#include "openblas/lapacke.h"
#else
#include "lapacke.h"
#endif
using std::vector;

// 对向量x,y按照x的单增顺序同时进行排列
void doubleSort(std::vector<double>& x, std::vector<double>& y);

//列举五大样条边值条件，分别为periodic,complete,specified_2nd,natural,not_a_knot
enum class BCType
{
    periodic,
    complete,
    specified_2nd,
    natural,
    not_a_knot
};

//初始条件结构体，分别为插值点、插值点的值，左右边值的条件与值
struct InterpCondition
{
    std::vector<double> sites = std::vector<double>{};
    std::vector<double> function_values = std::vector<double>{};
    double derivative1 = 0.0;
    double derivative2 = 0.0;
    BCType left;
    BCType right;
};

//列举样条类型，有ppForm和Bspline两类
enum class SplineType
{
    ppForm,
    BSpline
};

// the class Spline声明
template <int Order, SplineType t>
class Spline;

// explicit specializations of order-1 and order-3 splines

//===================================================================
//                        ppForm Spline
//===================================================================

//一阶ppForm类型样条
template <>
class Spline<1, SplineType::ppForm>
{
private:
    vector<double> knots;//节点N
    vector<double> values;//值

public:
    //缺省构造
    Spline() = default;
    //构造函数，输入为初始值结构体
    Spline(const InterpCondition &);

public:
    //重构括号，返回样条在点x的值，在定义域外为分片多项式的自然延拓
    double operator()(double x) const;
    //返回节点的size
    int size() const { return knots.size(); }
    //返回第i个节点的坐标，i从1开始计数
    double knot(int i) const { return knots[i - 1]; }
    //返回[ t_i, t_ {i+1} ]处的多项式，i为1到N-1
    const Polynomial poly(int i) const;
};

//===================================================================

//三阶ppForm类型样条
template <>
class Spline<3, SplineType::ppForm>
{
private:
    vector<double> knots;//节点
    vector<Polynomial> pp;//分片多项式

//类内部实现所需的函数，对外无接口
private:
    //构造样条所需参数
    inline double mu(int i) const;
    //构造样条所需参数
    inline double lambda(int i) const;
    //初始化线性方程组的公共部分，即初始化A和rhs第2至N-1行
    void initialize_common_part(vector<vector<double>>&A,vector<double>&rhs,
                                const std::vector<double> &) const;
                                //Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                //const std::vector<double> &) const;
    //用二阶导数向量M构造每一段上的分片多项式
    void assign_polys_with_M(const vector<double>&M,const std::vector<double> &values);
        //const Eigen::VectorXd &M, const std::vector<double> &values);
    //完全样条
    void complete(bool left, vector<vector<double>>&A,vector<double> &rhs,
                    const std::vector<double> &values, double);
                //(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                //  const std::vector<double> &values, double);
    //二阶样条
    void specified_2nd(bool left, vector<vector<double>>&A,vector<double> &rhs,double);
        //(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,double);
    //自然样条
    void natural(bool left, vector<vector<double>>&A,vector<double> &rhs);
    //(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
    //非节点样条
    void not_a_knot(bool left, vector<vector<double>>&A,vector<double> &rhs);
    //(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
    //周期样条
    void periodic(vector<vector<double>>&A,vector<double> &rhs,
                    const std::vector<double> &values);
    //(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
     //             const std::vector<double> &values);
public:
    //缺省构造
    Spline() = default;
    //构造函数，输入为初始值结构体
    Spline(const InterpCondition &);
    //构造函数，输入为北太天元结构体
    Spline(const baltam::structure &);

    /// @brief 返回样条在x的值，在定义域外为邻边界分片多项式的自然延拓
    /// @param x
    /// @return s(x)
    double operator()(double x) const;

public:
    /// @brief 返回  [ t_i, t_ {i+1} ]处的多项式
    /// @param i 的范围为 [ 1, N-1 ], N 为节点数量
    /// @return p_i(x)
    const Polynomial &poly(int i) const;

    /// @brief 返回第i个节点
    /// @param 第i个节点, 1 <= i <= N
    /// @return t_i
    inline double knot(int i) const;

    /// @brief 返回一个包含所有节点的vector
    /// @return std::vector<double>
    const std::vector<double> &getKnots() const;

    /// @brief 返回节点数量
    /// @return int
    int size() const { return knots.size(); }

public:

   
    baltam::structure* spline_to_baltam_structure()  const;//将ppForm样条形式转化为北太天元结构体形式
};

//===================================================================
//                          Base Spline
//===================================================================
/*
//一阶Bspline形式样条
template <>
class Spline<1, SplineType::BSpline>
{
private:
    vector<double> knots; //节点
    vector<double> coefs; //coefficients of each base spline function
public:

    Spline() = default;//缺省构造
    Spline(const InterpCondition &c);//构造函数，输入为初始值结构体

    double operator()(double x) const;//返回样条在x的值
};

//===================================================================

//三阶Bspline形式样条

template <>
class Spline<3, SplineType::BSpline>
{
private:
    vector<double> knots;//节点，从1到N
    vector<double> coefs;//基函数前的系数,从-1到N
//类内部实现所需函数
private:
    void init_matrix(Eigen::MatrixXd &A, Eigen::VectorXd &rhs, const std::vector<double> &values);
    //完全样条
    void ctor_complete(const std::vector<double> &, const std::vector<double> &, double, double);
    //二阶样条
    void ctor_specified_2nd(const std::vector<double> &, const std::vector<double> &, double, double);
    //自然样条
    void ctor_natural(const std::vector<double> &, const std::vector<double> &);
    //非节点样条
    void ctor_not_a_knot(const std::vector<double> &, const std::vector<double> &);
    //周期样条
    void ctor_periodic(const std::vector<double> &, const std::vector<double> &);

    /// @brief return coefs with index i, the index is same as in the notes
    /// @param i index -1 <= i <= N
    /// @return coef_i
    double coef(int i) const;

public:
    Spline() = default;//缺省构造
    Spline(const InterpCondition &);//构造函数，输入为初始值结构体

    /// @brief return value of s(x) at point x, s doesn't know its value out of its defined interval
    /// @param x
    /// @return s(x)
    double operator()(double x) const;

public:
    /// @brief 返回 [ t_i, t_ {i+1} ]处的多项式
    /// @param i in [ 0, N-1 ], N为节点数
    /// @return p_i(x)
    const Polynomial poly(int i) const;//未实现

    /// @brief 返回第i个节点, BSpline's ghost knots included with the same index in the notes
    /// @param i index of the required knot, -2 <= i <= N+3
    /// @return t_i
    double knot(int i) const;

    /// @brief return a vector consisting of all the knots
    /// @return std::vector<double>
    const std::vector<double> &getKnots() const { return knots; }

    /// @brief return number of knots
    /// @return int
    int size() const { return knots.size(); }
};

//===================================================================
//二阶Bspline形式样条
//给定插值点 t_i, i = 1, 2, . . . , N ;s(x_i) = f (x_i), i =1, 2, . . . , N −1，其中 x_i = (t_i + t_{i+1})/2
template <>
class Spline<2, SplineType::BSpline>
{
private:
    std::vector<double> knots;//节点，从1至N
    std::vector<double> coefs;//系数，从0至N

    /// @brief return coefs with index i, the index is same as in the notes
    /// @param i index -1 <= i <= N
    /// @return coef_i
    double coef(int i) const;

public:

    /// @brief return value of s(x) at point x, s doesn't know its value out of its defined interval
    /// @param x
    /// @return s(x)
    double operator()(double x) const;

public:
    /// @brief return the polynomial at [ t_i, t_ {i+1} ]
    /// @param i in [ 0, N-1 ], N is  number of knots
    /// @return p_i(x)
    const Polynomial poly(int i) const;

    /// @brief return i'th knot, BSpline's ghost knots included with the same index in the notes
    /// @param i index of the required knot,1 - Order <= i <= N + Order
    /// @return t_i
    double knot(int i) const;

    /// @brief return a vector consisting of all the knots
    /// @return std::vector<double>
    const std::vector<double> &getKnots() const { return knots; }

    /// @brief return number of knots
    /// @return int
    int size() const { return knots.size(); }
};
*/
#else
// do nothing
#endif
