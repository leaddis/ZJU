/*
 * ==========================================================================
 *
 *       FileName:  aveknt.h
 *
 *    Description:  header for aveknt
 *
 *        Version:  1.0
 *        Created:  2024.8.28
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  QianZhouyue
 *      Copyright:  non
 *
 * ==========================================================================
 */

#ifndef SPLINES_AVEKNT_H
#define SPLINES_AVEKNT_H

#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"


namespace baltam::splines {
    void proto_aveknt_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_AVEKNT_H