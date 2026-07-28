/*
 * ==========================================================================
 *
 *       FileName:  csape.h
 *
 *    Description:  header for csape
 *
 *        Version:  1.0
 *        Created:  2023.3.9
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  shaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#ifndef  SPLINES_CSAPE_H
#define  SPLINES_CSAPE_H

#include "splines_common.h"

namespace baltam::splines{
    void proto_csape_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);
}

#endif //  SPLINES_CSAPE_H

