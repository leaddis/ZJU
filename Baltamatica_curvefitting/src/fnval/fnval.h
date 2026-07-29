/*
 * ==========================================================================
 *
 *       FileName:  fnval.h
 *
 *    Description:  header for fnval
 *
 *        Version:  1.0
 *        Created:  2023.3.22
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  shaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#ifndef SPLINES_FNVAL_H
#define SPLINES_FNVAL_H

#include "splines_common.h"

namespace baltam::splines{
    double fnval(const baltam::structure & S, double x);
    void proto_fnval_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);
}

#endif