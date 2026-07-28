/*
 * ==========================================================================
 *
 *       FileName:  csaps.cpp
 *
 *    Description:  source file for csaps
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

#include "csaps.h"
#include <vector>
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pCsapsHelp = R"(
CSAPS 三次平滑样条插值。

YY = CSAPS(X, Y) 返回用于数据点 X 和 Y 的三次平滑样条插值 YY。该函数使用三次样条为数据拟合出一条平滑的曲线。输出 YY 是对应于输入 X 的拟合值。

YY = CSAPS(X, Y, P) 允许你指定平滑参数 P。P 的值控制样条的平滑度：

如果 P = 1，CSAPS 返回没有平滑的三次样条插值。
如果 P = 0，CSAPS 返回最小二乘直线拟合。
P 的值在 0 和 1 之间时，权衡了曲线的平滑度和数据拟合的逼近度。默认的 P 值为 0.5。
YY = CSAPS(X, Y, P, XX) 计算指定点 XX 处的三次平滑样条值，而不是原始数据点 X。

[YY, PP] = CSAPS(...) 还返回分段多项式结构 PP，可以使用 FNVAL 函数对其进行高效的计算。
)";

namespace baltam::splines {



}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(csaps)

    REGISTER_EXPORT_FUNCTION(csaps, csaps, g_pCsapsHelp)
}
