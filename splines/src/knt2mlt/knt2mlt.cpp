/*
 * ==========================================================================
 *
 *       FileName:  knt2mlt.cpp
 *
 *    Description:  Source file for the knt2mlt function.
 *
 *        Version:  1.2
 *        Created:  2024.08.27
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Bianchenhao
 *      Copyright:
 *
 * ==========================================================================
 */
#include "knt2mlt.h"
#include <vector>
#include <algorithm> // For sort
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pKnt2mltHelp = R"(
knt2mlt(T) 函数返回结点多重性向量M。具体来说,
M(i) = # { j<i : T(j) = T(i) },  i=1:length(T),
如果输入未排序，先对T进行排序。
[m,t] = knt2mlt(T) 还会返回排序后的结点序列。
例如，[m,t] = knt2mlt([1 2 3 3 1 3]) 返回
[0 1 0 0 1 2] 作为m，和 [1 1 2 3 3 3] 作为t。
)";

namespace baltam::splines
{

    // 计算结点重数的函数
    void compute_multiplicities(std::vector<const_ba_obj_rawptr> &in_args, std::vector<double> &M)
    {
        auto pT = in_args[0]->get<matrix<double>>();
        if (!pT || pT->cols() == 0)
        {
            // 输入矩阵为空或列数为0时的处理
            M.resize(1, 0); // 创建一个1x1的矩阵，值为0
        }
        else
        {
            // 将输入矩阵转换为向量
            std::vector<double> T(pT->cols(), 0);
            for (int i = 0; i < pT->cols(); ++i)
            {
                T[i] = (*pT)(0, i); // 假设结点在第一行
            }
            std::sort(T.begin(), T.end());

            // 计算结点重数
            M.resize(T.size(), 0);
            for (size_t i = 0; i < T.size(); ++i)
            {
                M[i] = (i == 0 ? 0 : (T[i] == T[i - 1] ? M[i - 1] + 1 : 0));
            }
        }
    }

    // 返回一个只包含结点重数的函数
    void return_only_multiplicities(std::vector<ba_obj_rawptr> &out_args, const std::vector<double> &M)
    {
        auto *pM = new baltam::matrix<double>(1, M.size());
        for (size_t i = 0; i < M.size(); ++i)
        {
            (*pM)(0, i) = M[i];
        }
        *out_args[0] = ba_obj(ba_double_mat, pM);
    }

    // 返回结点重数和排序后的结点序列的函数
    void return_multiplicities_and_sorted_t(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args, const std::vector<double> &M)
    {
        auto pX = in_args[0]->get<matrix<double>>();
        // 将输入矩阵转换为向量
        std::vector<double> T(pX->cols(), 0);
        for (int i = 0; i < pX->cols(); ++i)
        {
            T[i] = (*pX)(0, i);
        }
        if (pX->cols() == 0)
        {
            // 输入矩阵为空时的处理
            T.resize(1, 0); // 创建一个1x1的矩阵，值为0
            auto pT = new baltam::matrix<double>(1, 1);
            auto pM = new baltam::matrix<double>(1, 1);
            (*pT)(0, 0) = 0;
            (*pM)(0, 0) = 0;
            *out_args[0] = ba_obj(ba_double_mat, pM);
            *out_args[1] = ba_obj(ba_double_mat, pT);
        }
        else
        {
            std::sort(T.begin(), T.end());

            auto pM = new baltam::matrix<double>(1, M.size());
            for (size_t i = 0; i < M.size(); ++i)
            {
                (*pM)(0, i) = M[i];
            }
            *out_args[0] = ba_obj(ba_double_mat, pM);

            auto pT = new baltam::matrix<double>(1, T.size());
            for (size_t i = 0; i < T.size(); ++i)
            {
                (*pT)(0, i) = T[i];
            }
            *out_args[1] = ba_obj(ba_double_mat, pT);
        }
    }

    // 主函数实现
    void proto_knt2mlt_baltam(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args)
    {
        BALTAM_PARAM_CHECK(1, 1, 1, 2) // 检查输入参数和输出参数的数量
                                       // 计算结点重数
        std::vector<double> M;
        compute_multiplicities(in_args, M);
        
        if (out_args.size() == 1)
        {
            // 如果输出参数为1，只返回结点重数
            return_only_multiplicities(out_args, M);
        }
        else if (out_args.size() == 2)
        {
            // 如果输出参数为2，返回结点重数和排序后的结点序列
            return_multiplicities_and_sorted_t(in_args, out_args, M);
        }
        else
        {
            throw std::invalid_argument("输出参数数量不符合预期");
        }
    }

} // namespace baltam::splines

namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(knt2mlt)
    REGISTER_EXPORT_FUNCTION(knt2mlt, knt2mlt, g_pKnt2mltHelp)
}
