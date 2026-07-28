/*
 * ==========================================================================
 *
 *       FileName:  knt2mlt.h
 *
 *    Description:  Header file for knt2mlt function
 *
 *        Version:  1.0
 *        Created:  2024.08.27
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Bianchenhao
 *      Copyright:
 *
 * ==========================================================================
 */
#ifndef SPLINES_KNT2MLT_H
#define SPLINES_KNT2MLT_H

#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"

namespace baltam::splines
{

    // 计算结点的多重性
    void compute_multiplicities(std::vector<const_ba_obj_rawptr> &in_args, std::vector<double> &M);

    // 仅返回多重性向量
    void return_only_multiplicities(std::vector<ba_obj_rawptr> &out_args, const std::vector<double> &M);

    // 返回多重性向量和排序后的结点序列
    void return_multiplicities_and_sorted_t(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args, const std::vector<double> &M);

    // 主函数：处理输入并分配输出
    void proto_knt2mlt_baltam(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args);

} // namespace baltam::splines

#endif // SPLINES_KNT2MLT_H
