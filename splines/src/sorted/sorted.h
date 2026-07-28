/*
 * ==========================================================================
 *
 *       FileName:  sorted.h
 *
 *    Description:  Header file for the sorted function.
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
#ifndef SPLINES_SORTED_H
#define SPLINES_SORTED_H

#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"

namespace baltam::splines
{

    void proto_sorted_baltam(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args);

} // namespace baltam::splines

#endif // SORTED_H
