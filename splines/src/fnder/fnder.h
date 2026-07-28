/*
 * ==========================================================================
 *
 *       FileName:  fnder.h
 *
 *    Description:  header for fnder
 *
 *        Version:  1.0
 *        Created:  2023.4.9
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  CaoShaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#ifndef SPLINES_FNDER_H
#define SPLINES_FNDER_H

#include "splines_common.h"

namespace baltam::splines {
    baltam::structure *fnder_pp_implement(const baltam::structure &S, int degree);
    baltam::structure *fnder_B_implement(const baltam::structure &S, int degree);
    void fnder_B_implement_diff(baltam::structure &S); // 这里的返回值类型不同其他
    baltam::structure *fnder_B_implement_diff(const baltam::structure &S, int degree);
    baltam::structure *fnder_B_implement_integrate(const baltam::structure &S, int degree);
    void proto_fnder_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_FNDER_H

