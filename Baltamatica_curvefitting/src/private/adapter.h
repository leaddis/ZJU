/*
 * ============================================================================
 *
 *       Filename:  adapter.h
 *
 *    Description:  header for adapter
 *
 *        Version:  1.0
 *        Created:  06/08/2022 02:30:17 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2022, Haoyang Liu
 *
 * ============================================================================
 */

#ifndef BALTAM_PLUGIN_SPLINES_ADAPTER_H
#define BALTAM_PLUGIN_SPLINES_ADAPTER_H

#include "typedefs.h"
#include "bex/bex.hpp"
#include "ba_obj/ba_obj.h"
#include <vector>

namespace baltam::splines {
    template <class F>
    BALTAM_LOCAL void adapt(F&& f, int nlhs, bxArray *plhs[], int nrhs, const bxArray *prhs[]){
        std::vector<const_ba_obj_rawptr> in_args;
        std::vector<ba_obj_rawptr> out_args;

        for (auto i = 0; i < nrhs; ++i){
            in_args.emplace_back(reinterpret_cast<const_ba_obj_rawptr>(prhs[i]));
        }

        out_args.emplace_back(new ba_obj());
        for (auto i = 1; i < nlhs; ++i){
            out_args.emplace_back(new ba_obj());
        }

        try {
            f(in_args, out_args);
        }
        catch (...){
            for (size_t i = 0; i < out_args.size(); ++i){
                plhs[i] = nullptr;
                delete out_args[i];
            }
            throw;
        }

        for (size_t i = 0; i < out_args.size(); ++i){
            if (out_args[i]->type() == ba_void){
                plhs[i] = nullptr;
                delete out_args[i];
            } else {
                plhs[i] = reinterpret_cast<bxArray*>(out_args[i]);
            }
        }
    }
}
#endif

