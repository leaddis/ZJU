/*
 * ============================================================================
 *
 *       Filename:  RegisterExportFunction.h
 *
 *    Description:  导出函数注册功能宏
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

#ifndef BALTAM_FFT_REGISTER_EXPORT_FUNCTION_H
#define BALTAM_FFT_REGISTER_EXPORT_FUNCTION_H

#include "ExportFunction.h"

#define REGISTER_EXPORT_FUNCTION(key, fun, help) \
    namespace export_function_##key##_##fun { \
        struct register_##fun { \
            register_##fun() { \
                ExportFunctionManager::get_instance().register_export_function( \
                        {#key, fun, help}); \
            } \
        }; \
        static register_##fun _register_##fun; \
    } \

#endif

