/*
 * ==========================================================================
 *
 *       FileName:  spmak.h
 *
 *    Description:  header for spmak
 *
 *        Version:  1.0
 *        Created:  2023.5.13
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#ifndef SPLINES_SPMAK_H
#define SPLINES_SPMAK_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_spmak_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_SPMAK_H
