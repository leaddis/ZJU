/*
 * ==========================================================================
 *
 *       FileName:  fn2fm.cpp
 *
 *    Description:  source file for fn2fm
 *
 *        Version:  1.0
 *        Created:  2023.5.13
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  YangJunyin in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#include "fn2fm.h"
#include <stdexcept>
#include <vector>
#include "Splines/Spline.h"
#include "spapi/spapi.h"
#include "fnder/fnder.h"
#include "fnval/fnval.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "ba_obj/structure.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pFn2fmHelp = R"(
fn2fm 将样条函数转换成指定的形式，并返回转换后的样条函数。

    ns = fn2fm(s,form) 将样条函数 s 转换为 form 格式，并返回转换后的样条函数 ns。

    form 由字符向量或字符串标量形式指定，形式的选择可以为 'B-' 或 'pp'。

    B-form 将函数描述为给定结序列的给定阶 k 的 B 样条的加权和，pp 格式用其局部多项式系数来描述一个函数。
)";

namespace baltam::splines
{
    // B 格式转 pp 格式的函数体
    void fn2fm_case_pp(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args) {
    
        auto s = in_args[0]->get<baltam::structure>(); // 获取输入的待转换样条结构体
        auto *res = new baltam::structure(); // 构建输出结果的结构体

        // form : pp
        res->set_field("form", new ba_obj("pp", ba_char_mat));

        // breaks
        auto bknots = s->get_field("knots")->get<baltam::matrix<double>>(); // 获得b-knots
        std::vector<double> ppknots;
        ppknots.push_back((*bknots)[0]);
        // 去掉重复节点（因为给定的样条结构体节点已经为单增排序，这里直接判定相邻非重即可）
        for (int i = 1; i < bknots->cols(); i++) 
            if ((*bknots)[i]!=(*bknots)[i-1]) 
                ppknots.push_back((*bknots)[i]);
        auto *ppbreaks = new baltam::matrix<double>(1, ppknots.size());
        for (size_t i = 0; i < ppknots.size(); i++)
            (*ppbreaks)[i] = ppknots[i];
        res->set_field("breaks", new ba_obj(ba_double_mat, ppbreaks));
        // 另一种转写方式
        // res->set_field("breaks", (s->get_field("knots")));


        // coefs（尚未完成）
        std::vector<std::vector<Polynomial>> temp_coefs; // 构造临时基样条矩阵
        std::vector<double> temp_bknots;
        for (int i = 0; i < bknots->cols(); i++) 
            temp_bknots.push_back((*bknots)[i]);
        int num = s->get_field("number")->as_int();
        int ord = s->get_field("order")->as_int();

        for (int i = 0; i < num; i++) {
            std::vector<double> temp_site;
            std::vector<Polynomial> temp_line;
            for (int j = 0; j < i+1; j++) {
                Polynomial temp_zero;
                temp_line.push_back(temp_zero);
            }
            for (int j = i; j < ord+i+1; j++)
                temp_site.push_back(temp_bknots[j]);
            Base_Spline temp_spline(temp_site);
            for (int j = 0; j < ord; j++) {
                temp_line.push_back(temp_spline.poly(j));
            }
            for (int j = 0; j < bknots->cols()-ord-i-1; j++) {
                Polynomial temp_zero;
                temp_line.push_back(temp_zero);
            }
            temp_coefs.push_back(temp_line);
        }

        auto bcoefs = s->get_field("coefs")->get<baltam::matrix<double>>();
        std::vector<double> balpha;
        for (int i = 0; i < bcoefs->cols(); i++) 
            balpha.push_back((*bcoefs)[i]);
        
        std::vector<Polynomial> result_poly;
        for (int i = 0; i < bknots->cols(); i++) {
            Polynomial temp_poly;
            for (int j = 0; j < num; j++)
                temp_poly += balpha[j] * temp_coefs[j][i];
            result_poly.push_back(temp_poly);
        }
        
        
        std::vector<std::vector<double>> result_coefs;
        int k=0;
        for (int i = 1; i < bknots->cols(); i++) {
            if ((*bknots)[i]!=(*bknots)[i-1]) {
                std::vector<double> temp_result;
                for (int j = ord-1; j >=0; j--)
                    temp_result.push_back(result_poly[i].reformulate((*bknots)[i-1])[j]);
                    // result_coefs[k][j] = result_poly[i].reformulate((*bknots)[i-1])[j];  
                result_coefs.push_back(temp_result);
                k++;    
            }
        }
        // res->set_field("coefs", new ba_obj(k));

        auto *coefs = new baltam::matrix<double>(k, ord);
        for (int i = 0; i < k; i++)
            for (int j = 0; j < ord; j++)
                (*coefs)(i,j) = result_coefs[i][j];
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // test...
        // auto *yyy = new baltam::matrix<double>(1, temp_y.size()); // 测试
        // for (int i = 0; i < temp_y.size(); i++)
        //     (*yyy)[i] = temp_y[i];
        // res->set_field("spapi_y", new ba_obj(ba_double_mat, yyy));
        // auto *xxx = new baltam::matrix<double>(1, temp_x.size());
        // for (int i = 0; i < temp_x.size(); i++)
        //     (*xxx)[i] = temp_x[i];
        // res->set_field("temp_bkx", new ba_obj(ba_double_mat, xxx));

        // pieces
        int piece = ppknots.size()-1;
        res->set_field("pieces", new ba_obj(piece));

        // order
        res->set_field("order", new ba_obj(static_cast<int64_t>(s->get_field("order")->as_int())));
        
        // dim
        res->set_field("dim", new ba_obj(static_cast<int64_t>(s->get_field("dim")->as_int())));

        *out_args[0] = ba_obj(ba_struct, res);
    }
    // 转换格式不变情况的函数体
    void fn2fm_case_const(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args) {
        auto s = in_args[0]->get<baltam::structure>(); // 获取输入的待转换样条结构体        
        auto *res = new baltam::structure(*s); // 构建输出结果的结构体
        *out_args[0] = ba_obj(ba_struct, res);
    }
    // pp 格式转 B 格式的函数体
    void fn2fm_case_B(std::vector<const_ba_obj_rawptr> &in_args,
                       std::vector<ba_obj_rawptr> &out_args) {
        auto s = in_args[0]->get<baltam::structure>(); // 获取输入的待转换样条结构体
        auto *res = new baltam::structure(); // 构建输出结果的结构体
        // form : B-
        res->set_field("form", new ba_obj("B-", ba_char_mat));
        
        // knots
        auto ppbreaks = s->get_field("breaks")->get<baltam::matrix<double>>(); // 获得pp-breaks
        std::vector<double> bbreaks;
        for (int i = 0; i < ppbreaks->size(); i++) 
            bbreaks.push_back((*ppbreaks)[i]);
        int order = s->get_field("order")->as_int();
        int temp_size = bbreaks.size();
        std::vector<double> bbreaks2;
        bbreaks2 = augknt(bbreaks, order); // 改成
        auto *bknots = new baltam::matrix<double>(1, bbreaks2.size());
        for (size_t i = 0; i < bbreaks2.size(); i++)
            (*bknots)[i] = bbreaks2[i];
        res->set_field("knots", new ba_obj(ba_double_mat, bknots));
        
        // number
        int num = bbreaks2.size() - order;
        res->set_field("number", new ba_obj(num));

        // coefs（尚未完成）
        int tmp = num % temp_size; // num 为 B 样条插值需要的条件数量，即 x 长度.
        // 临时变量，存储各节点需要满足的n（即直至n阶导数条件）
        std::vector<int> temp_times;
        for (int i = 0; i < temp_size; i++) {
            temp_times.push_back((num-tmp)/temp_size-1);
            if (tmp > 0) {
                temp_times[i]+=1;
                tmp-=1;
            }
        }
        std::vector<double> temp_x;
        std::vector<double> temp_y;
        for (int i = 0; i < temp_size; i++) {
            for (int j = 0; j <= temp_times[i]; j++) {
                temp_x.push_back(bbreaks[i]);
                if (j == 0) {
                    temp_y.push_back(fnval(*s,bbreaks[i])); // 尚未完成的部分                  
                } else {
                    temp_y.push_back(fnval(*(fnder_pp_implement(*s,j)),bbreaks[i])); // 尚未完成的部分
                }
            }
        }
        auto *temp_result = new baltam::structure();
        temp_result = spapi(bbreaks2, temp_x, temp_y); // 尚未完成的部分
        auto *coefs = temp_result->get_field("coefs")->get<baltam::matrix<double>>();
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));


        // // 测试 中间过程构造的矩阵的正确性
        // // auto *coefs = new baltam::matrix<double>(1, temp_times.size());
        // // for (int i = 0; i < temp_times.size(); i++)
        // //     (*coefs)[i] = temp_times[i];
        // auto *yyy = new baltam::matrix<double>(1, temp_y.size());
        // for (int i = 0; i < temp_y.size(); i++)
        //     (*yyy)[i] = temp_y[i];
        // res->set_field("spapi_y", new ba_obj(ba_double_mat, yyy));
        // auto *xxx = new baltam::matrix<double>(1, temp_x.size());
        // for (int i = 0; i < temp_x.size(); i++)
        //     (*xxx)[i] = temp_x[i];
        // res->set_field("spapi_x", new ba_obj(ba_double_mat, xxx));

        // 测试其他函数的直接调用（输出参数不行）
        // std::vector<const_ba_obj_rawptr> temp_input;
        // int j = 1;
        // temp_input.push_back(in_args[0]);
        // temp_input.push_back(new ba_obj(j));
        // std::vector<const_ba_obj_rawptr> temp_output;
        // baltam::splines::fnder_pp_case2(temp_input, temp_output);
        
        
        // order
        res->set_field("order", new ba_obj(static_cast<int64_t>(s->get_field("order")->as_int())));
        
        // dim
        res->set_field("dim", new ba_obj(static_cast<int64_t>(s->get_field("dim")->as_int())));

        *out_args[0] = ba_obj(ba_struct, res);
        // *out_args[0] = ba_obj(ba_struct, temp_output);
    }
    // fn2fm 函数体
    void proto_fn2fm_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        // 参数检查
        if (in_args.size() > 2) {
            throw std::invalid_argument("输入参数过多。");
        }
        if (in_args.size() < 1) {
            throw std::invalid_argument("输入参数过少。");
        }
        // 获取参数 s, form.
        auto s = in_args[0]->get<baltam::structure>();
        std::string form = in_args[1]->as_string();
        // 检查 s 是否为样条结构体
        if (!in_args[0]->is_struct()) // 第一个参数应为一个样条结构体
            throw std::invalid_argument{"输入参数错误，请输入样条结构体。"};
        if (!is_legal_spline_structure(*(in_args[0]->get<baltam::structure>())))
            throw std::invalid_argument{"不合法的样条结构体。"};
        // 检查 form 是否为目标结构
        try {
            std::string form_s = s->get_field("form")->as_string();

            if (form_s == "B-" && form == "pp")
                fn2fm_case_pp(in_args, out_args);
            else if (form_s == "pp" && form == "B-")
                fn2fm_case_B(in_args, out_args);
            else if ((form_s == "B-" && form == "B-") || (form_s == "pp" && form == "pp"))
                fn2fm_case_const(in_args, out_args);
            else
                throw std::invalid_argument{"不合法的 form 参数。"};
        } catch (std::invalid_argument&){
            throw std::invalid_argument{"不合法的 form 参数。"};
        }
    }
}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(fn2fm)

    REGISTER_EXPORT_FUNCTION(fn2fm, fn2fm, g_pFn2fmHelp)
}
