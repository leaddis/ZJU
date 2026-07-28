/*
 * ==========================================================================
 *
 *       FileName:  csapi.cpp
 *
 *    Description:  source file for csapi
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

#include "csapi.h"
#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pCsapiHelp = R"(
csapi 创建一个 ppForm 的三次样条曲线，其边界条件采用 not-a-knot 条件。目前仅支持一维情形。

    s = csapi(x,y) 创建一个三次样条曲线，x，y 为插值条件，x，y 必须为长度相同的一维向量。
    这里要求 x 已经完成单增排列。

    s 的结果是满足插值条件与 not-a-knot 边界条件的三次样条曲线，为 pp 格式。

    values = csapi(x,y,xx) 返回生成样条在点 xx 处的值，x，y 为插值条件，x，y 必须为长
    度相同的一维向量。这里要求 x 已经完成单增排列。xx 为一维向量，是所要求点的 x 坐标。

    values 的结果是所要求点处的值，为一维向量。
)";

namespace baltam::splines {
    //一个用于构造插值条件的函数
    void csapi_construct_InterpCondition(InterpCondition &c,
                                         const_ba_obj_rawptr &x,
                                         const_ba_obj_rawptr &y);

    // 输入参数为2个的情形 pp = csapi(x,y)
    void csapi_case1(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args);

    // 输入参数为3个的情形 values = csapi(x,y,xx)
    void csapi_case2(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args);

    // 函数体
    void proto_csapi_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        // 检查参数个数，输入参数个数区间为[2,3]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 3, 1, 1)

        // 根据输入参数的个数选择执行不同的函数
        if (in_args.size() == 2)
            csapi_case1(in_args, out_args);
        else // 因为已经做过参数个数检查，所以此处不是2就是3
            csapi_case2(in_args, out_args);

    }

    //插值条件构造函数
    void csapi_construct_InterpCondition(InterpCondition &c,
                                         const_ba_obj_rawptr &x,
                                         const_ba_obj_rawptr &y) {
        try {
            // 获得指向输入数据的指针 pX，pY，并且要求其有相同的列数
            auto pX = x->get<matrix<double>>();
            auto pY = y->get<matrix<double>>();
            if (pX->cols() != pY->cols()) {
                throw std::invalid_argument("x 和 y 必须具备相同的列数。");
            }
            c.left = BCType::not_a_knot;                         // 设置边界条件为 not-a-knot
            c.right = BCType::not_a_knot;                        // csapi 默认两侧均为 not-a-knot 边界条件
            for (int i = 0; i < pX->size(); ++i) {
                c.sites.push_back((*pX)[i]);                     // 将 pX 数据放入插值条件的横坐标中
                c.function_values.push_back((*pY)[i]);           // 将 pY 数据放入插值条件的纵坐标(函数值)中
            }
        }
        catch (std::invalid_argument &e) {
            throw e;
        }
    }

    // 输入参数为2个的情形 pp = csapi(x,y)
    void csapi_case1(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr xTemp = in_args[0];
        const_ba_obj_rawptr yTemp = in_args[1];
        const_ba_obj_ptr pX_smart_ptr, pY_smart_ptr;
        spline_parameter_check_double(in_args[0], xTemp, pX_smart_ptr);
        spline_parameter_check_double(in_args[1], yTemp, pY_smart_ptr);
        try {
            InterpCondition c;                                   // 样条插值的插值条件类
            csapi_construct_InterpCondition(c, xTemp, yTemp);
            // 根据输入参数 pX,pY 建立样条函数 s
            Spline<3, SplineType::ppForm> s{c};                  // 以插值条件 c 建立样条函数 s
            *out_args[0] = ba_obj(ba_struct, s.spline_to_baltam_structure());//将样条函数 s 转换为 baltam 结构体并存储在 out_args 的第一个位置。s.spline_to_baltam_structure() 将样条函数转换为适合 baltam 使用的格式。
        }
        catch (std::invalid_argument &e) {
            throw e;
        }
    }

    // 输入参数为3个的情形 values = csapi(x,y,xx)
    void csapi_case2(std::vector<const_ba_obj_rawptr> &in_args,
                     std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr xTemp = in_args[0];
        const_ba_obj_rawptr yTemp = in_args[1];
        const_ba_obj_rawptr xxTemp = in_args[2];
        const_ba_obj_ptr pX_smart_ptr, pY_smart_ptr, pXX_smart_ptr;
        spline_parameter_check_double(in_args[0], xTemp, pX_smart_ptr);
        spline_parameter_check_double(in_args[1], yTemp, pY_smart_ptr);
        spline_parameter_check_double(in_args[2], xxTemp, pXX_smart_ptr);

        try {
            InterpCondition c;                                   // 样条插值的插值条件类
            csapi_construct_InterpCondition(c, xTemp, yTemp);
            // 根据输入参数 pX,pY 建立样条函数 s
            Spline<3, SplineType::ppForm> s{c};                  // 以插值条件 c 建立样条函数 s

            // 获得指向第三个输入数据的指针 pXX, 为所要求点值的 x 坐标
            auto pXX = xxTemp->get<matrix<double>>();
            auto *result = new baltam::matrix<double>(1, pXX->size());     // 指向输出结果的指针
            for (int i = 0; i < pXX->size(); i++) {
                (*result)(0, i) = s((*pXX)[i]);
            }
            *out_args[0] = ba_obj(ba_double_mat, result);
        }
        catch (std::invalid_argument &e) {
            throw e;
        }
    }


}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(csapi)

    REGISTER_EXPORT_FUNCTION(csapi, csapi, g_pCsapiHelp)
}
