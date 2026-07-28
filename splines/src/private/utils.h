/*
 * ============================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/18/2022 04:55:35 PM
 *       Revision:  none
 *       Compiler:  gcc/g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2022, Haoyang Liu
 *
 * ============================================================================
 */

#ifndef BALTAM_BUILTIN_UTILS_H
#define BALTAM_BUILTIN_UTILS_H

#include "ba_obj/ba_obj.h"
#include "typedefs.h"
#include <vector>

namespace baltam::splines::internal {
    /**
     * @brief 通用类型转化函数（非模板）
     * @param src 源数据指针。
     * @param type 转化后的类型。
     * @return 转化后的 @p ba_obj 对象指针。如果不能转化，则返回 @p nullptr
     *
     * 转化的机制是调用了 @p numerical_cast ，即在原有静态转化基础上，增加了
     * 复数到其它类型的转化（先取实部）。
     *
     * @attention 当无法得知转化后的目标类型时，才考虑调用该函数。否则应该调用
     * 通用类型转化函数（模板）。
     */
    ba_obj_rawptr matrix_numeric_cast(const const_ba_obj_rawptr& src, int type);

    /**
     * @brief 通用类型转化函数（模板）
     * @tparam t 转化后的类型。
     * @param src 源数据指针。
     * @return 转化后的 @p ba_obj 对象指针。如果不能转化，则返回 @p nullptr
     *
     * 转化的机制是调用了 @p numerical_cast ，即在原有静态转化基础上，增加了
     * 复数到其它类型的转化（先取实部）。
     *
     * @attention 当可以知道转化后的目标类型时，建议调用该函数。否则考虑调用
     * 通用类型转化函数（非模板）。
     */
    template <int t>
    ba_obj_rawptr matrix_numeric_cast(const const_ba_obj_rawptr& src);
}

#endif

