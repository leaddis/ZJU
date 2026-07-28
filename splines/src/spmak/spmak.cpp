/*
 * ==========================================================================
 *
 *       FileName:  spmak.cpp
 *
 *    Description:  source file for spmak
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

#include "spmak.h"
#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pSpmakHelp = R"(
spmak 根据给出的信息创建一个 B 格式的样条曲线。

    sp = spmak(knots,coefs) 根据节点和系数信息创建对应的样条曲线。这里要求 knots 是单增
    非降的一维向量，coefs 是一个一维向量。样条曲线的次数 Order 满足 Order + 1 = length(knots)
    - length(coefs)。sp 返回对应的样条曲线，为 B 格式。
)";

namespace baltam::splines {

    void spmak(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr knotsTemp = in_args[0];
        const_ba_obj_rawptr coefsTemp = in_args[1];
	const_ba_obj_ptr pk_smart_ptr, pc_smart_ptr;
	spline_parameter_check_double(in_args[0],knotsTemp,pk_smart_ptr);
	spline_parameter_check_double(in_args[1],coefsTemp,pc_smart_ptr);

        auto pKnots = knotsTemp->get<matrix<double>>();
        auto pCoefs = coefsTemp->get<matrix<double>>();

        // 对输入参数的合理性进行检查
	if (!pKnots->is_vector()) {
            throw std::invalid_argument("第一个输入参数必须为向量。");
        }
	if (!pCoefs->is_vector()) {
            throw std::invalid_argument("第二个输入参数必须为向量。");
        }
	int t1 = pKnots->cols();
	int t2 = pCoefs->cols();
        if (t1<=t2){
	    throw std::invalid_argument("节点的个数必须大于系数的个数。");
	}
	// 检查 knots 是否单调非降
        for(baIndex i = 0; i<pKnots->size()-1; i++){
	    if((*pKnots)[i]>(*pKnots)[i+1]){
		throw std::invalid_argument("节点需按单调非降顺序排列。");
	    }
	}

	// 去除次数过多的重节点
	int k = static_cast<int>(pKnots->cols()-pCoefs->cols()); // 多项式的阶数
	std::vector<double> tmp1; // 存储节点
	std::vector<double> tmp2; // 存储系数
	for(int i = 0; i<pCoefs->size(); i++){
	    if((*pKnots)[i]!=(*pKnots)[i+k]){
		tmp1.push_back((*pKnots)[i]);
		tmp2.push_back((*pCoefs)[i]);
	    }   // 如果连续 k+1 个节点相同，则直接跳过该节点与对应的系数
	}
	for(int j=pKnots->cols()-k; j<pKnots->cols(); j++){
	    tmp1.push_back((*pKnots)[j]);
	}

	// 构建输出结果的结构体
        auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : B-
        res->set_field("form", new ba_obj("B-", ba_char_mat));
    
        // knots 节点
        auto *kno = new baltam::matrix<double>(1, tmp1.size());
        for (int i = 0; i < (int)tmp1.size(); i++) {
            (*kno)[i] = tmp1[i];
        }
        res->set_field("knots", new ba_obj(ba_double_mat, kno));

        // coefs B样条的系数
        auto *coefs = new baltam::matrix<double>(1, tmp2.size());
	for (int i = 0; i < (int)tmp2.size(); i++) {
            (*coefs)[i] = tmp2[i];
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));


        // number 多项式的片数
        res->set_field("number", new ba_obj(static_cast<int>(tmp2.size())));

        // order 多项式的阶数
        res->set_field("order", new ba_obj(k));

        // dim 目标函数的维数，目前只能为1
        res->set_field("dim", new ba_obj(1));

        *out_args[0] = ba_obj(ba_struct, res);
    }

    void proto_spmak_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
	
        // 检查参数个数，输入参数个数区间为[2,2]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 2, 1, 1)

        spmak(in_args,out_args);
    }
}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(spmak)

    REGISTER_EXPORT_FUNCTION(spmak, spmak, g_pSpmakHelp)
}
