/*
 * ==========================================================================
 *
 *       FileName:  spapi.cpp
 *
 *    Description:  source file for spapi
 *
 *        Version:  1.0
 *        Created:  2023.6.3
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#include "spapi.h"
#include <vector>
#include "Splines/Spline.h"
#include "Splines/SpapiSolver.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pSpapiHelp = R"(
spapi 根据给定的B样条节点或样条曲线阶数以及插值条件创建一个B格式的样条曲线。

    s = spapi(knots,x,y) 创建一个样条曲线，knots 为B样条的节点，x 为插值节点，y为插值节点上的值
    及相应阶导数值，x，y 必须为长度相同的一维向量。这里要求 x 已经完成非降排列，重复的值被视为在该点处
    的对应阶导数值。s 的阶数 k 满足 k = length(knots) - length(x)。

    s = spapi(k,x,y) 创建一个 k 阶样条曲线，x，y 为插值条件，其要求与 s = spapi(knots,x,y) 相同。
)";

namespace baltam::splines {
    
    // 用于内部调用，返回一个B样条结构体指针 
    baltam::structure *spapi(std::vector<double> knots, std::vector<double> x, std::vector<double> y)
    {	

        if (x.size() != y.size()){
            throw std::invalid_argument("x 和 y 的长度不匹配");
        }
        // 检查 knots 和 x 是否单调非降
        if (knots.size() > 0){
            for(size_t i = 0; i<knots.size()-1; i++){
                if(knots[i]>knots[i+1]){
                    throw std::invalid_argument("knots需按单调非降顺序排列。");
                }
            }
        }

        if (x.size() > 0){
            for(size_t i = 0; i<x.size()-1; i++){
                if(x[i]>x[i+1]){
                    throw std::invalid_argument("x需按单调非降顺序排列。");
                }
            }
        }

        // 求解系数
        SpapiSolver solver(knots,x,y);
        bool test = solver.solve();
        std::vector<double> tmp = solver.get_res();
        if(test!=true){
            throw std::invalid_argument("插值条件不相容。");
        }

        // 构建输出结果的结构体
        auto *res = new baltam::structure(); // 指向输出结果的指针
                                             // form : B-
        res->set_field("form", new ba_obj("B-", ba_char_mat));

        // knots 节点
        auto *kno = new baltam::matrix<double>(1, knots.size());
        for (size_t i = 0; i < knots.size(); i++) {
            (*kno)[i] = knots[i];
        }
        res->set_field("knots", new ba_obj(ba_double_mat, kno));

        // coefs B样条的系数
        auto *coefs = new baltam::matrix<double>(1, tmp.size());
        for (int i = 0; i < (int)tmp.size(); i++) {
            (*coefs)[i] = tmp[i];
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // number 多项式的片数
        res->set_field("number", new ba_obj(static_cast<int>(tmp.size())));

        // order 多项式的阶数
        res->set_field("order", new ba_obj(static_cast<int>(knots.size()-tmp.size())));

        // dim 目标函数的维数，目前只能为1
        res->set_field("dim", new ba_obj(1));

        return res;
    }

    void spapi_case1(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr xTemp = in_args[1];
	const_ba_obj_rawptr yTemp = in_args[2];
	const_ba_obj_ptr  px_smart_ptr, py_smart_ptr;
	
	spline_parameter_check_double(in_args[1],xTemp,px_smart_ptr);
	spline_parameter_check_double(in_args[2],yTemp,py_smart_ptr);

        auto pKnots = in_args[0]->get<matrix<double>>();
        auto pX = xTemp->get<matrix<double>>();
	auto pY = yTemp->get<matrix<double>>();
	
        // 对输入参数的合理性进行检查
	if (!pKnots->is_vector()) {
            throw std::invalid_argument("第一个输入参数必须为向量。");
        }
	if (!pX->is_vector()) {
            throw std::invalid_argument("第二个输入参数必须为向量。");
        }
	if (!pY->is_vector()) {
            throw std::invalid_argument("第三个输入参数必须为向量。");
        }
	
        if ((pX->cols()) != (pY->cols())){
	    throw std::invalid_argument("x 和 y 的长度不匹配");
	}
	// 检查 knots 和 x 是否单调非降
        for(baIndex i = 0; i<pKnots->size()-1; i++){
	    if((*pKnots)[i]>(*pKnots)[i+1]){
		throw std::invalid_argument("knots需按单调非降顺序排列。");
	    }
	}
	for(baIndex i = 0; i<pX->size()-1; i++){
	    if((*pX)[i]>(*pX)[i+1]){
		throw std::invalid_argument("x需按单调非降顺序排列。");
	    }
	}

	// 求解系数
	std::vector<double> _knots,_x,_y;
	for(int i=0; i<pX->size(); i++){
	    _x.push_back((*pX)[i]);
	    _y.push_back((*pY)[i]);
	}
	for(int i=0; i<pKnots->size(); i++){
	    _knots.push_back((*pKnots)[i]);
	}
	SpapiSolver solver(_knots,_x,_y);
	bool test = solver.solve();
	std::vector<double> tmp = solver.get_res();
	if(test!=true){
	    throw std::invalid_argument("插值条件不相容。");
	}

	// 构建输出结果的结构体
        auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : B-
        res->set_field("form", new ba_obj("B-", ba_char_mat));
    
        // knots 节点
        auto *kno = new baltam::matrix<double>(1, pKnots->size());
        for (int i = 0; i < pKnots->size(); i++) {
            (*kno)[i] = (*pKnots)[i];
        }
        res->set_field("knots", new ba_obj(ba_double_mat, kno));

        // coefs B样条的系数
        auto *coefs = new baltam::matrix<double>(1, tmp.size());
	    for (int i = 0; i < (int)tmp.size(); i++) {
            (*coefs)[i] = tmp[i];
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // number 多项式的片数
        res->set_field("number", new ba_obj(static_cast<int>(tmp.size())));

        // order 多项式的阶数
        res->set_field("order", new ba_obj(static_cast<int>(_knots.size()-tmp.size())));

        // dim 目标函数的维数，目前只能为1
        res->set_field("dim", new ba_obj(1));

        *out_args[0] = ba_obj(ba_struct, res);
    }

    void spapi_case2(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
	const_ba_obj_rawptr kTemp = in_args[0];
        const_ba_obj_rawptr xTemp = in_args[1];
	const_ba_obj_rawptr yTemp = in_args[2];
	const_ba_obj_ptr  px_smart_ptr, py_smart_ptr;
	
	spline_parameter_check_double(in_args[1],xTemp,px_smart_ptr);
	spline_parameter_check_double(in_args[2],yTemp,py_smart_ptr);

	int k = 0;
	try {
	    k = kTemp->as_int();
	}
	catch (std::invalid_argument &) {
	    throw std::invalid_argument("B样条的阶数必须为整数。");
	}
        auto pX = xTemp->get<matrix<double>>();
	auto pY = yTemp->get<matrix<double>>();
	
        // 对输入参数的合理性进行检查
	if (k < 2) {
            throw std::invalid_argument("B样条的阶数至少为2。");
        }
	if (!pX->is_vector()) {
            throw std::invalid_argument("第二个输入参数必须为向量。");
        }
	if (!pY->is_vector()) {
            throw std::invalid_argument("第三个输入参数必须为向量。");
        }
        if ((pX->cols()) != (pY->cols())){
	    throw std::invalid_argument("x 和 y 的长度不匹配");
	}

	if (k > pX->size()){
	    k = pX->size();
	}
	
	// x 是否单调非降
	for(baIndex i = 0; i<pX->size()-1; i++){
	    if((*pX)[i]>(*pX)[i+1]){
		throw std::invalid_argument("x需按单调非降顺序排列。");
	    }
	}
	// x中的点最多重复 k-1 次
	for(baIndex i = 0; i<=pX->size()-k; i++){
	    if((*pX)[i]==(*pX)[i+k-1]){
		throw std::invalid_argument("x中的点最多重复k-1次。");
	    }
	}

	// 求解系数
	std::vector<double> _x,_y;
	for(int i=0; i<pX->size(); i++){
	    _x.push_back((*pX)[i]);
	    _y.push_back((*pY)[i]);
	}
	SpapiSolver solver(k,_x,_y);
	bool test = solver.solve();
	std::vector<double> tmp = solver.get_res();
	std::vector<double> Knots = solver.get_knots();
	if(test!=true){
	    throw std::invalid_argument("插值条件不相容。");
	}

	// 构建输出结果的结构体
        auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : B-
        res->set_field("form", new ba_obj("B-", ba_char_mat));
    
        // knots 节点
        auto *knots = new baltam::matrix<double>(1, Knots.size());
        for (size_t i = 0; i < Knots.size(); i++) {
            (*knots)[i] = Knots[i];
        }
        res->set_field("knots", new ba_obj(ba_double_mat, knots));

        // coefs B样条的系数
        auto *coefs = new baltam::matrix<double>(1, tmp.size());
	    for (int i = 0; i < (int)tmp.size(); i++) {
            (*coefs)[i] = tmp[i];
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // number 多项式的片数
        res->set_field("number", new ba_obj(static_cast<int>(tmp.size())));

        // order 多项式的阶数
        res->set_field("order", new ba_obj(k));

        // dim 目标函数的维数，目前只能为1
        res->set_field("dim", new ba_obj(1));

        *out_args[0] = ba_obj(ba_struct, res);
    }

    void proto_spapi_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
	
        // 检查参数个数，输入参数个数区间为[3,3]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(3, 3, 1, 1)

	const_ba_obj_rawptr Temp = in_args[0];
	const_ba_obj_ptr pT_smart_ptr;
	spline_parameter_check_double(in_args[0],Temp,pT_smart_ptr);
	auto pKnots = Temp->get<matrix<double>>();
	if((pKnots->cols())==1 && (pKnots->rows())==1){
	    spapi_case2(in_args,out_args);
	}else{
	    spapi_case1(in_args,out_args);
	}
    }
}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(spapi)

    REGISTER_EXPORT_FUNCTION(spapi, spapi, g_pSpapiHelp)
}

