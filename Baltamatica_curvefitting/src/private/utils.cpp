/*
 * =====================================================================================
 *
 *       Filename:  utils.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/18/2022 05:05:35 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2022, Haoyang Liu
 *
 * =====================================================================================
 */

#include "private/utils.h"
#include "ba_obj/sparse_matrix.h"
#include <stdexcept>
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/generator.h"
#include "proto/type_factory.h"
#include "utils/numeric_cast.h"

namespace baltam::splines::internal {
    template <int s, int t>
    ba_obj_rawptr matrix_numeric_cast(const const_ba_obj_rawptr& src){
        if constexpr (s == t){
            return new ba_obj(*src);
        } else {
            using S = typename type_factory<s>::type;
            using T = typename type_factory<t>::type;

            const auto *p_src = src->get<S>();
            auto *p_dst = new T(p_src->rows(), p_src->cols());
            auto dst = new ba_obj(t, p_dst);

            for (baIndex i = 0; i < p_src->size(); ++i)
                (*p_dst)[i] = numeric_cast<typename T::type>((*p_src)[i]);

            return dst;
        }
    }

    template <int s, int t>
    ba_obj_rawptr sparse_matrix_numeric_cast(const const_ba_obj_rawptr& src) {
        if constexpr (s == t) {
            return new ba_obj(*src);
        }
        else if (t >= ba_sparse_double && t <= ba_sparse_logical) {
            using S = typename type_factory<s>::type;
            using T = typename type_factory<t>::type::type;

            const auto *p_src = src->get<S>();
            std::vector<triplet<T>> values;
            auto *pr = p_src->pr();
            auto *ir = p_src->ir();
            auto *jc = p_src->jc();
            for (baIndex j = 0; j < p_src->cols(); ++j) {
                for (baSparseIndex p = jc[j]; p < jc[j + 1]; ++p) {
                    values.template emplace_back(ir[p], j, numeric_cast<T>(pr[p]));
                }
            }
            auto *p_dst = new sparse_matrix<T>(values, p_src->rows(), p_src->cols());
            auto dst = new ba_obj(t, p_dst);
            return dst;
        }
        return nullptr;
    }

    template <int t>
    ba_obj_rawptr matrix_numeric_cast(const const_ba_obj_rawptr& src){

#define __call_matrix_numeric_cast_impl(i, _) \
        return matrix_numeric_cast<i, t>(src);

        BALTAM_SWITCH_CASE_FROM_TO(
                src->type(),
                BA_TYPE_INT_MAT,
                BA_TYPE_BOOL_MAT,
                __call_matrix_numeric_cast_impl, _)

#undef __call_matrix_numeric_cast_impl

        if (src->is_sparse()) {
#define __call_sparse_matrix_numeric_cast_impl(i, _) \
        return sparse_matrix_numeric_cast<i, t>(src);

            BALTAM_SWITCH_CASE_FROM_TO(
                    src->type(),
                    BA_TYPE_SPARSE_DOUBLE,
                    BA_TYPE_SPARSE_LOGICAL,
                    __call_sparse_matrix_numeric_cast_impl, _)

#undef __call_sparse_matrix_numeric_cast_impl
        }
        return nullptr;
    }

    ba_obj_rawptr matrix_numeric_cast(const const_ba_obj_rawptr& src, int type){

#define __call_matrix_numeric_cast_impl(i, _) \
        return matrix_numeric_cast<i>(src);

        BALTAM_SWITCH_CASE_FROM_TO(
                type,
                BA_TYPE_INT_MAT,
                BA_TYPE_BOOL_MAT,
                __call_matrix_numeric_cast_impl, _)

        if (src->is_sparse()) {
            BALTAM_SWITCH_CASE_FROM_TO(
                    type,
                    BA_TYPE_SPARSE_DOUBLE,
                    BA_TYPE_SPARSE_LOGICAL,
                    __call_matrix_numeric_cast_impl, _)
        }
#undef __call_matrix_numeric_cast_impl

        return nullptr;
    }
}

