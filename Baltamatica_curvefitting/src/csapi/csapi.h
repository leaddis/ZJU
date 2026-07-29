/*
 * ==========================================================================
 *
 *       FileName:  csapi.h
 *
 *    Description:  header for csapi
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

#ifndef SPLINES_CSAPI_H
#define SPLINES_CSAPI_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_csapi_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_CSAPI_H

