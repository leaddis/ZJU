/*
 * ==========================================================================
 *
 *       FileName:  fnval.cpp
 *
 *    Description:  source file for fnval
 *
 *        Version:  1.0
 *        Created:  2023.3.22
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  shaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#include "fnval.h"
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/structure.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include "splines_common.h"
#include <iostream>

static const char *g_pFnvalHelp = R"(
fnval 函数返回样条在一个点或一组点处的函数值。

    y = fnval(s,x) 返回样条函数 s 在 x 处的值,x 可以为标量、一维向量。

    fnval(s,x) 同 fnval(x,s)。
)";

namespace baltam::splines
{
    // 内部调用，调用者需保证输入的样条结构体的合法性，返回 S(x),
    // 如要得到多个点的值，请使用 vector 参数重载形式
    double fnval(const baltam::structure &S, double x)
    {
        if (S.get_field("form")->as_string() == "pp")
        {
            // 从输入的样条结构体中提取出节点和分片多项式系数矩阵
            auto Knots = S.get_field("breaks")->get<matrix<double>>();
            auto Coefs = S.get_field("coefs")->get<matrix<double>>();
            for (int i = 1; i < static_cast<int>(Knots->cols()); ++i)
            {
                // 找到 x 位于哪一片多项式中
                if (x <= (*Knots)(0, i))
                {
                    std::vector<double> T; // 对应的多项式的系数向量
                    for (int j = 0; j < static_cast<int>(Coefs->cols()); ++j)
                    {
                        T.push_back((*Coefs)(i - 1, j));
                    }
                    // 结构体中的系数为泰勒展开形式，所以用这个构造函数
                    Polynomial P((*Knots)(0, i - 1), T, false);
                    return P(x);
                }
            }
            // 这里意味着 x > Knots.back() ，需要构造最后一个多项式
            std::vector<double> T;
            for (int i = 0; i < static_cast<int>(Coefs->cols()); ++i)
            {
                T.push_back((*Coefs)(Coefs->rows() - 1, i));
            }
            Polynomial P((*Knots)(0, Knots->cols() - 2), T, false);
            return P(x);
        }
        else if (S.get_field("form")->as_string() == "B-")
        {
            // 从 S 中提取出来各个 B 样条基函数，然后计算 res
            auto Knots = S.get_field("knots")->get<matrix<double>>();
            auto Coefs = S.get_field("coefs")->get<matrix<double>>();
            auto Number = S.get_field("number")->as_int();
            auto Order = S.get_field("order")->as_int();
            std::vector<Base_Spline> B;    // 全部基函数的集合
            std::vector<double> end_point; // 每个基样条的右端点
            end_point.resize(0, 0);
            // 构造每一个基函数
            for (int i = 0; i < Number; ++i)
            {
                std::vector<double> k; // 第 i 个基函数的结点序列
                for (int j = 0; j < Order + 1; ++j)
                { // 每个基函数有 Order + 1 个结点
                    k.push_back((*Knots)(0, i + j));
                }
                end_point.push_back((*Knots)(0, i + Order));
                B.push_back(Base_Spline(k));
            }
            // 计算结果
            double res = 0.0, eps = 1e-6;
            for (size_t j = 0; j < B.size(); ++j)
            {
                double tt = (*Coefs)(0, j) * B[j](x);
                if ((j != B.size() - 1) && (fabs(x - end_point[j]) <= eps))
                    tt = 0;
                res += tt;
            }
            return res;
        }
        else
            throw std::invalid_argument{"不合法的样条结构体。"};
    }
    // 内部调用，调用者需保证输入的样条结构体的合法性，返回 [y_1 , ... , y_n] = S([x_1 , ... , x_n])
    std::vector<double> fnval(const baltam::structure &S, const std::vector<double> &X)
    {
        std::vector<double> res(X);
        for (size_t i = 0; i < res.size(); ++i)
        {
            res[i] = fnval(S, X[i]);
        }
        return res;
    }

    // 第一个输入参数是pp样条结构体的情形
    void fnval_pp_case1(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args);

    // 第二个输入参数是pp样条结构体的情形
    void fnval_pp_case2(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args);

    // 第一个输入参数是B样条结构体的情形
    void fnval_B_case1(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args);

    // 第二个输入参数是B样条结构体的情形
    void fnval_B_case2(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args);

    // 函数体
    void proto_fnval_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args)
    {
        // 检查参数个数，输入参数个数区间为[2,2]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 2, 1, 1)
        if (in_args[0]->is_struct() && !in_args[1]->is_struct())
        {
            auto s = in_args[0]->get<structure>();
            if (!is_legal_spline_structure(*s)) // 此处已经完成结构体合法性的判断
                throw std::invalid_argument{"输入结构体不合法。"};
            if (s->get_field("form")->as_string() == "pp")
                fnval_pp_case1(in_args, out_args);
            else
                fnval_B_case1(in_args, out_args);
        }
        else if (!in_args[0]->is_struct() && in_args[1]->is_struct())
        {
            auto s = in_args[1]->get<structure>();
            if (!is_legal_spline_structure(*s)) // 此处已经完成结构体合法性的判断
                throw std::invalid_argument{"输入结构体不合法。"};
            if (s->get_field("form")->as_string() == "pp")
                fnval_pp_case2(in_args, out_args);
            else
                fnval_B_case2(in_args, out_args);
        }
        else
            throw std::invalid_argument{"输入参数错误，仅接受输入样条结构体与横坐标向量。"};
    } // end of proto_fnval_baltam

    // 第一个输入参数是pp样条结构体的情形
    void fnval_pp_case1(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args)
    {
        auto S = in_args[0]->get<structure>();
        // 检查第二个参数是否为double
        const_ba_obj_rawptr xTemp = in_args[1];
        const_ba_obj_ptr pX_smart_ptr;
        spline_parameter_check_double(in_args[1], xTemp, pX_smart_ptr);
        auto pX = xTemp->get<baltam::matrix<double>>();
        std::vector<double> X;
        for (int i = 0; i < pX->cols(); ++i)
        {
            X.push_back((*pX)(0, i));
        }
        auto Y = fnval(*S, X);
        auto res = new baltam::matrix<double>(1, pX->cols());
        for (int i = 0; i < res->cols(); ++i)
        {
            (*res)(0, i) = Y[i];
        }
        *out_args[0] = ba_obj(ba_double_mat, res);
    }

    // 第二个输入参数是pp样条结构体的情形
    void fnval_pp_case2(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args)
    {
        auto temp = in_args;
        std::reverse(temp.begin(), temp.end());
        fnval_pp_case1(temp, out_args);
    }

    // 第一个输入参数是B样条结构体的情形
    void fnval_B_case1(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args)
    {
        auto S = in_args[0]->get<structure>();
        // 检查第二个参数是否为double
        const_ba_obj_rawptr xTemp = in_args[1];
        const_ba_obj_ptr pX_smart_ptr;
        spline_parameter_check_double(in_args[1], xTemp, pX_smart_ptr);
        auto pX = xTemp->get<baltam::matrix<double>>();
        std::vector<double> X;
        for (int i = 0; i < pX->cols(); ++i)
        {
            X.push_back((*pX)(0, i));
        }
        auto Y = fnval(*S, X);
        auto res = new baltam::matrix<double>(1, pX->cols());
        for (int i = 0; i < res->cols(); ++i)
        {
            (*res)(0, i) = Y[i];
        }
        *out_args[0] = ba_obj(ba_double_mat, res);
    }

    // 第二个输入参数是B样条结构体的情形
    void fnval_B_case2(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args)
    {
        auto temp = in_args;
        std::reverse(temp.begin(), temp.end());
        fnval_B_case1(temp, out_args);
    }

} // end of baltam::splines

namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(fnval)

    REGISTER_EXPORT_FUNCTION(fnval, fnval, g_pFnvalHelp)
}
