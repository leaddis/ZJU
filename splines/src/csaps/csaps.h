/*
* ==========================================================================
 *
 *       FileName:  csaps.h
 *
 *    Description:  header for csaps
 *
 *        Version:  1.0
 *        Created:  2022.12.15
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:QianZhouyue
 *      Copyright:
 *
 * ==========================================================================
 */

#ifndef SPLINES_CSAPS_H
#define SPLINES_CSAPS_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_csaps_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_CSAPS_H