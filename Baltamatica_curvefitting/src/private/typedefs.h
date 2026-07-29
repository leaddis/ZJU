/*
 * ============================================================================
 *
 *       Filename:  typedefs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/31/2022 07:43:09 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2022, Haoyang Liu
 *
 * ============================================================================
 */

#ifndef BALTAM_PLUGIN_SPLINES_TYPEDEFS_H
#define BALTAM_PLUGIN_SPLINES_TYPEDEFS_H

#include "ba_obj/ba_obj.h"

namespace baltam::splines {
    using const_ba_obj_rawptr = const ba_obj*;
    using ba_obj_rawptr = ba_obj*;
}

#endif

