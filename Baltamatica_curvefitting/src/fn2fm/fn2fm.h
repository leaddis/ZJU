/*
 * ==========================================================================
 *
 *       FileName:  fn2fm.h
 *
 *    Description:  header for fn2fm
 *
 *        Version:  1.0
 *        Created:  2023.6.10
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  YangJunyin in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#ifndef SPLINES_FN2FM_H
#define SPLINES_FN2FM_H

#include "splines_common.h"

namespace baltam::splines {
    void proto_fn2fm_baltam(std::vector<const_ba_obj_rawptr>& in_args,
                            std::vector<ba_obj_rawptr>& out_args);

}
#endif //SPLINES_FN2FM_H