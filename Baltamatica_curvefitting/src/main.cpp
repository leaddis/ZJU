/*
 * ==========================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  main file for "fft plugin"
 *
 *        Version:  2.0
 *        Created:  2022-12-16
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  JinGang Zhou , jingang.zhou@pku.edu.cn
 *   Organization:  CQBDRI
 *      Copyright:  Copyright (c) 2022, Jingang Zhou
 *
 * ==========================================================================
 */

#include "bex/bex.hpp"
#include "splines_common.h"

using namespace baltam;

int bxPluginInit(int, const bxArray*[]){
//    bxAddCXXClass<Spline<1,SplineType::ppForm> >("spline");
//    bxAddCXXClass<Spline<3,SplineType::ppForm> >("spline");
//    bxAddCXXClass<Spline<1,SplineType::BSpline> >("spline");
//    bxAddCXXClass<Spline<3,SplineType::BSpline> >("spline");
    return 0;
}

int bxPluginFini(){ return 0; }

bexfun_info_t * bxPluginFunctions() {
    static bool bInit = false;
    if (!bInit){
        bInit = true;
        ExportFunctionManager::get_instance().data().push_back({"", nullptr, nullptr});
    }
    return ExportFunctionManager::get_instance().data().data();
}
