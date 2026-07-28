/*
 * ============================================================================
 *
 *       Filename:  ExportFunction.cpp
 *
 *    Description:  导出函数列表整合单例
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

#include "ExportFunction.h"

namespace baltam {
    void ExportFunctionManager::register_export_function(const bexfun_info_t& fun){
        m_data.push_back(fun);
    }

    void ExportFunctionManager::register_export_function(bexfun_info_t&& fun){
        m_data.emplace_back(fun);
    }

    auto ExportFunctionManager::data() -> vec_t& {
        return m_data;
    }

    ExportFunctionManager& ExportFunctionManager::get_instance(){
        static ExportFunctionManager mgr;
        return mgr;
    }
}

