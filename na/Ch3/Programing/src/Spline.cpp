#include "Spline.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>


//===================================================================================
//           Implementation of 1st-Order Piecewise Polynomial Spline
//===================================================================================

// 构造函数
Spline<1, SplineType::ppForm>::Spline(const InterpCondition& c)
    : knots(c.sites), values(c.function_values)
{
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    doubleSort(knots, values); // 按照knots单增的顺序排序
    fit(knots, values);        // 拟合样条
}

// 对向量x,y按照x的单增顺序同时进行排列
void Spline<1, SplineType::ppForm>::doubleSort(std::vector<double> &x, std::vector<double> &y)
{
    if (x.size() == y.size())
    {
        int size = x.size();
        for (int i = 0; i < size - 1; i++)
        {
            for (int j = i + 1; j < size; j++) // 修正内层循环j的起始值为i+1
            {
                if (x[i] > x[j])
                {
                    std::swap(x[i], x[j]);
                    std::swap(y[i], y[j]);
                }
            }
        }
    }
}

// 拟合方法
void Spline<1, SplineType::ppForm>::fit(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        throw std::invalid_argument("输入向量x和y必须具有相同的大小且至少包含2个点。");
    }

    knots = x;
    values = y;

    int n = knots.size();
    a.resize(n - 1);
    b.resize(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        a[i] = values[i];
        b[i] = (values[i + 1] - values[i]) / (knots[i + 1] - knots[i]);
    }
}

// 重载()运算符用于评估
double Spline<1, SplineType::ppForm>::operator()(double x_val) const {
    int n = knots.size();
    if (n < 2) {
        throw std::runtime_error("样条尚未拟合。");
    }

    // 找到x_val所在的区间
    int i = 0;
    while (i < n - 2 && x_val >= knots[i + 1]) {
        ++i;
    }

    // 线性插值
    return a[i] + b[i] * (x_val - knots[i]);
}

//===================================================================================
//           Implementation of 3rd-Order Piecewise Polynomial Spline
//===================================================================================

inline double Spline<3, SplineType::ppForm>::mu(int i) const
{
    if (i <= 0 || i >= static_cast<int>(knots.size()))
        throw std::invalid_argument{"样条内部实现错误：输入索引在节点标号之外。"};
    return (knot(i) - knot(i - 1)) / (knot(i + 1) - knot(i - 1));
}

inline double Spline<3, SplineType::ppForm>::lambda(int i) const
{
    return 1 - mu(i);
}

void Spline<3, SplineType::ppForm>::initialize_common_part(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                                           const std::vector<double> &values) const
{
    int N = knots.size();
    for (int i = 1; i < N - 1; i++)
    {
        A(i, i) = 2;
        A(i, i - 1) = mu(i + 1);
        A(i, i + 1) = lambda(i + 1);
        rhs(i) = 6.0 * ((values[i + 1] - values[i]) / (knots[i + 1] - knots[i]) -(values[i] - values[i - 1]) / (knots[i] - knots[i - 1])) /
                        (knots[i + 1] - knots[i - 1]);
    }
}

void Spline<3, SplineType::ppForm>::assign_polys_with_M(const Eigen::VectorXd &M, const std::vector<double> &values)
{
    int N = knots.size();
    // 将M赋值给分片多项式
    for (int i = 0; i < N - 1; i++)
    {
        Polynomial res{values[i]};                                          // res = f_i
        Polynomial p{-knots[i], 1.0};                                       // p(x) = x - x_i
        double K = (values[i + 1] - values[i]) / (knots[i + 1] - knots[i]); // K = f[x_i,x_{i+1}]
        double c_1 = K - (2 * M(i) + M(i + 1)) * (knots[i + 1] - knots[i]) / 6.0;
        res += c_1 * p;              // res = f_i + c_1 (x - x_i)
        res += (M(i) / 2.0) * p * p; // res = f_i + c_1 (x - x_i) + M_i /2 *(x - x_i)^2
        double c_3 = (M(i + 1) - M(i)) / (knots[i + 1] - knots[i]) / 6.0;
        res += c_3 * p * p * p; // res = f_i + c_1 (x - x_i) + M_i /2 *(x - x_i)^2 + c_3(x - x_i)^3
        pp.push_back(res);
    }
}

Spline<3, SplineType::ppForm>::Spline(const InterpCondition &c)
{
    check_input(c);
    knots = c.sites;
    int N = knots.size();
    // 系数矩阵
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N, N);
    // 线性方程组的右端向量
    Eigen::VectorXd rhs = Eigen::VectorXd::Zero(N);
    // 初始化公共向量
    initialize_common_part(A, rhs, c.function_values);

    // 判断左边值类型并初始化A、rhs第一行
    switch (c.left)
    {
    case BCType::complete:
        complete(true, A, rhs, c.function_values, c.derivative1);
        break;
    case BCType::specified_2nd:
        specified_2nd(true, A, rhs, c.derivative1);
        break;
    case BCType::natural:
        natural(true, A, rhs);
        break;
    case BCType::not_a_knot:
        not_a_knot(true, A, rhs);
        break;
    case BCType::periodic:
        if (c.right != BCType::periodic)
            throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
        periodic(A, rhs, c.function_values);
        break;
    default:
        throw std::invalid_argument("未定义的边界条件类型！");
        break;
    }

    // 判断右边值类型并初始化A、rhs最后一行
    switch (c.right)
    {
    case BCType::complete:
        complete(false, A, rhs, c.function_values, c.derivative2);
        break;
    case BCType::specified_2nd:
        specified_2nd(false, A, rhs, c.derivative2);
        break;
    case BCType::natural:
        natural(false, A, rhs);
        break;
    case BCType::not_a_knot:
        not_a_knot(false, A, rhs);
        break;
    case BCType::periodic:
        if (c.left != BCType::periodic)
            throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
        periodic(A, rhs, c.function_values);
        break;
    default:
        throw std::invalid_argument("未定义的边界条件类型！");
        break;
    }

    // 使用Eigen求解线性方程组
    Eigen::VectorXd M = A.fullPivLu().solve(rhs);
    assign_polys_with_M(M, c.function_values);
}


// 5 kinds of splines
void Spline<3, SplineType::ppForm>::complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                             const std::vector<double> &values, double d)
{
    if (left)
    {
        A(0, 0) = -(knot(2) - knot(1)) * 2;
        A(0, 1) = -(knot(2) - knot(1)) ;
        rhs(0) = 6*(d - (values[1] - values[0]) / (knot(2) - knot(1)));
    }
    else
    {
        int N = knots.size();
        A(N - 1, N - 2) = (knot(N) - knot(N - 1));
        A(N - 1, N - 1) = (knot(N) - knot(N - 1)) * 2;
        rhs(N - 1) = 6 * (d - (values[N - 1] - values[N - 2]) / (knot(N) - knot(N - 1)));
    }
}

void Spline<3, SplineType::ppForm>::specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double d2)
{
    if (left)
    {
        A(0, 0) = 1;
        rhs(0) = d2;
    }
    else
    {
        int N = knots.size();
        A(N - 1, N - 1) = 1;
        rhs(N - 1) = d2;
    }
}

void Spline<3, SplineType::ppForm>::natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    specified_2nd(left, A, rhs, 0);
}

void Spline<3, SplineType::ppForm>::not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    if (left)
    {
        A(0, 0) = -lambda(2);
        A(0, 1) = 1;
        A(0, 2) = -mu(2);
        rhs(0) = 0;
    }
    else
    {
        int N = knots.size();
        A(N - 1, N - 3) = -lambda(N - 1);
        A(N - 1, N - 2) = 1;
        A(N - 1, N - 1) = -mu(N - 1);
        rhs(N - 1) = 0;
    }
}

void Spline<3, SplineType::ppForm>::periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                             const std::vector<double> &values)
{
    int N = knots.size();
    A(0, 0) = 2.0;
    A(0, N - 2) = (knot(N) - knot(N - 1)) / (knot(2) - knot(1) + knot(N) - knot(N - 1));
    A(0, 1) = 1 - A(0, N - 2);
    A(N - 1, 0) = 1.0;
    A(N - 1, N - 1) = -1.0;
    rhs(0) = (values[1] - values[0]) / (knots[1] - knots[0]);
    rhs(0) -= (values[N - 1] - values[N - 2]) / (knots[N - 1] - knots[N - 2]);
    rhs(0) *= 6.0;
    rhs(0) /= knots[1] - knots[0] + knots[N - 1] - knots[N - 2];
    rhs(N - 1) = 0;
}
//**************************************************

double Spline<3, SplineType::ppForm>::operator()(double x) const
{
    if (!(x >= knots.front() && x <= knots.back()))
        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    for (size_t i = 1; i < knots.size(); i++)
    {
        if (x <= knots[i])
            return pp[i - 1](x);
    }
    return pp[pp.size() - 1](x);
}

const Polynomial &Spline<3, SplineType::ppForm>::poly(int x) const
{
    if (x < 1 || x > static_cast<int>(pp.size()))
        throw std::invalid_argument("索引越界！没有与输入索引对应的分片多项式。");
    return pp[x - 1];
}

inline double Spline<3, SplineType::ppForm>::knot(int x) const
{
    if (x < 1 || x > static_cast<int>(knots.size()))
        throw std::invalid_argument("索引越界！没有与输入索引对应的结点。");
    return knots[x - 1];
}

const std::vector<double> &Spline<3, SplineType::ppForm>::getKnots() const
{
    return knots;
}

const std::vector<Polynomial> &Spline<3, SplineType::ppForm>::getPolys() const
{
    return pp;
}

void Spline<3, SplineType::ppForm>::printPolys() const {
    if (pp.empty()) {
        std::cout << "No piecewise polynomials available. Please fit the spline first." << std::endl;
        return;
    }

    std::cout << "Piecewise Polynomials:" << std::endl;
    for (size_t i = 0; i < pp.size(); ++i) {
        std::cout << "Interval [" << knots[i] << ", " << knots[i + 1] << "): ";
        std::cout << pp[i].toString() << std::endl;
    }
}

void Spline<3, SplineType::ppForm>::check_input(const InterpCondition &c) const
{
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    
    // 检查是否按升序排列
    for (size_t i = 1; i < c.sites.size(); i++)
    {
        if (c.sites[i] < c.sites[i - 1])
            throw std::invalid_argument("样条插值输入坐标向量不是按升序排列的！");
    }

    // 检查knot是否重复
    std::vector<double> temp = c.sites;
    std::sort(temp.begin(), temp.end());
    for (size_t i = 1; i < temp.size(); i++)
    {
        if (temp[i] == temp[i - 1])
            throw std::invalid_argument("样条插值输入坐标向量中存在重复值！");
    }

} 



//==========================================================================================================================
//                                                      B-spline
//==========================================================================================================================

Spline<3,SplineType::B_spline>::Spline(const InterpCondition &cond){
    int M = cond.sites.size();
        if (M <= 2)
            throw std::invalid_argument("B样条的节点数必须大于2！");

        check(cond);
        knots = cond.sites;
        generate_base_bspline(cond.sites);
        int N = B.size();
        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N, N);
        Eigen::VectorXd rhs = Eigen::VectorXd::Zero(N);
        initialize_common_A(A, rhs, B, cond.sites, cond.function_values);
        // 判断左边值类型并初始化A、rhs第一行
        switch (cond.left) {
            case BCType::complete:
                complete(true, A, rhs, cond.function_values, cond.derivative1);
                break;
            case BCType::specified_2nd:
                specified_2nd(true, A, rhs, cond.derivative1);
                break;
            case BCType::natural:
                natural(true, A, rhs);
                break;
            case BCType::not_a_knot:
                not_a_knot(true, A, rhs);
                break;
            case BCType::periodic:
                if (cond.right != BCType::periodic)
                    throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
                periodic(A, rhs, cond.sites ,cond.function_values);
                break;
            default:
                throw std::invalid_argument("未定义的边界条件类型！");
                break;
        }

        // 判断右边值类型并初始化A、rhs最后一行
        switch (cond.right) {
            case BCType::complete:
                complete(false, A, rhs, cond.function_values, cond.derivative2);
                break;
            case BCType::specified_2nd:
                specified_2nd(false, A, rhs, cond.derivative2);
                break;
            case BCType::natural:
                natural(false, A, rhs);
                break;
            case BCType::not_a_knot:
                not_a_knot(false, A, rhs);
                break;
            case BCType::periodic:
                if (cond.left != BCType::periodic)
                    throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
                // periodic(A, rhs, cond.sites ,cond.function_values);
                break;
            default:
                throw std::invalid_argument("未定义的边界条件类型！");
                break;
        }

        // 使用Eigen求解线性方程组
        Eigen::VectorXd control_points = A.fullPivLu().solve(rhs);
        assign_polys_with_control_points(control_points, B);

};

double Spline<3,SplineType::B_spline>::operator()(double x) const{
        if (!(x >= knots.front() && x <= knots.back()))
            throw std::invalid_argument("输入坐标点在样条函数定义域外！");
        for (size_t i = 1; i < knots.size(); i++)
        {
            if (x <= knots[i])
                return pp[i - 1](x);
        }
        return pp[pp.size() - 1](x);
};

const std::vector<Polynomial> &Spline<3, SplineType::B_spline>::get_Polys() const
{
    return pp;
}

void Spline<3, SplineType::B_spline>::print_Polys() const{
    if (pp.empty()) {
            cout << "No piecewise polynomials available. Please fit the spline first." << endl;
            return;
        }

        cout << "Piecewise Polynomials:" << endl;
        for (size_t i = 0; i < pp.size(); ++i) {
            cout << "Interval [" << knots[i] << ", " << knots[i + 1] << "): ";
            cout << pp[i].toString() << endl;
        }
};

void Spline<3, SplineType::B_spline>::check(const InterpCondition &c) const
{
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");

    // 检查是否按升序排列
    for (size_t i = 1; i < c.sites.size(); i++)
    {
        if (c.sites[i] < c.sites[i - 1])
            throw std::invalid_argument("样条插值输入坐标向量不是按升序排列的！");
    }

    // 检查knot是否重复
    std::vector<double> temp = c.sites;
    std::sort(temp.begin(), temp.end());
    for (size_t i = 1; i < temp.size(); i++)
    {
        if (temp[i] == temp[i - 1])
            throw std::invalid_argument("样条插值输入坐标向量中存在重复值！");
    }
}

void Spline<3, SplineType::B_spline>::generate_base_bspline(const vector<double> &knots){
        int M = knots.size();
        //生成基样条需要的结点，结点数为M+3
        vector<double> new_knots;
        new_knots.resize(M + 2*3);
        for (int i = 0; i < M + 2*3; i++)
        {
            if (i < 3)
                new_knots[i] = knots[0] - 3 + i;
            else if (i >= M + 3)
                new_knots[i] = knots[M - 1] + i - M - 3 + 1;
            else
                new_knots[i] = knots[i - 3];
        }
        
        //生成基样条
        B.resize(3 + M - 1);
        for (int i = 0; i < 3 + M - 1; i++)
        {
            vector<double> segment_vector(new_knots.begin() + i, new_knots.begin() + i + 3 + 2);
            B[i] = Base_Bspline(segment_vector);
        }
    };


// 初始化线性方程组的公共部分，即初始化A和rhs第1至N行
    void Spline<3, SplineType::B_spline>::initialize_common_A(Eigen::MatrixXd &A, Eigen::VectorXd &rhs, 
                                                            const std::vector<Base_Bspline> &B, 
                                                            const std::vector<double> &knots, const std::vector<double> &values)
    {
        int N = values.size();
        int M = B.size();
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                A(i, j) = B[j](knots[i]);
            }
        }
        for (int i = 0; i < N; i++)
        {
            rhs(i) = values[i];
        }
    };

    void Spline<3, SplineType::B_spline>::assign_polys_with_control_points(const Eigen::VectorXd &control_points, 
                                                                        const std::vector<Base_Bspline> &B)
    {
        int M = control_points.size();
        int N = 3 + 1;
        pp.resize(knots.size() - 1);
        for (int i = 0; i < knots.size()-1; i++)
        {
            Polynomial res{0.0};
            for (int j = 0; j < N; j++)
            {
                res += control_points[i + j] * B[i + j].poly(3 - j);
            }
            pp[i] = res;
        }
    };

// 5 kinds of splines
void Spline<3, SplineType::B_spline>::complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                             const std::vector<double> &values, double d)
{
    int N = knots.size();
    int M = rhs.size();
    if (left)
    {
        for (int j = 0; j < M; j++)
        {
            A(N, j) = B[j].d(1, knots[0]);
        }
        rhs(N) = d;
    }
    else
    {
        for (int j = 0; j < M; j++)
        {
            A(N+1, j) = B[j].d(1, knots[N-1]);
        }
        rhs(N+1) = d;
    }
}

void Spline<3, SplineType::B_spline>::specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double d2)
{
    int N = knots.size();
    int M = rhs.size();
    if (left)
    {
        for (int j = 0; j < M; j++)
        {
            A(N, j) = B[j].d(2, knots[0]);
        }
        rhs(N) = d2;
    }
    else
    {
        for (int j = 0; j < M; j++)
        {
            A(N+1, j) = B[j].d(2, knots[N-1]);
        }
        rhs(N+1) = d2;
    }
}

void Spline<3, SplineType::B_spline>::natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    int N = knots.size();
    int M = rhs.size();
    if (left)
    {
        for (int j = 0; j < M; j++)
        {
            A(N, j) = B[j].d(2, knots[0]);
        }
        rhs(N) = 0;
    }
    else
    {
        for (int j = 0; j < M; j++)
        {
            A(N+1, j) = B[j].d(2, knots[N-1]);
        }
        rhs(N+1) = 0;
    }
}

void Spline<3, SplineType::B_spline>::not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    throw std::invalid_argument("我的B样条尚不支持此边界条件！");
}

void Spline<3, SplineType::B_spline>::periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                  const vector<double> &knots,const std::vector<double> &values){
        int N = knots.size();
        int M = B.size();
        for (size_t i = 0; i < 3- 1; i++)
        {
            for (size_t j = 0; j < M; j++)
            {
                A(N + i , j) = B[j].d(i + 1 , knots[0]) - B[j].d(i + 1 , knots[N - 1]);
            }
        }
    };
//**************************************************





//**************************************************************************************** */



// template <int order>
// Spline<order,SplineType::B_spline>::Spline(const InterpCondition &cond){
//     int M = cond.sites.size();
//     if (order < 0 )
//         throw std::invalid_argument("B样条的阶数必须大于1！");
//     if (M <= 2)
//         throw std::invalid_argument("B样条的节点数必须大于2！");
    
//     check(cond);
//     generate_base_bspline(cond.sites);
//     int N = B.size();
//     Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N, N);
//     Eigen::VectorXd rhs = Eigen::VectorXd::Zero(N);
//     initialize_common_A(A, rhs, B, cond.function_values);
//     // 判断左边值类型并初始化A、rhs第一行
//     switch (cond.left)
//     {
//     case BCType::complete:
//         complete(true, A, rhs, cond.function_values, cond.derivative1);
//         break;
//     case BCType::specified_2nd:
//         specified_2nd(true, A, rhs, cond.derivative1);
//         break;
//     case BCType::natural:
//         natural(true, A, rhs);
//         break;
//     case BCType::not_a_knot:
//         not_a_knot(true, A, rhs);
//         break;
//     case BCType::periodic:
//         if (cond.right != BCType::periodic)
//             throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
//         if (cond.function_values[0]!=cond.function_values[M-1])
//             throw std::invalid_argument{"周期边界条件要求首尾值相等！"};
//         periodic(A, rhs, cond.function_values);
//         break;
//     default:
//         throw std::invalid_argument("未定义的边界条件类型！");
//         break;
//     }

//     // 判断右边值类型并初始化A、rhs最后一行
//     switch (cond.right)
//     {
//     case BCType::complete:
//         complete(false, A, rhs, cond.function_values, cond.derivative2);
//         break;
//     case BCType::specified_2nd:
//         specified_2nd(false, A, rhs, cond.derivative2);
//         break;
//     case BCType::natural:
//         natural(false, A, rhs);
//         break;
//     case BCType::not_a_knot:
//         not_a_knot(false, A, rhs);
//         break;
//     case BCType::periodic:
//         if (cond.left != BCType::periodic)
//             throw std::invalid_argument{"周期边界条件不能和其他边界条件混合使用！"};
//         if (cond.function_values[0]!=cond.function_values[N-1])
//             throw std::invalid_argument{"周期边界条件要求首尾值相等！"};
//         periodic(A, rhs, cond.function_values);
//         break;
//     default:
//         throw std::invalid_argument("未定义的边界条件类型！");
//         break;
//     }

//     // 使用Eigen求解线性方程组
//     Eigen::VectorXd control_points = A.fullPivLu().solve(rhs);
//     assign_polys_with_control_points(control_points, cond.function_values);       
// }


// 5 kinds of splines
// template <int order>
// void Spline<order,SplineType::B_spline>::complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
//                                              const std::vector<double> &values, double d)
// {
//     throw std::invalid_argument("我的B样条尚不支持此边界条件！");
// }
// template <int order>
// void Spline<order,SplineType::B_spline>::specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double d2)
// {
//     throw std::invalid_argument("我的B样条尚不支持此边界条件！");
// }
// template <int order>
// void Spline<order,SplineType::B_spline>::natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
// {
//     throw std::invalid_argument("我的B样条尚不支持此边界条件！");
// }
// template <int order>
// void Spline<order,SplineType::B_spline>::not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
// {
//     throw std::invalid_argument("我的B样条尚不支持此边界条件！");
// }
// template <int order>
// void Spline<order,SplineType::B_spline>::periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
//                                              const vector<double> &knots, const vector<double> &values)
// {
//     int N = values.size();
//     int M = B.size();
//     for (size_t i = 0; i < order; i++)
//     {
//         for (size_t j = 0; j < M; j++)
//         {
//             A(N , j) = B[j].d(i , knots[0]) - B[j].d(i , knots[N - 1]);
//         }
//     }

// }
//**************************************************

// template <int order>
// void Spline<order,SplineType::B_spline>::check(const InterpCondition &c) const
// {
//     if (order < 0)
//         throw std::invalid_argument("B样条的阶数必须大于0！");
//     if (c.sites.size() != c.function_values.size())
//         throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    
//     // 检查是否按升序排列
//     for (size_t i = 1; i < c.sites.size(); i++)
//     {
//         if (c.sites[i] < c.sites[i - 1])
//             throw std::invalid_argument("样条插值输入坐标向量不是按升序排列的！");
//     }

//     // 检查knot是否重复
//     std::vector<double> temp = c.sites;
//     std::sort(temp.begin(), temp.end());
//     for (size_t i = 1; i < temp.size(); i++)
//     {
//         if (temp[i] == temp[i - 1])
//             throw std::invalid_argument("样条插值输入坐标向量中存在重复值！");
//     }

// }

//**************************************************
// template <int order>
// void Spline<order,SplineType::B_spline>::generate_base_bspline(const vector<double> &knots)
// {
//     int M = knots.size();
//     //生成基样条需要的结点，结点数为M+order
//     Eigen::VectorXd new_knots = Eigen::VectorXd::Zero(M + 2 * order);
//     for (int i = 0; i < M + order; i++)
//     {
//         if (i < order)
//             new_knots[i] = knots[0] - order + i;
//         else if (i >= M + order)
//             new_knots[i] = knots[M - 1] + i - M - order + 1;
//         else
//             new_knots[i] = knots[i - order];
//     }
    
//     //生成基样条
//     B.resize(order + M - 1);
//     for (int i = 0; i < order + M - 1; i++)
//     {
//         B[i] = Base_Bspline(new_knots.segment(i, order + 2));
//     }
// }

// template <int order>
// void Spline<order,SplineType::B_spline>::initialize_common_A(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
//                                                            const vector<Base_Bspline> &B, 
//                                                            const vector<double> &knots, const vector<double> &values) const
// {
//     int N = values.size();
//     int M = B.size();
//     for (int i = 0; i < N; i++)
//     {
//         for (int j = 0; j < M; j++)
//         {
//             A(i, j) = B[j](knots[i]);
//         }
//     }
//     for (int i = 0; i < N; i++)
//     {
//         rhs(i) = values[i];
//     }
// }

// template <int order>
// void Spline<order,SplineType::B_spline>::assign_polys_with_control_points(const Eigen::VectorXd &control_points, const std::vector<Base_Bspline> &B)
// {
//     int M = control_points.size();
//     int N = M - order;
//     for (int i = 0; i < N; i++)
//     {
//         Polynomial res{0.0};
//         for (int j = 0; j < order; j++)
//         {
//             res += control_points[i + j] * B[i + j].poly(order - j);
//         }
//         pp[i] = res;
//     }
// }

// template <int order>
// double Spline<order,SplineType::B_spline>::operator()(double x) const
// {
//     if (!(x >= knots.front() && x <= knots.back()))
//         throw std::invalid_argument("输入坐标点在样条函数定义域外！");
//     for (size_t i = 1; i < knots.size(); i++)
//     {
//         if (x <= knots[i])
//             return pp[i - 1](x);
//     }
//     return pp[pp.size() - 1](x);
// }

// template <int order>
// const Polynomial &Spline<order,SplineType::B_spline>::Poly(int x) const
// {
//     if (x < 1 || x > static_cast<int>(pp.size()))
//         throw std::invalid_argument("索引越界！没有与输入索引对应的分片多项式。");
//     return pp[x - 1];
// }


// template <int order>
// const std::vector<Polynomial> &Spline<order,SplineType::B_spline>::get_Polys() const
// {
//     return pp;
// }

// template <int order>
// void Spline<order,SplineType::B_spline>::print_Polys() const {
//     if (pp.empty()) {
//         cout << "No piecewise polynomials available. Please fit the spline first." << endl;
//         return;
//     }

//     cout << "Piecewise Polynomials:" << endl;
//     for (size_t i = 0; i < pp.size(); ++i) {
//         cout << "Interval [" << knots[i] << ", " << knots[i + 1] << "): ";
//         cout << pp[i].toString() << endl;
//     }
// }