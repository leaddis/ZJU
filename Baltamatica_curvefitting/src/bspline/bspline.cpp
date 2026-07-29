/*
 * ==========================================================================
 *
 *       FileName:  bspline.h
 *
 *    Description:  header for bspline
 *
 *        Version:  1.0
 *        Created:  2022.12.15
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#include "bspline.h"
#include <vector>
#include <algorithm>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>
static const char *g_pBsplineHelp = R"(
bspine 根据输入节点创建一个 B-Spline 基函数，并返回其 pp 格式的表达

    p = bspline(t), 该函数返回值为一个结构体，表示根据输入节点序列 t 生成的 B-Spline 基函数的 pp 格式。
    节点序列 t 要求满足单增排列，且无重复元素
)";
namespace baltam::splines
{
    void bspline_case(std::vector<const_ba_obj_rawptr> &in_args,
                      std::vector<ba_obj_rawptr> &out_args)
    {
        const_ba_obj_rawptr xTemp = in_args[0];
        const_ba_obj_ptr pX_smart_ptr;
        spline_parameter_check_double(in_args[0], xTemp, pX_smart_ptr);
        auto p = xTemp->get<matrix<double>>(); // 提取出指向数据的指针
        if (p->rows() > 1)
            throw std::invalid_argument{"输入的节点应为一个单行向量。"};
        const int n = p->cols();   // 列数
        std::vector<double> sites; // 基样条节点序列
        for (int i = 0; i < n; i++)
        {
            sites.push_back((*p)[i]);
        }
        if (sites.size() == 1)
            throw std::invalid_argument("输入的节点数应大于1");
        sort(sites.begin(),sites.end()); // 进行单增排序
        Base_Spline s(sites);
        // 重节点情形
        int num = 0;
        for (int i = 0; i < n - 1; ++i)
        {
            if (sites[i] == sites[i + 1])
                num++;
        }
        auto *res = new baltam::structure();
        // form : pp
        res->set_field("form", new ba_obj("pp", baltam::ba_char_mat));
        // breaks ： knots
        auto *pBreaks = new baltam::matrix<double>(1, n - num);
        int k = 0;
        for (int i = 1; i <= n; ++i)
        {
            if (sites[i - 1] != sites[i])
            {
                k++;
                (*pBreaks)[k - 1] = sites[i - 1];
            }
        }
        res->set_field("breaks", new ba_obj(baltam::ba_double_mat, pBreaks));
        // coefs : coefficients of polynomial of each piece
        auto *pCoefs = new baltam::matrix<double>(sites.size() - 1 - num, n - 1);
        k = 0;
        for (int i = 1; i <= static_cast<int>(sites.size() - 1); ++i)
        {
            if (sites[i - 1] != sites[i])
            {
                k++;
                for (int j = 0; j <= n - 2; ++j)
                {
                    (*pCoefs)(k - 1, j) = s.poly(i - 1).reformulate(sites[i - 1])[n - 2 - j];
                }
            }
        }
        res->set_field("coefs", new ba_obj(baltam::ba_double_mat, pCoefs));
        // order : n-1 ( this is Bspline spline interpolation, why the return value is MATLAB is n-1?)
        res->set_field("order", new ba_obj(n - 1));
        // pieces : number of piecewise polynomials
        res->set_field("pieces", new ba_obj(static_cast<int>(sites.size() - 1 - num)));
        // dim :1 ( one-dimensional function )
        res->set_field("dim", new ba_obj(1));
        *out_args[0] = ba_obj(baltam::ba_struct, res);
    }
    void proto_bspline_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                              std::vector<ba_obj_rawptr> &out_args)
    {
        // 检查参数个数，输入参数区间为[1,1],输出参数区间为[1,1]
        BALTAM_PARAM_CHECK(1, 1, 1, 1)
        bspline_case(in_args, out_args);
    }
}
namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(bspline)

    REGISTER_EXPORT_FUNCTION(bspline, bspline, g_pBsplineHelp)
}
