/*
 * ============================================================================
 *
 *       Filename:  splines.h
 *
 *    Description:  header for splines
 *
 *        Version:  1.0
 *        Created:  07/28/2022 17:49:46 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Jingang Zhou, jingang.zhou@cqbdri.pku.edu.cn
 *      Copyright:  Copyright (c) 2022, Jingang Zhou
 *
 * ============================================================================
 */

#ifndef BALTAM_PLUGIN_SPLINES_H
#define BALTAM_PLUGIN_SPLINES_H

#include "bex/bex.hpp"
#include "bex/bex.import.hpp"
#include "ba_obj/extern_obj.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "ExportFunction/ExportFunction.h"
#include "ExportFunction/RegisterExportFunction.h"
#include "private/adapter.h"
#include "private/typedefs.h"
#include "private/curve.h"
#include "private/utils.h"
#include <vector>

#define GENERATE_SPLINES_PLUGIN_FUNC(funcName) \
    void funcName(int nlhs, bxArray *plhs[], int nrhs, const bxArray *prhs[]){       \
        splines::adapt(splines::proto_##funcName##_baltam, nlhs, plhs, nrhs, prhs);  \
    }

namespace baltam::splines {

    // 判断一个参数是否为 double 或者可以转化为 double，都不可以抛出异常
#define spline_parameter_check_double(in_arg, temp, smart_ptr)            \
    try{                                                                  \
        if(in_arg->type() != ba_double_mat)                                 \
        {                                                                 \
            temp = internal::matrix_numeric_cast(in_arg,ba_double_mat);   \
            if(temp == nullptr)                                           \
                throw std::invalid_argument{"仅支持实数数值矩阵的输入参数。"}; \
            smart_ptr = const_ba_obj_ptr(temp);                           \
        }                                                                 \
    }                                                                     \
    catch (std::invalid_argument &e) {                                    \
        throw e;                                                          \
    }

    // spapi 和 fn2fm 所需函数 aptknt 的实现
    std::vector<double> aptknt(std::vector<double> knots, int k);

    // aptknt 所需的 aveknt 函数的实现
    std::vector<double> aveknt(std::vector<double> knots, int k);

    // augknt 所需的 augknt 函数的实现
    std::vector<double> augknt(std::vector<double> knots, int k);

    // 判断一个结构体是否为一个合法的样条结构体，pp or B
    bool is_legal_spline_structure(const baltam::structure &s);


}


#endif

