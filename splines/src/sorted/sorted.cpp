/*
 * ==========================================================================
 *
 *       FileName:  sorted.cpp
 *
 *    Description:  Source file for the sorted function.
 *
 *        Version:  1.0
 *        Created:  2024.08.27
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Bianchenhao
 *      Copyright:
 *
 * ==========================================================================
 */
#include <vector>
#include <algorithm> // For std::sort
#include "sorted.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pSortedHelp = R"(
函数sorted的基本用法："
pointer = sorted(meshsites, sites)
这条语句返回一个整数行向量 pointer，其中的每个元素 j 表示在 meshsites 中小于或等于 sites(j) 的元素数量。
功能详解：
meshsites：这是一个非递减的序列，代表了一组基准点或结点。
sites：这是另一组需要评估的点，函数将查找这些点在 meshsites 中的相对位置。
函数通过比较 sites 中的每个点与 meshsites 中的点，计算出每个 sites 点左侧的 meshsites 点的数量。
)";

namespace baltam::splines
{

    // 计算 pointer 数组的函数
    void compute_pointer(std::vector<double> &meshsites, std::vector<double> &sites, std::vector<double> &pointer)
    {
        size_t mesh_index = 0;
        std::sort(meshsites.begin(), meshsites.end());
        std::sort(sites.begin(), sites.end());
        for (int i = 0; i < static_cast<int>(sites.size()); ++i)
        { // 从小到大排序后依次枚举
            while (mesh_index < meshsites.size() && meshsites[mesh_index] <= sites[i])
            {
                mesh_index++;
            }
            pointer[i] = static_cast<double>(mesh_index);
        }
    }

    // 主函数实现
    void proto_sorted_baltam(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args)
    {
        BALTAM_PARAM_CHECK(2, 2, 1, 1) // 检查输入参数和输出参数的数量

        auto pMeshSites = in_args[0]->get<matrix<double>>();
        auto pSites = in_args[1]->get<matrix<double>>();

        if (!pMeshSites || !pSites )
        { // 矩阵大小检查
            throw std::invalid_argument("输入参数不合理");
        }

        // 将输入矩阵转换为向量
        std::vector<double> meshsites(pMeshSites->cols(), 0);
        std::vector<double> sites(pSites->cols(), 0);
        for (int i = 0; i < pMeshSites->cols(); ++i)
        {
            meshsites[i] = (*pMeshSites)(0, i);
        }
        for (int i = 0; i < pSites->cols(); ++i)
        {
            sites[i] = (*pSites)(0, i);
        }

        std::vector<double> pointer(sites.size(), 0);
        compute_pointer(meshsites, sites, pointer); // 计算

        auto pPointer = new baltam::matrix<double>(1, pointer.size());
        for (size_t i = 0; i < pointer.size(); ++i)
        {
            (*pPointer)(0, i) = pointer[i];
        }

        *out_args[0] = ba_obj(ba_double_mat, pPointer);
    }

} // namespace baltam::splines

namespace baltam::splines
{
    GENERATE_SPLINES_PLUGIN_FUNC(sorted)
    REGISTER_EXPORT_FUNCTION(sorted, sorted, g_pSortedHelp)
}
