/*
 * ============================================================================
 *
 *       Filename:  ExportFunction.cpp
 *
 *    Description:  header for ExportFunction
 *
 *        Version:  1.0
 *        Created:  01/09/2022
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2022, Haoyang Liu
 *
 * ============================================================================
 */

#ifndef BALTAM_FFT_EXPORT_FUNCTION_H
#define BALTAM_FFT_EXPORT_FUNCTION_H

#include "bex/extern_fcn_type.h"
#include <string>
#include <vector>

namespace baltam {
    class ExportFunctionManager {
        public:
            using vec_t = std::vector<bexfun_info_t>;
            void register_export_function(const bexfun_info_t& info);
            void register_export_function(bexfun_info_t&& info);
            vec_t& data();
            static ExportFunctionManager &get_instance();
        private:
            ExportFunctionManager() = default;
            vec_t m_data;
    };
}

#endif

