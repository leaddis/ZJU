#ifndef SPLINE_H
#define SPLINE_H

#include "Polynomial.h"
#include "Base_Bspline.h"
#include <Eigen/Dense>

using namespace std;

enum BCType{
    complete,
    natural,
    specified_2nd,
    not_a_knot,
    periodic,
    Thm3_58
};

enum SplineType{
    ppForm,
    B_spline
};

struct InterpCondition
{
    std::vector<double> sites = std::vector<double>{};
    std::vector<double> function_values = std::vector<double>{};
    double derivative1 = 0.0;
    double derivative2 = 0.0;
    BCType left;
    BCType right;

    // // 构造函数，用于简单的拟合即左右边界条件都是natural
    // InterpCondition(const std::vector<double>& x, const std::vector<double>& y)
    //     :sites(x), function_values(y), left(natural), right(natural), derivative1(0.0), derivative2(0.0) {}

    // // 构造函数，用于自然和非结点边界条件
    // InterpCondition(const std::vector<double>& x, const std::vector<double>& y, BCType left_bc, BCType right_bc)
    //     : sites(x), function_values(y), left(left_bc), right(right_bc), derivative1(0.0), derivative2(0.0) {}

    // // 构造函数，用于完备边界条件
    // InterpCondition(const std::vector<double>& x, const std::vector<double>& y, 
    //                 BCType left_bc, double d1, BCType right_bc, double d2)
    //     : sites(x), function_values(y), left(left_bc), right(right_bc), derivative1(d1), derivative2(d2) {}
};

template <int Order, SplineType t>
class Spline{};

// explicit specializations of order-1 and order-3 splines

//=======================================================================================================
//                                          ppForm Spline
//=======================================================================================================



//=========================================
//          order-1 ppForm Spline
//=========================================
template <>
class Spline<1, SplineType::ppForm>
{
public:
    // 构造函数
    Spline() = default;
    explicit Spline(const InterpCondition& c);

    // 拟合方法，使用std::vector<double>
    void fit(const std::vector<double>& x, const std::vector<double>& y);

    // 重载()运算符用于评估
    double operator()(double x_val) const;

private:
    std::vector<double> knots;   // 节点位置
    std::vector<double> values;  // 节点处的函数值
    std::vector<double> a;       // 常数项系数
    std::vector<double> b;       // 线性项系数

    // 辅助方法：按x的单增顺序对knots和values进行排序
    void doubleSort(std::vector<double>& x, std::vector<double>& y);
};


//=========================================
//      order-3 ppForm Spline
//=========================================
template <>
class Spline<3, SplineType::ppForm>
{
private:
    vector<double> knots;         // 节点
    vector<Polynomial> pp;        // 分片多项式

    // 类内部实现所需的函数，对外无接口
private:
    // 构造样条所需参数
    inline double mu(int i) const;
    // 构造样条所需参数
    inline double lambda(int i) const;
    // 初始化线性方程组的公共部分，即初始化A和rhs第2至N-1行
    void initialize_common_part(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                                const std::vector<double> &) const;
    // 用二阶导数向量M构造每一段上的分片多项式
    void assign_polys_with_M(const Eigen::VectorXd &M, const std::vector<double> &values);
    // 完全样条
    void complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                  const std::vector<double> &values, double);
    // 二阶样条
    void specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double);
    // 自然样条
    void natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
    // 非节点样条
    void not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
    // 周期样条
    void periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                  const std::vector<double> &values);
    
    // 检查输入
    void check_input(const InterpCondition &c) const;

public:
    // 缺省构造
    Spline() = default;
    // 构造函数，输入为初始值结构体
    Spline(const InterpCondition &);

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

    /// @brief 返回所有分片多项式
    /// @return std::vector<Polynomial>
    const std::vector<Polynomial> &getPolys() const;

    /// @brief 打印所有分片多项式
    /// @return void
    void printPolys() const;
};

//===========================================================================================================
//                                             B-spline
//===========================================================================================================

template<>
class Spline<3,B_spline>{
    public:
        Spline() = default;
        Spline(const InterpCondition &cond);
        double operator()(double x) const;
    
    public:
        /// @brief 返回所有分片多项式
        /// @return std::vector<Polynomial>
        const vector<Polynomial> &get_Polys() const;
        void print_Polys() const;

    private:
        std::vector<double> knots;
        std::vector<Polynomial> pp;
        std::vector<Base_Bspline> B;
        void check(const InterpCondition &c) const;
        void generate_base_bspline(const std::vector<double> &knots);
        void initialize_common_A(Eigen::MatrixXd &A, Eigen::VectorXd &rhs, const std::vector<Base_Bspline> &B, const std::vector<double> &, const std::vector<double> &);
        void complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, const std::vector<double> &values, double d);
        void specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double d2);
        void natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
        void not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs);
        void periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs, const std::vector<double> &knots, const std::vector<double> &values);
        void assign_polys_with_control_points(const Eigen::VectorXd &control_points, const std::vector<Base_Bspline> &B);

};


// any order B-spline
template <int order>
class Spline<order,B_spline> {
public:
    // 缺省构造
    Spline() = default;
    // 构造函数，输入为初始值结构体
    Spline(const InterpCondition &cond){
        int M = cond.sites.size();
        if (order < 0)
            throw std::invalid_argument("B样条的阶数必须大于1！");
        if (M <= 2)
            throw std::invalid_argument("B样条的节点数必须大于2！");

        check(cond);
        knots = cond.sites;
        values = cond.function_values;
        generate_base_bspline(cond.sites);
        int N = B.size();
        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N, N);
        Eigen::VectorXd rhs = Eigen::VectorXd::Zero(N);
        initialize_common_A(A, rhs, B, cond.sites, cond.function_values);
        if (order > 1){
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
                case BCType::Thm3_58:
                    Thm_3_58(true, A, rhs);
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
                case BCType::Thm3_58:
                    Thm_3_58(false, A, rhs);
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
        }

        // 使用Eigen求解线性方程组
        Eigen::VectorXd control_points = A.fullPivLu().solve(rhs);
        assign_polys_with_control_points(control_points, B);
    };

    /// @brief 返回样条在x的值，在定义域外为邻边界分片多项式的自然延拓
    /// @param x
    /// @return s(x)
    double operator()(double x) const{
        if (!(x >= knots.front() && x <= knots.back()))
            throw std::invalid_argument("输入坐标点在样条函数定义域外！");
        for (size_t i = 1; i < knots.size(); i++)
        {
            if (x <= knots[i])
                return pp[i - 1](x);
        }
        return pp[pp.size() - 1](x);
    };
    
public:
    // /// @brief 返回  [ t_i, t_ {i+1} ]处的多项式
    // /// @param i 的范围为 [ 1, N-1 ], N 为节点数量
    // /// @return p_i(x)
    // const Polynomial &Poly(int i) const{
    //     if (x < 1 || x > static_cast<int>(pp.size()))
    //     throw std::invalid_argument("索引越界！没有与输入索引对应的分片多项式。");
    //     return pp[x - 1];
    // };

    /// @brief 返回所有分片多项式
    /// @return std::vector<Polynomial>
    const std::vector<Polynomial> &get_Polys() const{
        return pp;
    };

    /// @brief 打印所有分片多项式
    /// @return void
    void print_Polys() const{
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

private:
    vector <double> knots;             // Knot positions
    vector <double> values;            // Function values at knots
    Eigen::VectorXd control_points;    // Control points
    vector <Polynomial> pp;           // Polynomial representation for each interval
    vector <Base_Bspline> B;            // Base B-spline

private:
    // 检查输入
    void check(const InterpCondition &c) const{
        if (order < 0)
            throw std::invalid_argument("B样条的阶数必须大于0！");
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
    };
    // 完全样条
    void complete(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                  const std::vector<double> &values, double){
                    throw std::invalid_argument("我的B样条尚不支持此边界条件！");
    };
    // 二阶样条
    void specified_2nd(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs, double){
        throw std::invalid_argument("我的B样条尚不支持此边界条件！");
    };
    // 自然样条
    void natural(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs){
        throw std::invalid_argument("我的B样条尚不支持此边界条件！");
    };
    // 非节点样条
    void not_a_knot(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs){
        throw std::invalid_argument("我的B样条尚不支持此边界条件！");
    };
    //
    void Thm_3_58(bool left, Eigen::MatrixXd &A, Eigen::VectorXd &rhs){
        int M = rhs.size();
        int N = knots.size();
        if (order != 2)
            throw std::invalid_argument("我的B样条尚不支持此边界条件！");
        else{
            if (left)
            {
                A(N,0) = 1;
                A(N,1) = 1;
                rhs(N) = 2 * values[0];
            }
            else
            {
                // A(N+1,M-2) = 1;
                // A(N+1,M-1) = 1;
                // rhs(N+1) = 2 * values[values.size() - 1];
            }
        }
    }
    // 周期样条
    void periodic(Eigen::MatrixXd &A, Eigen::VectorXd &rhs,
                  const vector<double> &knots,const std::vector<double> &values){
        int N = knots.size();
        int M = B.size();
        for (size_t i = 0; i < order - 1; i++)
        {
            for (size_t j = 0; j < M; j++)
            {
                A(N + i , j) = B[j].d(i + 1 , knots[0]) - B[j].d(i + 1 , knots[N - 1]);
            }
        }
    };
    //生成基样条
    void generate_base_bspline(const vector<double> &knots){
        int M = knots.size();
        //生成基样条需要的结点，结点数为M+order
        vector<double> new_knots;
        new_knots.resize(M + 2*order);
        for (int i = 0; i < M + 2*order; i++)
        {
            if (i < order)
                new_knots[i] = knots[0] - order + i;
            else if (i >= M + order)
                new_knots[i] = knots[M - 1] + i - M - order + 1;
            else
                new_knots[i] = knots[i - order];
        }
        
        //生成基样条
        B.resize(order + M - 1);
        for (int i = 0; i < order + M - 1; i++)
        {
            vector<double> segment_vector(new_knots.begin() + i, new_knots.begin() + i + order + 2);
            B[i] = Base_Bspline(segment_vector);
        }
    };//good

    // 初始化线性方程组的公共部分，即初始化A和rhs第1至N行
    void initialize_common_A(Eigen::MatrixXd &A , Eigen::VectorXd &rhs,
                                const vector<Base_Bspline> &B, 
                                const vector<double> &knots, const vector<double> &values) const{
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
    // 用control_point和base_spline构造每一段上的分片多项式
    void assign_polys_with_control_points(const Eigen::VectorXd &control_points, const std::vector<Base_Bspline> &B){
        int M = control_points.size();
        int N = order + 1;
        pp.resize(knots.size() - 1);
        for (int i = 0; i < knots.size()-1; i++)
        {
            Polynomial res{0.0};
            for (int j = 0; j < N; j++)
            {
                res += control_points[i + j] * B[i + j].poly(order - j);
            }
            pp[i] = res;
        }
    };
};
#endif // SPLINE_H