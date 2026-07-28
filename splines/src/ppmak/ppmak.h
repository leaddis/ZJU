/*
 * ==========================================================================
 *
 *       FileName:  ppmak.h
 *
 *    Description:  header for ppmak
 *
 *        Version:  1.0
 *        Created:  2023.4.6
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#ifndef SPLINES_PPMAK_H
#define SPLINES_PPMAK_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_ppmak_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_PPMAK_H

