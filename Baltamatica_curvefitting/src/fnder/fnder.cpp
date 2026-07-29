/*
 * ==========================================================================
 *
 *       FileName:  fnder.cpp
 *
 *    Description:  source file for fnder
 *
 *        Version:  1.0
 *        Created:  2023.4.9
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  CaoShaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */


#include "fnder.h"
#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/structure.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>
#include "splines_common.h"

static const char *g_pFnderHelp = R"(
fnder 计算一个样条函数的导函数,并仍以样条函数的形式返回。

    ds = fnder(s,order) 得到样条函数 s 的 order 阶导函数,s 可以是 pp 格式、B 格式。

    order 是一个不超过样条函数次数的正整数,阶数为负值时,返回其绝对值阶的不定积分。

    该函数输出的形式与输入相同,仍以样条函数的格式返回。

    fnder(s) 同 fnder(s,1)。
)";

namespace baltam::splines {


    // pp格式，一个参数，第二个参数默认为1，求一个样条的导数
    void fnder_pp_case1(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args);

    // pp格式，两个参数，第二个参数用户指定，正数为求导，负数为求积分
    void fnder_pp_case2(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args);

    // B格式，一个参数，第二个参数默认为1，求一个样条的导数
    void fnder_B_case1(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args);

    // B格式，两个参数，第二个参数用户指定，正数为求导，负数为求积分
    void fnder_B_case2(std::vector<const_ba_obj_rawptr> &in_args,
                        std::vector<ba_obj_rawptr> &out_args);


    // 实现函数，对一个样条结构体求导或者求积分，返回一个样条结构体
    // baltam::structure *fnder_pp_implement(const baltam::structure &S, int degree);
    // baltam::structure *fnder_B_implement(const baltam::structure &S, int degree);
    // baltam::structure fnder_B_implement_diff(const baltam::structure &S); // 这里的返回值类型不同其他
    // baltam::structure *fnder_B_implement_diff(const baltam::structure &S, int degree);
    // baltam::structure *fnder_B_implement_integrate(const baltam::structure &S, int degree);

    //==================================================================================
    // fnder 函数体
    void proto_fnder_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        BALTAM_PARAM_CHECK(1, 2, 1, 1)
        if (!in_args[0]->is_struct()) // 第一个参数应为一个样条结构体
            throw std::invalid_argument{"输入参数错误，请输入样条结构体。"};
        auto S = in_args[0]->get<baltam::structure>();
        if (!is_legal_spline_structure(*S))
            throw std::invalid_argument{"不合法的样条结构体。"};
        if(S->get_field("form")->as_string() == "pp") {
            if (in_args.size() == 1)
                fnder_pp_case1(in_args, out_args);
            else
                fnder_pp_case2(in_args, out_args);
        }
        else if(S->get_field("form")->as_string() == "B-")
        {
            if (in_args.size() == 1)
                fnder_B_case1(in_args, out_args);
            else
                fnder_B_case2(in_args, out_args);
        }
        else
            throw std::invalid_argument{"不合法的样条结构体。"};
    }
    //===================================================================================

    void fnder_pp_case1(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args) {
        auto pS = in_args[0]->get<structure>();
        auto res = fnder_pp_implement(*pS, 1);
        *out_args[0] = ba_obj(ba_struct, res);
    }

    void fnder_pp_case2(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args) {
        auto pS = in_args[0]->get<structure>();
        if (!in_args[1]->is_integer() && in_args[1]->type() != ba_type::ba_double_mat)
            throw std::invalid_argument{"求导（积分）次数必须为整数。"};
        int degree = in_args[1]->as_int();
        auto res = fnder_pp_implement(*pS, degree);
        *out_args[0] = ba_obj(ba_struct, res);
    } // end of fnder_case2

    // B格式，一个参数，第二个参数默认为1，求一个样条的导数
    void fnder_B_case1(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args)
    {
        auto pS = in_args[0]->get<structure>();
        auto res = fnder_B_implement(*pS, 1);
        *out_args[0] = ba_obj(ba_struct, res);
    }

    // B格式，两个参数，第二个参数用户指定，正数为求导，负数为求积分
    void fnder_B_case2(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args)
    {
        auto pS = in_args[0]->get<structure>();
        if (!in_args[1]->is_integer() && in_args[1]->type() != ba_type::ba_double_mat)
            throw std::invalid_argument{"求导（积分）次数必须为整数。"};
        int degree = in_args[1]->as_int();
        auto res = fnder_B_implement(*pS, degree);
        *out_args[0] = ba_obj(ba_struct, res);
    }

    baltam::structure *fnder_B_implement(const baltam::structure &S, int degree)
    {
        // 如果阶数为 0 ，直接返回输入的样条结构体（什么都不做）
        if (degree == 0)
            return new baltam::structure(S);
        // 取出输入的结构体的阶数（三次样条为4）
        auto d_order = S.get_field("order")->as_int();
        // 不允许求导次数大于样条函数阶数（三次样条最多求导四次）
        if (degree > d_order)
            throw std::invalid_argument{"求导次数不能大于样条函数阶数。"};
        if(degree > 0)
            return fnder_B_implement_diff(S,degree);
        else
            return fnder_B_implement_integrate(S,degree);
    }
    void fnder_B_implement_diff(baltam::structure &S)
    {
        auto Knots = S.get_field("knots")->get<matrix<double>>();
        auto Coefs = S.get_field("coefs")->get<matrix<double>>();
        auto Order = S.get_field("order")->as_int();
        // 计算函数说明文档中的 alpha,beta
        int n = Order - 1;
        std::vector<double> alpha;
        std::vector<double> beta;
        for (int i = 1; i <= static_cast<int>(Knots->size() - Order); ++i) {
            double temp = (*Knots)(0,i + n -1) - (*Knots)(0,i-1);
            if(temp == 0)
                alpha.push_back(0);
            else
                alpha.push_back(n / temp);

            temp = (*Knots)(0,i + n ) - (*Knots)(0,i);
            if(temp == 0)
                beta.push_back(0);
            else
                beta.push_back(n / temp);
        }
        // 计算求导数之后重新组合过后的系数向量
        std::vector<double> d_Coefs(Coefs->size()+1);
        d_Coefs[0] = (*Coefs)(0) * alpha[0];
        d_Coefs.back() = - (*Coefs)(Coefs->size()-1) * beta.back();
        for (int i = 1; i < static_cast<int>(d_Coefs.size()) - 1; ++i) {
            d_Coefs[i] = (*Coefs)(i) * alpha[i] - (*Coefs)(i-1) * beta[i-1];
        }
        std::vector<double> d_Knots(Knots->size());
        for (size_t i = 0; i < d_Knots.size(); ++i) {
            d_Knots[i] = (*Knots)(i);
        }
        // 如果求导之后第一个基函数的结点都相同，删掉第一个
        if((*Knots)(0) == (*Knots)(n))
        {
            d_Coefs = std::vector<double>(d_Coefs.begin() +1,d_Coefs.end());
            d_Knots = std::vector<double>(d_Knots.begin()+1,d_Knots.end());
        }
        // 如果求导之后最后一个基函数的结点都相同，删掉最后一个
        if((*Knots)(Knots->size()-1) == (*Knots)(Knots->size()-1 -n))
        {
            d_Coefs = std::vector<double>(d_Coefs.begin(),d_Coefs.end()-1);
            d_Knots = std::vector<double>(d_Knots.begin(),d_Knots.end()-1);
        }
        // 根据计算结果对返回值进行装配
        auto res_Coefs = new baltam::matrix<double>(1,d_Coefs.size());
        auto res_Knots = new baltam::matrix<double>(1,d_Knots.size());
        for (size_t i = 0; i < d_Coefs.size(); ++i) {
            (*res_Coefs)[i] = d_Coefs[i];
        }
        for (size_t i = 0; i < d_Knots.size(); ++i) {
            (*res_Knots)[i] = d_Knots[i];
        }
        S.set_field("number",new ba_obj(static_cast<int>(d_Coefs.size())));
        S.set_field("order", new ba_obj(static_cast<int>(Order -1)));
        S.set_field("coefs",new ba_obj(ba_double_mat,res_Coefs));
        S.set_field("knots", new ba_obj(ba_double_mat,res_Knots));
    }
    baltam::structure *fnder_B_implement_diff(const baltam::structure &S, int degree)
    {
        auto res = new baltam::structure(S);
        for (int i = 0; i < degree; ++i) {
           fnder_B_implement_diff(*res);
        }
        return res;
    }

    void fnder_B_implement_integrate(baltam::structure &S)
    {
        auto Knots = S.get_field("knots")->get<matrix<double>>();
        auto Coefs = S.get_field("coefs")->get<matrix<double>>();
        auto Order = S.get_field("order")->as_int();

        //压缩spline
        // todo

        //计算右端重节点数
        double LastKnot = (*Knots)(0,Knots->cols() - 1);
        int multiNum = 0;
        for (int i = Knots->cols() - 1; i >=0; --i) {
            if ((*Knots)(0,i) != LastKnot) {
                break;
            }
            ++multiNum;
        }

        //构建原函数所需节点
        auto res_Knots = new baltam::matrix<double>(1,Knots->cols() + Order+1 - multiNum);
        for (auto i = 0; i < Knots->cols(); ++i) {
            (*res_Knots)(0,i) = (*Knots)(0,i);
        }
        for (auto i = Knots->cols(); i < (*res_Knots).cols(); ++i) {
            (*res_Knots)(0,i) = LastKnot;
        }
        //Gauss法解原函数系数（算法已简化）
        auto res_Coefs = new baltam::matrix<double>(1,(*res_Knots).cols() - (Order+1));
        for (int i = 1; i <= res_Coefs->cols(); ++i) {
            double temp = (*res_Knots)(0,i+Order-1) - (*res_Knots)(0,i-1);
            //要求所有基函数无关，即不能出现零函数
            if(temp == 0){
                throw std::invalid_argument{"重节点数量过多。"};
            }
            //Gauss
            if (i == 1) {
                (*res_Coefs)(0,i-1) = (*Coefs)(0,i-1)* temp/Order; // x1 = y1 / a1
            }
            else if (i <= Coefs->cols()){
                (*res_Coefs)(0,i-1) = (*Coefs)(0,i-1)* temp/Order + (*res_Coefs)(0,i-2); // xi = yi/ai + xi-1
            }
            else {
                (*res_Coefs)(0,i-1) = (*res_Coefs)(0,i-2); // xi = 0/ai + xi-1
            }
        }
        S.set_field("number" , new ba_obj(static_cast<int>(res_Coefs->cols())));
        S.set_field("order" , new ba_obj(static_cast<int>(Order + 1)));
        S.set_field("coefs" , new ba_obj(ba_double_mat,res_Coefs));
        S.set_field("knots" , new ba_obj(ba_double_mat,res_Knots));
    }

    baltam::structure *fnder_B_implement_integrate(const baltam::structure &S, int degree)
    {
        auto res = new baltam::structure(S);
        for (int i = 0; i > degree; --i) {
             fnder_B_implement_diff(*res);
        }
        return res;
    }

    baltam::structure *fnder_pp_implement(const baltam::structure &S, int degree) {
        // 如果阶数为 0 ，直接返回输入的样条结构体（什么都不做）
        if (degree == 0)
            return new baltam::structure(S);
        // 取出输入的结构体的阶数（三次样条为4）
        auto d_order = S.get_field("order")->as_int();
        // 不允许求导次数大于样条函数阶数（三次样条最多求导四次）
        if (degree > d_order)
            throw std::invalid_argument{"求导次数不能大于样条函数阶数。"};
        // 取出输入的样条结构体的系数矩阵
        auto Coefs = (S.get_field("coefs"))->get<baltam::matrix<double>>();
        // 取出输入的样条结构体的节点向量
        auto d_breaks = S.get_field("breaks")->get<matrix<double>>();
        // 构建出分片多项式，然后根据阶进行求导或者求积分
        std::vector<Polynomial> P{};
        for (int i = 0; i < Coefs->rows(); ++i) {
            std::vector<double> temp;
            // 取出Coefs的第 i 行存到一个 std::vector<double> 中用来构建多项式
            for (int j = 0; j < Coefs->cols(); ++j) {
                temp.push_back((*Coefs)(i, j));
            }
            // 详见多项式类的构造函数说明
            P.push_back(Polynomial((*d_breaks)(0, i), temp, false));
        }
        if (degree > 0) { // 阶数大于零为求导数
            for (int i = 0; i < static_cast<int>(P.size()); ++i) {
                P[i] = P[i].Diff(degree); // 每个分片多项式求 degree 阶导数
            }
        } else {  // 阶数小于零为求积分
            for (int i = 0; i < static_cast<int>(P.size()); ++i) {
                P[i] = P[i].Integrate(-degree); // 每个分片多项式求 degree 阶导数
            }
        }
        // 导函数的分片多项式系数矩阵
        auto d_coefs = new baltam::matrix<double>(Coefs->rows(), P[0].deg() + 1);
        for (int i = 0; i < d_coefs->rows(); ++i) {
            // 将多项式转化为在节点处泰勒展开的形式
            P[i] = P[i].reformulate((*d_breaks)[i]);
            for (int j = 0; j < d_coefs->cols(); ++j) {
                (*d_coefs)(i, j) = P[i][d_coefs->cols() - j - 1]; // 对结果的系数矩阵进行赋值
             }
        }
        if(degree < 0)  // 积分常数显式设为0
        {
            for (int i = 0; i < d_coefs->rows(); ++i) {
                for (int j = 0; j < -degree; ++j) {
                    (*d_coefs)(i,d_coefs->cols() - j - 1) = 0;
                }
            }
        }
        auto *res = new baltam::structure();

        auto d_form = S.get_field("form")->as_string();
        res->set_field("form", new ba_obj(d_form));

        auto d_breaksTemp = new matrix<double>(*d_breaks); // 结果不能与输入共享数据，否则闪退
        res->set_field("breaks", new ba_obj(ba_double_mat, d_breaksTemp));

        res->set_field("coefs", new ba_obj(ba_double_mat, d_coefs));

        auto d_pieces = S.get_field("pieces")->as_int();
        res->set_field("pieces", new ba_obj(static_cast<int64_t>(d_pieces)));

        res->set_field("order", new ba_obj(static_cast<int64_t>(d_order - degree)));

        auto d_dim = S.get_field("dim")->as_int();
        res->set_field("dim", new ba_obj(static_cast<int64_t>(d_dim)));

        return res;

    } //end of fnder_implement
} // end of namespace baltam::splines


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(fnder)

    REGISTER_EXPORT_FUNCTION(fnder, fnder, g_pFnderHelp)
}


