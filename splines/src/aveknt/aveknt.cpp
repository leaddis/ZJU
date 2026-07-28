/*
 * ==========================================================================
 *
 *       FileName:  aveknt.cpp
 *
 *    Description:  Source file for the aveknt function.
 *
 *        Version:  1.0
 *        Created:  2024.08.28
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  QianZhouyue
 *      Copyright:
 *
 * ==========================================================================
 */
#include <vector>
#include <algorithm> // For std::sort
#include "aveknt.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pAvekntHelp = R"(
函数aveknt的基本用法："
tstar = aveknt(knots,order)其中，knots是一个待处理节点向量，order是阶数，tstar是返回通过阶数和节点t通过平均求和得到的新的节点向量。
t和k的关系是：k<=length(t)，特别的在k=length(t)时，tstar是空向量。
)";

namespace baltam::splines
{
    void proto_aveknt_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                             std::vector<ba_obj_rawptr> &out_args)
    {
        //检查参数个数，输入参数个数区间为[2,2]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 2, 1, 1)
        const_ba_obj_rawptr temp_knots = in_args[0];
        const_ba_obj_rawptr temp_order = in_args[1];
        const_ba_obj_ptr pKnots_smart_ptr, pOrder_smart_ptr;
        spline_parameter_check_double(in_args[0], temp_knots, pKnots_smart_ptr);
        int order = 0;
        try
        {
            order = temp_order->as_int();
            if (order <= 1)
            {
                throw std::invalid_argument("阶数必须是大于1的整数");
            }
        }
        catch (std::invalid_argument &e)
        {
            throw e;
        }
        /*
        catch (std::invalid_argument &)
        {
            throw std::invalid_argument("阶数必须是大于1的整数");
        }
        */
        std::vector<double> knots;
        auto pKnots = in_args[0]->get<matrix<double>>();
        //
        for (int i = 0; i < pKnots->size(); i++)
        {
            knots.push_back((*pKnots)[i]);
        }
        try {
            if (knots.size() < 1) {
                throw std::invalid_argument("输入向量不能是空向量");
            }
        }
        catch (std::invalid_argument &e) {
            throw e;
        }
        int n = knots.size();
        std::vector<double> tstar;
        tstar.reserve(n - order); // 为tstar分配空间
        // 检查k是否有效
        if (order <= 1 || static_cast<size_t>(order) > n)
        {
            throw std::invalid_argument("阶数必须大于1且不超过结点向量的长度");
        }
        // order = length(t)时，返回空向量
        else if (order == n)
        {
        }
        // 计算平均向量
        else
        {
            for (int i = 0; i < n - order; i++)
            {
                double sum = 0;
                for (int j = i + 1; j < i + order; j++)
                {
                    sum += knots[j];
                }
                tstar.push_back((sum / (order-1)));
            }
        }

        // 将tstar转换为baltam结构体
        auto *result = new baltam::matrix<double>(1, tstar.size());
        for (int i = 0; i < tstar.size(); i++)
        {
            (*result)(0, i) = tstar[i];
        }
        *out_args[0] = ba_obj(ba_double_mat, result);
    }

} // namespace baltam::splines

namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(aveknt)
    REGISTER_EXPORT_FUNCTION(aveknt, aveknt, g_pAvekntHelp)
}
