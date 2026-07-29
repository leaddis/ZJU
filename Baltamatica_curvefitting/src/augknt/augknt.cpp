/*
* ==========================================================================
 *
 *       FileName:  augknt.cpp
 *
 *    Description:  source file for augknt
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
#include "augknt.h"
#include <vector>
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <algorithm>
#include <iostream>

static const char *g_pAugkntHelp = R"(
augknt 会根据所需要的样条阶数扩充输入的节点序列（knots为节点序列，k为样条阶数，multi为内节点重数 ）

    augknt(knots,k) 根据给出的`knots`返回一个单增的扩充节点序列满足只在首尾两个节点有重复，
    重复k次（所以节点序列可能会变短）

    augknt(knots,k,mults) 边界点同 augknt(knots,k) ，内节点会重复 multi 次，并且若输
    入的 multi 为数组并且长度与内节点数量相同那么第 j 个内节点将会重复 multi(j) 次，否则都重
    复 multi(1) 次

    [augknot, addl] = augknt(...) addl为左侧增加的节点个数(因此可以为负数)
)";

namespace baltam::splines {
    
    void augknt_case1(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args);

    void augknt_case2(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args);

    // 函数体
    void proto_augknt_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        // 检查参数个数，输入参数个数区间为[2,3]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 3, 1, 2)
        if( in_args.size() == 2 ) augknt_case1( in_args, out_args );
        else augknt_case2( in_args, out_args );
        // 根据输入参数的个数选择执行不同的函数
    }

    void augknt_case1(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args){
        const_ba_obj_rawptr tempKnots = in_args[0];
        const_ba_obj_rawptr tempK = in_args[1];
        const_ba_obj_ptr pa_smart_ptr;
    	spline_parameter_check_double(in_args[0],tempKnots,pa_smart_ptr);
        int k = 0;
        try {
            k= tempK->as_int(); // 不是 int 类型将会抛出异常
            if( k < 1 ) throw std::invalid_argument("第二个参数必须为正整数。");
        }
        catch (std::invalid_argument &) {
            throw std::invalid_argument{"第二个参数必须为正整数。"};
        }

        auto pKnots = in_args[0]->get<matrix<double>>();
        std::vector<double> knots;
        for( int i = 0; i < pKnots->size(); i++ ) knots.push_back((*pKnots)[i]);
        //把输入的节点转化成vector数组

        std::sort(knots.begin(),knots.end());
        int countBegin;
        if( out_args.size() == 2 ) countBegin = std::count(knots.begin(),knots.end(),knots[0]);
        int n = std::unique(knots.begin(),knots.end()) - knots.begin();
        //数组排序去重
        auto *result = new baltam::matrix<double>(1, 2*k+n-2);   
        int idx = 0;
        for( int i = 0; i < k; i++ ){
            (*result)(0,idx++) = knots[0];
        }

        for( int i = 1; i < n-1; i++ ){
            (*result)(0,idx++) = knots[i];
        }

        for( int i = 0; i < k; i++ ){
            (*result)(0,idx++) = knots[n-1];
        }
        
        *out_args[0] = ba_obj(ba_double_mat, result);
        if( out_args.size() == 2 ) *out_args[1] = ba_obj(static_cast<int>(k-countBegin));   
    
    }

    void augknt_case2(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args){
        const_ba_obj_rawptr tempKnots = in_args[0];
        const_ba_obj_rawptr tempK = in_args[1];
        const_ba_obj_rawptr tempMulti = in_args[2];
        const_ba_obj_ptr pa_smart_ptr, pb_smart_ptr;
    	spline_parameter_check_double(in_args[0],tempKnots,pa_smart_ptr);

        int k = 0;
        try {
            k= tempK->as_int(); // 不是 int 类型将会抛出异常
            if( k < 1 ) throw std::invalid_argument("第二个参数必须为正整数。");
        }
        catch (std::invalid_argument &) {
            throw std::invalid_argument{"第二个参数必须为正整数。"};
        }

        try {
            if (in_args[2]->type() != ba_int_mat) {
                tempMulti = internal::matrix_numeric_cast(in_args[2], ba_int_mat);
                if (tempMulti == nullptr)
                    throw std::invalid_argument{"仅支持整数类型的内节点重数参数输入。"};
                pb_smart_ptr = const_ba_obj_ptr(tempMulti);
            }
        }
        catch (std::invalid_argument & e) {
            throw e;
        }
        //参数类型检查

        auto pKnots = in_args[0]->get<matrix<double>>();
        std::vector<double> knots;
        for( int i = 0; i < pKnots->size(); i++ ) knots.push_back((*pKnots)[i]);
        //把输入的节点转化成vector数组
        auto pMulti = in_args[2]->get<matrix<double>>();
        std::vector<int> multis;
        for( int i = 0; i < pMulti->size(); i++ ) multis.push_back(int((*pMulti)[i]));
        //baltam 输入的参数默认为double，需要先强转成int

        std::sort(knots.begin(),knots.end());
        int countBegin;
        if( out_args.size() == 2 ) countBegin = std::count(knots.begin(),knots.end(),knots[0]);
        int n = std::unique(knots.begin(),knots.end()) - knots.begin();
        //数组排序去重

        int internalKnotsNum = 0;
        if( pMulti->size() == n-2 ){
            for( int i = 0; i < n-2; i++ ) {
                if( multis[i] < 1 ) throw std::invalid_argument{"内节点重数必须为正整数。"};
                internalKnotsNum += multis[i];
            }
        }else internalKnotsNum = (n-2)*multis[0];

        auto *result = new baltam::matrix<double>(1, 2*k+internalKnotsNum);
        int idx = 0;
        for( int i = 0; i < k; i++ ){
            (*result)(0,idx++) = knots[0];
        }

        if( pMulti->size() == n-2 ){
            for( int i = 1; i < n-1; i++ ){
                for( int j = 0; j < multis[i-1]; j++ ) (*result)(0,idx++) = knots[i];
            }
        }else{
            for( int i = 1; i < n-1; i++ ){
                for( int j = 0; j < multis[0]; j++ ) (*result)(0,idx++) = knots[i];
            }
        }


        for( int i = 0; i < k; i++ ){
            (*result)(0,idx++) = knots[n-1];
        }

        *out_args[0] = ba_obj(ba_double_mat, result);
        if( out_args.size() == 2 ) *out_args[1] = ba_obj(static_cast<int>(k-countBegin));
    }

}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(augknt)

    REGISTER_EXPORT_FUNCTION(augknt, augknt, g_pAugkntHelp)
}
