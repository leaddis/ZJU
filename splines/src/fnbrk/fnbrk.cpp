/*
 * ==========================================================================
 *
 *       FileName:  fnbrk.cpp
 *
 *    Description:  source file for fnbrk
 *
 *        Version:  1.0
 *        Created:  2023.5.13
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#include "fnbrk.h"
#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pFnbrkHelp = R"(
fnbrk 用来得到样条函数的相关信息

    [out1,...,outn] = fnbrk(f,part1,...,partm) 要求n小于等于m
    parti 表示第i个输入的样条信息，具体可见函数说明文档
    f 表示输入的样条函数； outi 输出第i个输入的样条信息
)";

namespace baltam::splines
{
    // 参数为矩阵格式
    ba_obj fnbrk_case_double_mat(const_ba_obj_rawptr &in, const baltam::structure &s)
    {
        auto *pCond = in->get<matrix<double>>();
        if (pCond->rows() != 1 || pCond->cols() != 2)
        {
            // 第三个参数是矩阵，但是行数不为1 或者 列数不是1也不是2
            std::string error_message{"第三个参数为矩阵时，其大小必须为 1*2 ，输入参数大小："};
            error_message += std::to_string(pCond->rows());
            error_message += " * ";
            error_message += std::to_string(pCond->cols());
            throw std::invalid_argument{error_message};
        }
        auto res = new baltam::structure();
        if(s.get_field("form")->as_string()=="B-")
        {
            throw std::invalid_argument("暂不支持B样条截取区间,您可先使用fn2fm转换pp样条，再截取区间");
        }
        res->set_field("form", new ba_obj(s.get_field("form")->as_string()));
        res->set_field("order", new ba_obj(static_cast<int64_t>(s.get_field("order")->as_int())));
        res->set_field("dim", new ba_obj(static_cast<int64_t>(s.get_field("dim")->as_int())));
        auto *pbreak = s.get_field("breaks")->get<baltam::matrix<double>>();
        int lidx = -1, ridx = -1;
        if ((*pCond)[0] >= (*pCond)[1])
        {
            throw std::invalid_argument{"截取区间应为递增排列"};
        }
        for (int i = 0; i < pbreak->cols() - 1; ++i)
        {
            if ((*pCond)[0] >= (*pbreak)[i] && (*pCond)[0] < (*pbreak)[i + 1])
            {
                lidx = i;
            }
            if ((*pCond)[1] > (*pbreak)[i] && (*pCond)[1] <= (*pbreak)[i + 1])
            {
                ridx = i + 1;
            }
        }
        if (lidx == -1 || ridx == -1)
        {
            throw std::invalid_argument{"截取区间应在样条区间之间"};
        }
        res->set_field("pieces", new ba_obj(static_cast<int64_t>(ridx - lidx)));
        auto *pnewbreak = new baltam::matrix<double>(1, ridx - lidx+1);
        (*pnewbreak)[0] = (*pCond)[0];
        for (int i = 1; i < ridx - lidx ; ++i)
        {
            (*pnewbreak)[i] = (*pbreak)[lidx+i];
        }
        (*pnewbreak)[ridx-lidx] = (*pCond)[1];
        res->set_field("breaks", new ba_obj(baltam::ba_double_mat, pnewbreak));

        auto *pcoef = s.get_field("coefs")->get<baltam::matrix<double>>(); 
        auto *pnewcoef= new baltam::matrix<double>(ridx-lidx, pcoef->cols());
        vector<double> p0coef;
        for(int i=pcoef->cols()-1;i>=0;--i)
        {
            p0coef.push_back((*pcoef)(lidx,i));
        }
        Polynomial p0(p0coef);//注意p0coef为p0在(*pbreak)[lidx]处展开的系数
        for(int i=0;i<ridx-lidx;++i)
        {
            for(int j=0;j<pcoef->cols();++j)
            {
                if(i==0)
                {
                    (*pnewcoef)(i,j)=p0.reformulate((*pnewbreak)[0]-(*pbreak)[lidx])[pcoef->cols()-j-1];
                }
                else
                    (*pnewcoef)(i,j)=(*pcoef)(i+lidx,j);
            }
        }
        res->set_field("coefs", new ba_obj(baltam::ba_double_mat, pnewcoef));
        return ba_obj(baltam::ba_struct, res);
    }
    void fnbrk_case_pp(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args)
    {
        int n = out_args.size();
        auto s = in_args[0]->get<baltam::structure>();
        for (int i = 0; i < n; ++i)
        {
            if (in_args[i + 1]->type() == ba_double_mat)
                *out_args[i] = fnbrk_case_double_mat(in_args[i + 1], *s);
            else
            {
                std::string str = in_args[i + 1]->as_string();
                if (str[0] == 'f')
                    *out_args[i] = *(s->get_field("form"));
                else if (str[0] == 'v')
                    *out_args[i] = ba_obj(1);
                else if (str[0] == 'd')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("dim")->as_int()));
                else if (str[0] == 'c')
                    *out_args[i] = *(s->get_field("coefs"));
                else if (str[0] == 'i')
                {
                    auto *pi = new baltam::matrix<double>(1, 2);
                    auto *pbreak = s->get_field("breaks")->get<baltam::matrix<double>>();
                    (*pi)(0, 0) = (*pbreak)(0, 0);
                    (*pi)(0, 1) = (*pbreak)(0, pbreak->cols() - 1);
                    *out_args[i] = ba_obj(baltam::ba_double_mat, pi);
                }
                else if (str[0] == 'o')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("order")->as_int()));
                else if (str[0] == 'p')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("pieces")->as_int()));
                else if (str[0] == 'b')
                    *out_args[i] = *(s->get_field("breaks"));
                else
                    throw std::invalid_argument("无效的样条信息输入");
            }
        }
    }
    void fnbrk_case_B(std::vector<const_ba_obj_rawptr> &in_args,
                      std::vector<ba_obj_rawptr> &out_args)
    {
        int n = out_args.size();
        auto s = in_args[0]->get<baltam::structure>();
        for (int i = 0; i < n; ++i)
        {

            if (in_args[i + 1]->type() == ba_double_mat)
                *out_args[i] = fnbrk_case_double_mat(in_args[i + 1], *s);
            else
            {
                std::string str = in_args[i + 1]->as_string();
                if (str[0] == 'f')
                    *out_args[i] = *(s->get_field("form"));
                else if (str[0] == 'v')
                    *out_args[i] = ba_obj(1);
                else if (str[0] == 'd')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("dim")->as_int()));
                else if (str[0] == 'c')
                    *out_args[i] = *(s->get_field("coefs"));
                else if (str[0] == 'i')
                {
                    auto *pi = new baltam::matrix<double>(1, 2);
                    auto *pbreak = s->get_field("knots")->get<baltam::matrix<double>>();
                    (*pi)(0, 0) = (*pbreak)(0, 0);
                    (*pi)(0, 1) = (*pbreak)(0, pbreak->cols() - 1);
                    *out_args[i] = ba_obj(baltam::ba_double_mat, pi);
                }
                else if (str[0] == 'o')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("order")->as_int()));
                else if (str[0] == 'n')
                    *out_args[i] = ba_obj(static_cast<int64_t>(s->get_field("number")->as_int()));
                else if (str[0] == 'k')
                    *out_args[i] = *(s->get_field("knots"));
                else
                    throw std::invalid_argument("无效的样条信息输入");
            }
        }
    }
    void proto_fnbrk_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args)
    {
        int m = in_args.size() - 1;
        int n = out_args.size();
        if (m < 1)
            throw std::invalid_argument("输入参数过少。");
        if (n < 1)
            throw std::invalid_argument("输出参数过少。");
        if (!in_args[0]->is_struct()) // 第一个参数应为一个样条结构体
            throw std::invalid_argument{"输入参数错误，请输入样条结构体。"};
        if (!is_legal_spline_structure(*(in_args[0]->get<baltam::structure>())))
            throw std::invalid_argument{"不合法的样条结构体。"};
        if (n > m)
            throw std::invalid_argument{"输入的参数个数需要大于输出参数个数"};
        auto s = in_args[0]->get<baltam::structure>();
        if (s->get_field("form")->as_string() == "pp")
            fnbrk_case_pp(in_args, out_args);
        else if (s->get_field("form")->as_string() == "B-")
            fnbrk_case_B(in_args, out_args);
    }
}

namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(fnbrk)

    REGISTER_EXPORT_FUNCTION(fnbrk, fnbrk, g_pFnbrkHelp)
}
