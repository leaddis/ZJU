#include "Spline.h"

// 对向量x,y按照x的单增顺序同时进行排列
void doubleSort(std::vector<double> &x, std::vector<double> &y)
{
    if (x.size() == y.size())
    {
        int size = x.size();
        int temp;
        for (int i = 0; i < size - 1; i++)
        {
            for (int j = 1; j < size; j++)
            {
                if (x[i] > x[j])
                {
                    temp = x[i];
                    x[i] = x[j];
                    x[j] = temp;
                    temp = y[i];
                    y[i] = y[j];
                    y[j] = temp;
                }
            }
        }
    }
}

//===================================================================
//                        ppForm Spline
//===================================================================

Spline<1, SplineType::ppForm>::Spline(const InterpCondition &c)
    : knots(c.sites), values(c.function_values)
{
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    doubleSort(knots, values); // 对knots, values进行按照knots单增的排序
}

double Spline<1, SplineType::ppForm>::operator()(double x) const
{
    // x must be in the defined domain of the spline
    //    if (!(x >= knots.front() && x <= knots.back()))
    //        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    for (size_t i = 1; i < knots.size(); i++)
    {
        // 判断x是否小于x_i
        if (x <= knots[i])
            return poly(i)(x);
    }
    // x大于x_{N-1}
    return poly(knots.size() - 1)(x);
}

const Polynomial Spline<1, SplineType::ppForm>::poly(int i) const
{
    double k = (values[i] - values[i - 1]) / (knots[i] - knots[i - 1]);
    double b = values[i - 1] - knots[i - 1] * (values[i] - values[i - 1]) / (knots[i] - knots[i - 1]);
    // 返回y=kx+b的分片多项式
    return Polynomial{b, k};
}

// Spline<3,ppForm>
//=======================================================================================================
//=======================================================================================================

inline double Spline<3, SplineType::ppForm>::mu(int i) const
{
    if (i <= 0 || i >= static_cast<int>(knots.size()))
        throw std::invalid_argument{"样条内部实现错误：输入索引在节点标号之外。"};
    if (i == 1)
    {
        int N = knots.size();
        return (knot(N) - knot(N - 1)) / (knot(2) - knot(1) + knot(N) - knot(N - 1));
    }
    else
    {
        return (knot(i) - knot(i - 1)) / (knot(i + 1) - knot(i - 1));
    }
}

inline double Spline<3, SplineType::ppForm>::lambda(int i) const
{
    return 1 - mu(i);
}

void Spline<3, SplineType::ppForm>::initialize_common_part(vector<vector<double>> &A, vector<double> &rhs,
                                                           const std::vector<double> &values) const
{
    int N = knots.size();
    for (int i = 1; i < N - 1; i++)
    {
        A[i][i] = 2;
        A[i][i - 1] = mu(i + 1);
        A[i][i + 1] = lambda(i + 1);
        rhs[i] = 6.0 * ((values[i + 1] - values[i]) / (knots[i + 1] - knots[i]) - (values[i] - values[i - 1]) / (knots[i] - knots[i - 1])) /
                 (knots[i + 1] - knots[i - 1]); 
    }
}

/*
void Spline<3, SplineType::ppForm>::initialize_common_part(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                                           const std::vector<double> &values) const {
    int N = knots.size();
    for (int i = 1; i < N - 1; i++) {
        A(i, i) = 2;
        A(i, i - 1) = mu(i + 1);
        A(i, i + 1) = lambda(i + 1);
        rhs(i) = 6.0 * ((values[i + 1] - values[i]) / (knots[i + 1] - knots[i]) -
                        (values[i] - values[i - 1]) / (knots[i] - knots[i - 1])) /
                 (knots[i + 1] - knots[i - 1]); // f[x_{i-1}, x_i, x_{i+1}]
    }
}*/
void Spline<3, SplineType::ppForm>::assign_polys_with_M(const vector<double> &M, const std::vector<double> &values)
{
    int N = knots.size();
    // 将M赋值给分片多项式
    for (int i = 0; i < N - 1; i++)
    {
        Polynomial res{values[i]};                                          // res = f_i
        Polynomial p{-knots[i], 1.0};                                       // p(x) = x - x_i
        double K = (values[i + 1] - values[i]) / (knots[i + 1] - knots[i]); // K = f[x_i,x_{i+1}]
        double c_1 = K - (2 * M[i] + M[i + 1]) * (knots[i + 1] - knots[i]) / 6.0;
        res += c_1 * p;              // res = f_i + c_1 (x - x_i)
        res += (M[i] / 2.0) * p * p; // res = f_i + c_1 (x - x_i) + M_i /2 *(x - x_i)^2
        double c_3 = (M[i + 1] - M[i]) / (knots[i + 1] - knots[i]) / 6.0;
        res += c_3 * p * p * p; // res = f_i + c_1 (x - x_i) + M_i /2 *(x - x_i)^2 + c_3(x - x_i)^3
        pp.push_back(res);
    }
}
/*
void Spline<3, SplineType::ppForm>::assign_polys_with_M(const Eigen::VectorXd &M,const std::vector<double> &values)
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
}*/

Spline<3, SplineType::ppForm>::Spline(const InterpCondition &c)
{
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    knots = c.sites;
    int N = knots.size();
    // 系数矩阵
    // Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N, N);
    vector<vector<double>> A(N, vector<double>(N, 0));
    // 线性方程组的右端向量
    // Eigen::VectorXd rhs = Eigen::VectorXd::Zero(N);
    vector<double> rhs(N, 0);
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
    vector<int> ipiv(N);
    vector<double> A1;
    for(int i=0;i<N*N;++i)
    {
        int k=i%N;
        int j=(i-k)/N;
        A1.push_back(A[j][k]);
    }
    int info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, N, 1, &A1[0], N, &ipiv[0], &rhs[0], 1); 
    if (info != 0)
    {
        throw std::invalid_argument("求解方程失败");
    }
    vector<double> M = rhs;
    // Eigen::VectorXd M = A.lu().solve(rhs);
    assign_polys_with_M(M, c.function_values);
}
void Spline<3, SplineType::ppForm>::complete(bool left, vector<vector<double>> &A, vector<double> &rhs,
                                             const std::vector<double> &values, double d)
{
    if (left)
    {
        A[0][0] = -(knot(2) - knot(1)) / 3;
        A[0][1] = -(knot(2) - knot(1)) / 6;
        rhs[0] = d - (values[1] - values[0]) / (knot(2) - knot(1));
    }
    else
    {
        int N = knots.size();
        A[N - 1][N - 2] = (knot(N) - knot(N - 1)) / 6;
        A[N - 1][N - 1] = (knot(N) - knot(N - 1)) / 3;
        rhs[N - 1] = d - (values[N - 1] - values[N - 2]) / (knot(N) - knot(N - 1));
    }
}
void Spline<3, SplineType::ppForm>::specified_2nd(bool left, vector<vector<double>> &A, vector<double> &rhs, double d)
{
    // 若left为true则说明为左边值，初始化第一行A、rhs，否则初始化最后一行A、rhs
    if (left)
    {
        A[0][0] = 1;
        rhs[0] = d;
    }
    else
    {
        int N = knots.size();
        A[N - 1][N - 1] = 1;
        rhs[N - 1] = d;
    }
}

void Spline<3, SplineType::ppForm>::natural(bool left, vector<vector<double>> &A, vector<double> &rhs)
{
    specified_2nd(left, A, rhs, 0);
}

void Spline<3, SplineType::ppForm>::not_a_knot(bool left, vector<vector<double>> &A, vector<double> &rhs)
{
    // 若left为true则说明为左边值，初始化第一行A、rhs，否则初始化最后一行A、rhs
    if (left)
    {
        A[0][0] = -lambda(2);
        A[0][1] = 1;
        A[0][2] = -mu(2);
        rhs[0] = 0;
    }
    else
    {
        int N = knots.size();
        A[N - 1][N - 3] = -lambda(N - 1);
        A[N - 1][N - 2] = 1;
        A[N - 1][N - 1] = -mu(N - 1);
        rhs[N - 1] = 0;
    }
}

void Spline<3, SplineType::ppForm>::periodic(vector<vector<double>> &A, vector<double> &rhs,
              const std::vector<double> &values)
{
    // 周期样条左右边值必须一致，故不需判断左右
    int N = knots.size();
    A[0][0] = 2.0;
    A[0][1] = lambda(1);
    A[0][ N - 2] = mu(1);
    A[N - 1][0] = 1.0;
    A[N - 1][N - 1] = -1.0;
    rhs[0] = (values[1] - values[0]) / (knots[1] - knots[0]);
    rhs[0] -= (values[N - 1] - values[N - 2]) / (knots[N - 1] - knots[N - 2]);
    rhs[0] *= 6.0;
    rhs[0] /= knots[1] - knots[0] + knots[N - 1] - knots[N - 2];
    rhs[N - 1] = 0;
}
/*
void Spline<3, SplineType::ppForm>::complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                             const std::vector<double> &values, double d)
{
    // 若left为true则说明为左边值，初始化第一行A、rhs，否则初始化最后一行A、rhs
    if (left)
    {
        A(0, 0) = -(knot(2) - knot(1)) / 3;
        A(0, 1) = -(knot(2) - knot(1)) / 6;
        rhs(0) = d - (values[1] - values[0]) / (knot(2) - knot(1));
    }
    else
    {
        int N = knots.size();
        A(N - 1, N - 2) = (knot(N) - knot(N - 1)) / 6;
        A(N - 1, N - 1) = (knot(N) - knot(N - 1)) / 3;
        rhs(N - 1) = d - (values[N - 1] - values[N - 2]) / (knot(N) - knot(N - 1));
    }
}

void Spline<3, SplineType::ppForm>::specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double d)
{
    // 若left为true则说明为左边值，初始化第一行A、rhs，否则初始化最后一行A、rhs
    if (left)
    {
        A(0, 0) = 1;
        rhs(0) = d;
    }
    else
    {
        int N = knots.size();
        A(N - 1, N - 1) = 1;
        rhs(N - 1) = d;
    }
}

void Spline<3, SplineType::ppForm>::natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    specified_2nd(left, A, rhs, 0);
}

void Spline<3, SplineType::ppForm>::not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs)
{
    // 若left为true则说明为左边值，初始化第一行A、rhs，否则初始化最后一行A、rhs
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
    // 周期样条左右边值必须一致，故不需判断左右
    int N = knots.size();
    A(0, 0) = 2.0;
    A(0, 1) = lambda(1);
    A(0, N - 2) = mu(1);
    A(N - 1, 0) = 1.0;
    A(N - 1, N - 1) = -1.0;
    rhs(0) = (values[1] - values[0]) / (knots[1] - knots[0]);
    rhs(0) -= (values[N - 1] - values[N - 2]) / (knots[N - 1] - knots[N - 2]);
    rhs(0) *= 6.0;
    rhs(0) /= knots[1] - knots[0] + knots[N - 1] - knots[N - 2];
    rhs(N - 1) = 0;
}
*/
double Spline<3, SplineType::ppForm>::operator()(double x) const
{
    //    if (!(x >= knots.front() && x <= knots.back()))
    //        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    for (size_t i = 1; i < knots.size(); i++)
    {
        if (x <= knots[i])
            return pp[i - 1](x);
    }
    //    return 0; // never reached
    // x大于x_{N-1}
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
// 将ppForm样条形式转化为北太天元结构体形式
baltam::structure *Spline<3, SplineType::ppForm>::spline_to_baltam_structure() const
{
    using namespace baltam;
    auto *res = new baltam::structure();
    // form : pp
    res->set_field("form", new ba_obj("pp", ba_char_mat));

    // breaks ： knots
    int n = size();
    auto *pBreaks = new baltam::matrix<double>(1, n);
    for (int i = 1; i <= n; ++i)
    {
        (*pBreaks)[i - 1] = knot(i);
    }
    res->set_field("breaks", new ba_obj(ba_double_mat, pBreaks));

    // coefs : coefficients of polynomial of each piece
    auto *pCoefs = new baltam::matrix<double>(pp.size(), 4);
    for (int i = 1; i <= static_cast<int>(pp.size()); ++i)
    {
        for (int j = 0; j <= 3; ++j)
        {
            (*pCoefs)(i - 1, j) = poly(i).reformulate(knot(i))[3 - j];
        }
    }
    res->set_field("coefs", new ba_obj(ba_double_mat, pCoefs));

    // order : 3 ( this is cubic spline interpolation, why the return value is MATLAB is 4?)
    res->set_field("order", new ba_obj(4));

    // pieces : number of piecewise polynomials
    res->set_field("pieces", new ba_obj(static_cast<int>(pp.size())));

    // dim :1 ( one-dimensional function )
    res->set_field("dim", new ba_obj(1));

    return res;
}

Spline<3, SplineType::ppForm>::Spline(const baltam::structure &s)
{
    // 判断北太天元结构体是否为样条形式
    bool right_struct = s.isfield("form") && s.isfield("breaks") && s.isfield("coefs") &&
                        s.isfield("order") && s.isfield("pieces") && s.isfield("dim");
    if (!right_struct)
        throw std::invalid_argument{"无效参数：非样条结构体。"};
    // 为pp和knots赋初值
    auto Breaks = s.get_field("breaks")->get<baltam::matrix<double>>();
    auto Coefs = s.get_field("coefs")->get<baltam::matrix<double>>();
    if (Breaks->cols() != Coefs->rows() + 1)
        throw std::invalid_argument{"无效参数：错误的样条结构体，节点个数与分片多项式个数不匹配。"};
    for (int i = 0; i < Breaks->cols(); ++i)
    {
        knots.push_back((*Breaks)(i));
    }
    for (int i = 0; i < Coefs->rows(); ++i)
    {
        std::vector<double> temp;
        for (int j = 0; j < Coefs->cols(); ++j)
        {
            temp.push_back((*Coefs)(i, j));
        }
        pp.push_back(Polynomial{(*Breaks)(i), temp, false});
    }
}

//===================================================================
//                          Base Spline
//===================================================================
/*
Spline<1, SplineType::BSpline>::Spline(const InterpCondition &c)
        : knots(c.sites), coefs(c.function_values) {
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
}


double Spline<1, SplineType::BSpline>::operator()(double x) const {
    // x must be in the defined domain of the spline
    if (!(x >= knots.front() && x <= knots.back()))
        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    for (size_t i = 1; i < knots.size(); i++) // i = 2,3,...,N-2, x in ( t_{i-1},t_i ]
    {
        //1阶B样条确定x处的值需要B_{i-1}和B_i
        if (x <= knots[i])
            if (i == 1) {
                double ghost = knots.front() - (knots.back() - knots.front()) / knots.size();//ghost点，为节点的自然延拓
                std::vector<double> left={ghost, knots[0], knots[1]},right={knots[0], knots[1], knots[2]};
                return coefs[0] * Base_Spline(left)(x) +
                       coefs[1] * Base_Spline(right)(x);
            } else if (i == knots.size() - 1) {
                int N = knots.size();
                double ghost = knots.back() + (knots.back() - knots.front()) / knots.size();//ghost点，为节点的自然延拓
                std::vector<double> right={knots[N - 2], knots[N - 1], ghost},left={knots[N - 3], knots[N - 2], knots[N - 1]};
                return coefs[N - 1] * Base_Spline(right)(x) +
                       coefs[N - 2] * Base_Spline(left)(x);
            } else
            {
                std::vector<double> left={knots[i - 2], knots[i - 1], knots[i]},right={knots[i - 1], knots[i], knots[i + 1]};
                return coefs[i - 1] * Base_Spline(left)(x) +
                       coefs[i] * Base_Spline(right)(x);
            }

        else;
    }
    return 0; // never reached, quiet the complier
}

// cubic BSpline
//==============================================================================================
//==============================================================================================

double Spline<3, SplineType::BSpline>::operator()(double x) const {
    if (!(x >= knots.front() && x <= knots.back()))
        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    int N = knots.size();
    //三阶B样条确定x处的值需要B_{i-3}、B_{i-2}、B_{i-1}和B_i
    for (int i = 1; i <= N - 1; i++) {
        if (x <= knot(i + 1)) {
            double res = 0;
            res += coef(i - 2) * Base_Spline{knot(i - 3), knot(i - 2), knot(i - 1), knot(i), knot(i + 1)}(x);
            res += coef(i - 1) * Base_Spline{knot(i - 2), knot(i - 1), knot(i), knot(i + 1), knot(i + 2)}(x);
            res += coef(i) * Base_Spline{knot(i - 1), knot(i), knot(i + 1), knot(i + 2), knot(i + 3)}(x);
            res += coef(i + 1) * Base_Spline{knot(i), knot(i + 1), knot(i + 2), knot(i + 3), knot(i + 4)}(x);
            return res;
        } else;
    }
    return 0; // never reached
}

inline double Spline<3, SplineType::BSpline>::coef(int i) const {
    int N = knots.size();
    if (i < -1 || i > N)
        throw std::invalid_argument("索引越界！没有与输入索引对应的样条基函数系数。");
    return coefs[i + 1];
}

inline double Spline<3, SplineType::BSpline>::knot(int x) const {
    int N = knots.size();
    if (x < -2 || x > N + 3)
        throw std::invalid_argument("索引越界！没有与输入索引对应的结点。");
    if (1 <= x && x <= N)
        return knots[x - 1];
    else {
        double h = (knots.back() - knots.front()) / (N - 1); // average interval length
        if (x <= 0)
            return knots.front() + (x - 1) * h;
        else
            return knots.back() + (x - N) * h;
    }
}

const Polynomial Spline<3, SplineType::BSpline>::poly(int i) const {
    return Polynomial{};
}

Spline<3, SplineType::BSpline>::Spline(const InterpCondition &c) {
    if (c.sites.size() != c.function_values.size())
        throw std::invalid_argument("样条插值输入坐标向量大小不一致！");
    switch (c.left) {
        case BCType::complete:
            ctor_complete(c.sites, c.function_values, c.derivative1, c.derivative2);
            break;
        case BCType::specified_2nd:
            ctor_specified_2nd(c.sites, c.function_values, c.derivative1, c.derivative2);
            break;
        case BCType::natural:
            ctor_natural(c.sites, c.function_values);
            break;
        case BCType::not_a_knot:
            ctor_not_a_knot(c.sites, c.function_values);
            break;
        case BCType::periodic:
            ctor_periodic(c.sites, c.function_values);
            break;

        default:
            throw std::invalid_argument("未定义的边界条件类型！");
            break;
    }
}

//初始化矩阵的公共部分
void Spline<3, SplineType::BSpline>::init_matrix(Eigen::MatrixXd &A,
                                                 Eigen::VectorXd &rhs,
                                                 const std::vector<double> &values) {
    int N = knots.size();
    A = Eigen::MatrixXd::Zero(N + 2, N + 2);
    rhs = Eigen::VectorXd::Zero(N + 2);
    for (size_t i = 0; i < values.size(); i++) {
        rhs(i) = values[i];
    }
    //初始化前N行，后2行为边值条件
    // set some entries of A , i.e. the first N rows
    for (int i = 0; i <= N - 1; i++) // i is the index of the current row
    {
        A(i, i) = Base_Spline{knot(i - 2), knot(i - 1), knot(i), knot(i + 1), knot(i + 2)}(knot(i + 1));
        A(i, i + 1) = Base_Spline{knot(i - 1), knot(i), knot(i + 1), knot(i + 2), knot(i + 3)}(knot(i + 1));
        A(i, i + 2) = Base_Spline{knot(i), knot(i + 1), knot(i + 2), knot(i + 3), knot(i + 4)}(knot(i + 1));
    }
}

void Spline<3, SplineType::BSpline>::ctor_complete(const std::vector<double> &sites,
                                                   const std::vector<double> &values,
                                                   double d1, double d2) {
    knots = sites;
    Eigen::MatrixXd A;
    Eigen::VectorXd rhs;
    init_matrix(A, rhs, values);
    int N = knots.size();
    // set the last two entries of rhs
    rhs(N) = d1;
    rhs(N + 1) = d2;
    double a = knots.front();
    double b = knots.back();
    // set the last two rows of A
    A(N, 0) = Base_Spline{knot(-2), knot(-1), knot(0), knot(1), knot(2)}.d(a);
    A(N, 1) = Base_Spline{knot(-1), knot(0), knot(1), knot(2), knot(3)}.d(a);
    A(N, 2) = Base_Spline{knot(0), knot(1), knot(2), knot(3), knot(4)}.d(a);
    A(N + 1, N - 1) = Base_Spline{knot(N - 3), knot(N - 2), knot(N - 1), knot(N), knot(N + 1)}.d(b);
    A(N + 1, N) = Base_Spline{knot(N - 2), knot(N - 1), knot(N), knot(N + 1), knot(N + 2)}.d(b);
    A(N + 1, N + 1) = Base_Spline{knot(N - 1), knot(N), knot(N + 1), knot(N + 2), knot(N + 3)}.d(b);
    // solve the linear equation
    Eigen::VectorXd res = A.lu().solve(rhs);
    for (int i = 0; i < N + 2; i++) {
        coefs.push_back(res(i)); // set coefs exactly the result
    }
}

void Spline<3, SplineType::BSpline>::ctor_specified_2nd(const std::vector<double> &sites,
                                                        const std::vector<double> &values,
                                                        double d1, double d2) {
    knots = sites;
    Eigen::MatrixXd A;
    Eigen::VectorXd rhs;
    init_matrix(A, rhs, values);
    int N = knots.size();
    // set the last two entries of rhs
    rhs(N) = d1;
    rhs(N + 1) = d2;
    double a = knots.front();
    double b = knots.back();
    // set the last two rows of A
    A(N, 0) = Base_Spline{knot(-2), knot(-1), knot(0), knot(1), knot(2)}.dd(a);
    A(N, 1) = Base_Spline{knot(-1), knot(0), knot(1), knot(2), knot(3)}.dd(a);
    A(N, 2) = Base_Spline{knot(0), knot(1), knot(2), knot(3), knot(4)}.dd(a);
    A(N + 1, N - 1) = Base_Spline{knot(N - 3), knot(N - 2), knot(N - 1), knot(N), knot(N + 1)}.dd(b);
    A(N + 1, N) = Base_Spline{knot(N - 2), knot(N - 1), knot(N), knot(N + 1), knot(N + 2)}.dd(b);
    A(N + 1, N + 1) = Base_Spline{knot(N - 1), knot(N), knot(N + 1), knot(N + 2), knot(N + 3)}.dd(b);
    // solve the linear equation
    Eigen::VectorXd res = A.lu().solve(rhs);
    for (int i = 0; i < N + 2; i++) {
        coefs.push_back(res(i)); // set coefs exactly the result
    }
}

void Spline<3, SplineType::BSpline>::ctor_natural(const std::vector<double> &sites,
                                                  const std::vector<double> &values) {
    ctor_specified_2nd(sites, values, 0, 0);
}

void Spline<3, SplineType::BSpline>::ctor_not_a_knot(const std::vector<double> &sites,
                                                     const std::vector<double> &values) {
}

void Spline<3, SplineType::BSpline>::ctor_periodic(const std::vector<double> &sites,
                                                   const std::vector<double> &values) {
    knots = sites;
    Eigen::MatrixXd A;
    Eigen::VectorXd rhs;
    init_matrix(A, rhs, values);
    int N = knots.size();
    // set the last two entries of rhs
    rhs(N) = 0.0;
    rhs(N + 1) = 0.0;
    double a = knots.front();
    double b = knots.back();
    // set the last two rows of A
    A(N, 0) = Base_Spline{knot(-2), knot(-1), knot(0), knot(1), knot(2)}.d(a);
    A(N, 1) = Base_Spline{knot(-1), knot(0), knot(1), knot(2), knot(3)}.d(a);
    A(N, 2) = Base_Spline{knot(0), knot(1), knot(2), knot(3), knot(4)}.d(a);

    A(N + 1, 0) = Base_Spline{knot(-2), knot(-1), knot(0), knot(1), knot(2)}.dd(a);
    A(N + 1, 1) = Base_Spline{knot(-1), knot(0), knot(1), knot(2), knot(3)}.dd(a);
    A(N + 1, 2) = Base_Spline{knot(0), knot(1), knot(2), knot(3), knot(4)}.dd(a);

    A(N, N - 1) = -Base_Spline{knot(N - 3), knot(N - 2), knot(N - 1), knot(N), knot(N + 1)}.d(b);
    A(N, N) = -Base_Spline{knot(N - 2), knot(N - 1), knot(N), knot(N + 1), knot(N + 2)}.d(b);
    A(N, N + 1) = -Base_Spline{knot(N - 1), knot(N), knot(N + 1), knot(N + 2), knot(N + 3)}.d(b);

    A(N + 1, N - 1) = -Base_Spline{knot(N - 3), knot(N - 2), knot(N - 1), knot(N), knot(N + 1)}.dd(b);
    A(N + 1, N) = -Base_Spline{knot(N - 2), knot(N - 1), knot(N), knot(N + 1), knot(N + 2)}.dd(b);
    A(N + 1, N + 1) = -Base_Spline{knot(N - 1), knot(N), knot(N + 1), knot(N + 2), knot(N + 3)}.dd(b);

    // solve the linear equation
    Eigen::VectorXd res = A.lu().solve(rhs);
    for (int i = 0; i < N + 2; i++) {
        coefs.push_back(res(i)); // set coefs exactly the result
    }
}

//================================================================================================
//================================================================================================

double Spline<2, SplineType::BSpline>::coef(int i) const {
    int N = knots.size();
    if (i < 0 || i > N)
        throw std::invalid_argument("索引越界！没有与输入索引对应的样条基函数系数。");
    return coefs[i];
}

inline double Spline<2, SplineType::BSpline>::knot(int x) const {
    int N = knots.size();
    if (x < -1 || x > N + 2)
        throw std::invalid_argument("索引越界！没有与输入索引对应的结点。");
    if (1 <= x && x <= N)
        return knots[x - 1];
    else {
        double h = (knots.back() - knots.front()) / (N - 1); // average interval length
        if (x <= 0)
            return knots.front() + (x - 1) * h;
        else
            return knots.back() + (x - N) * h;
    }
}

double Spline<2, SplineType::BSpline>::operator()(double x) const {
    if (!(x >= knots.front() && x <= knots.back()))
        throw std::invalid_argument("输入坐标点在样条函数定义域外！");
    int N = knots.size();
    //二阶B样条确定x处的值需要B_{i-2}、B_{i-1}和B_i
    for (int i = 1; i <= N - 1; i++) {
        if (x <= knot(i + 1)) {
            double res = 0;
            res += coef(i - 1) * Base_Spline{knot(i - 2), knot(i - 1), knot(i), knot(i + 1)}(x);
            res += coef(i) * Base_Spline{knot(i - 1), knot(i), knot(i + 1), knot(i + 2)}(x);
            res += coef(i + 1) * Base_Spline{knot(i), knot(i + 1), knot(i + 2), knot(i + 3)}(x);
            return res;
        } else;
    }
    return 0; // never reached
}
*/