/*
 * ==========================================================================
 *
 *       FileName:  spapi.h
 *
 *    Description:  header for spapi
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


#ifndef SPLINES_SPAPI_H
#define SPLINES_SPAPI_H

#include "splines_common.h"

namespace baltam::splines {
    baltam::structure *spapi(std::vector<double> knots, std::vector<double> x, std::vector<double> y);
    void proto_spapi_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_SPAPI_H

