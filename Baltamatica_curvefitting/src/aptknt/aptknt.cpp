/*
* ==========================================================================
 *
 *       FileName:  aptknt.cpp
 *
 *    Description:  source file for aptknt
 *
 *        Version:  1.0
 *        Created:  2024.8.28
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author: czx
 *      Copyright:
 *
 * ==========================================================================
 */
#include "aptknt.h"
#include <vector>
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>
#include <augknt/augknt.h>

static const char *g_pAptkntHelp = R"(
aptknt 根据给出的数据位点以及样条阶数生成合适的B样条基函数节点序列 (tau为插值数据点，k为样条阶数)

    knots = aptknt(tau, k)
    tau 至少有 k 项且单增，并满足 tau(i) < tau(i+k-1)

    [knots, k] = aptknt(tau, k) 若tau少于k项，k将减小为length(tau), 并返回实际使用的k
)";

namespace baltam::splines {
    // 函数体
    void proto_aptknt_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        // 检查参数个数，输入参数个数区间为[2,3]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 2, 1, 2)
        const_ba_obj_rawptr tempKnots = in_args[0];
        const_ba_obj_rawptr tempK = in_args[1];
        const_ba_obj_ptr pa_smart_ptr;
        spline_parameter_check_double(in_args[0],tempKnots,pa_smart_ptr);
        int k = 0;
        try {
            k= tempK->as_int(); // 不是 int 类型将会抛出异常
            if( k < 2 ) throw std::invalid_argument("第二个参数必须大于1。");
        }
        catch (std::invalid_argument &) {
            throw std::invalid_argument{"第二个参数必须为大于1的正整数。"};
        }
        //参数类型检查

        auto pKnots = in_args[0]->get<matrix<double>>();
        if( pKnots->size() < k ) k = pKnots->size();
        int i = 0;
        for( ; i < pKnots->size()-k+1; i++ ) {
            if( (*pKnots)[i] > (*pKnots)[i+1] || (*pKnots)[i] >= (*pKnots)[i+k-1] ) throw std::invalid_argument{"输入数据点不合法。"};
        }
        for( ; i < pKnots->size()-2; i++ ) {
            if( (*pKnots)[i] > (*pKnots)[i+1] ) throw std::invalid_argument{"输入数据点不合法。"};
        }
        //检测输入的数据点是否合法

        auto *knotsStar = new baltam::matrix<double>(1, 2+pKnots->size()-k );
        (*knotsStar)(0,0) = (*pKnots)[0];
        (*knotsStar)(0,pKnots->size()-k+1) = (*pKnots)[pKnots->size()-1];
        double tempSum = 0;
        for( int j = 1; j < k; j++ ) tempSum += (*pKnots)[j];
        for( int i = 1; i < pKnots->size()-k+1; i++ ) {
            (*knotsStar)(0,i) = tempSum/(k-1);
            tempSum = tempSum - (*pKnots)[i] + (*pKnots)[i+k-1];
        }

        auto pAveKnots = ba_obj(ba_double_mat, knotsStar);
        std::vector<const_ba_obj_rawptr> tempInArgs{ &pAveKnots, in_args[1] };

        proto_augknt_baltam(tempInArgs, out_args);
        if( out_args.size() == 2 ) *out_args[1] = ba_obj(static_cast<int>(k));
    }
}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(aptknt)

    REGISTER_EXPORT_FUNCTION(aptknt, aptknt, g_pAptkntHelp)
}
