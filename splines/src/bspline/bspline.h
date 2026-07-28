/*
 * ==========================================================================
 *
 *       FileName:  bspline.h
 *
 *    Description:  header for bspline
 *
 *        Version:  1.0
 *        Created:  2022.12.15
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#ifndef SPLINES_BSPLINE_H
#define SPLINES_BSPLINE_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_bspline_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_BSPLINE_H

