/*
 * ==========================================================================
 *
 *       FileName:  ppmak.cpp
 *
 *    Description:  source file for ppmak
 *
 *        Version:  1.0
 *        Created:  2023.4.6
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:
 *      Copyright:
 *
 * ==========================================================================
 */

#include "ppmak.h"
#include <vector>
#include "limits.h"
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include <iostream>

static const char *g_pPpmakHelp = R"(
ppmak 根据给出的信息创建 pp 格式的分段多项式。

    pp = ppmak(breaks,coefs) 根据节点和系数信息创建对应的分段多项式。这里要求 breaks 是单增
    的一维向量，coefs 是一个矩阵，其列数 l = (length(breaks) - 1)*Order , Order 是分段多
    项式的次数; 行数决定了分段多项式的数量，即每行是一个以 breaks 为节点, 以 coefs 对应行为系数
    的分段多项式。pp 返回对应的分段多项式曲线，为 pp 格式。

    [pp1,pp2,...,ppm] = ppmak(breaks,coefs) 根据节点和系数信息创建对应的分段多项式。这里要
    求 breaks 是单增的一维向量, coefs 是一个矩阵。该函数返回多个一维分片多项式，即 coefs 的每
    一行对应输出中的一个一维分片多项式，为 pp 格式。输入参数要求与 pp = ppmak(breaks,coefs) 
    相同。

    pp = ppmak(breaks,coefs,d) 根据节点和系数信息创建对应的分段多项式。这里要求 breaks 是单
    增的一维向量，coefs 是一个矩阵，形式与 ppmak(breaks,coefs) 中不同：其列数是分段多项式的次
    数 Order ; 行数 r = (length(breaks - 1)*d, 其格式与 pp = ppmak(breaks,coefs) 输出
    结果中 coefs 矩阵的格式相同。pp 返回对应的分段多项式曲线，为 pp 格式。
)";

namespace baltam::splines {

    // 输入参数为2个的情形 pp = ppmak(breaks,coefs)
    void ppmak_case1(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr breaksTemp = in_args[0];
        const_ba_obj_rawptr coefsTemp = in_args[1];
        const_ba_obj_ptr pb_smart_ptr, pc_smart_ptr;
        spline_parameter_check_double(in_args[0],breaksTemp,pb_smart_ptr);
        spline_parameter_check_double(in_args[1],coefsTemp,pc_smart_ptr);

        auto pBreaks = breaksTemp->get<matrix<double>>();
        auto pCoefs = coefsTemp->get<matrix<double>>();

        // 对输入参数的合理性进行检查
	int t1 = pCoefs->cols();
	int t2 = pBreaks->cols()-1;
        if ((t1%t2) != 0)
            throw std::invalid_argument{"输入参数不匹配，length(breaks)-1 必须整除 coefs 的列数。"};

        auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : pp
        res->set_field("form", new ba_obj("pp", ba_char_mat));

        // breaks 节点
        auto *bre = new baltam::matrix<double>(1, pBreaks->cols());
        for (int i = 0; i < (pBreaks->cols()); i++) {
            (*bre)[i] = (*pBreaks)[i];
        }
        res->set_field("breaks", new ba_obj(ba_double_mat, bre));

        // coefs 分片多项式的系数
        int m = (pCoefs->rows()) * ((pBreaks->cols()) - 1); // 矩阵的行数
        int n = (pCoefs->cols()) / (pBreaks->cols() - 1); // 矩阵的列数,即分片多项式的阶数
        int num = (pBreaks->cols()) - 1; // 分片多项式的数量
        int dim = pCoefs->rows(); // 样条曲线的维数

        auto *coefs = new baltam::matrix<double>(m, n);
        for (int i = 0; i < num; i++) {
            for (int j = 1; j <= dim; j++) {
                for (int k = 0; k < n; k++) {
                    (*coefs)(i * dim + j - 1, k) = (*pCoefs)(j - 1, i * n + k);
                }
            }
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // pieces 分片多项式的数量
        res->set_field("pieces", new ba_obj(static_cast<int>(num)));

        // order 分片多项式的阶数
        res->set_field("order", new ba_obj(static_cast<int>(n)));

        // dim 样条曲线的维数
        res->set_field("dim", new ba_obj(static_cast<int>(dim)));

        *out_args[0] = ba_obj(ba_struct, res);
    }

    // 输入参数为3个的情形 pp = ppmak(breaks,coefs,d)
    void ppmak_case2(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
	const_ba_obj_rawptr breaksTemp = in_args[0];
        const_ba_obj_rawptr coefsTemp = in_args[1];
	const_ba_obj_ptr pb_smart_ptr, pc_smart_ptr;
	spline_parameter_check_double(in_args[0],breaksTemp,pb_smart_ptr);
	spline_parameter_check_double(in_args[1],coefsTemp,pc_smart_ptr);
        const_ba_obj_rawptr dTemp = in_args[2];

        // 对第三个参数进行检查
        int d = 0;
        try {
            d = dTemp->as_int(); // 不是 int 类型将会抛出异常
        }
        catch (std::invalid_argument &) {
            throw std::invalid_argument{"第三个参数必须为整数。"};
        }

        auto pBreaks = breaksTemp->get<matrix<double>>();
        auto pCoefs = coefsTemp->get<matrix<double>>();

        int pieces = pBreaks->cols() - 1; // 分段多项式的数量
        int m = pCoefs->rows(); // 系数矩阵的行数
        int order = pCoefs->cols(); // 分段多项式的阶数，即系数矩阵的列数

        // 对输入参数的合理性进行检查
        if (d * pieces != m)
            throw std::invalid_argument{
                    "输入参数不匹配，维数 d 与 length(breaks)-1 的乘积必须等于 coefs 的行数。"};

        auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : pp
        res->set_field("form", new ba_obj("pp", ba_char_mat));

        // breaks 节点
        auto *bre = new baltam::matrix<double>(1, pBreaks->cols());
        for (int i = 0; i < (pBreaks->cols()); i++) {
            (*bre)[i] = (*pBreaks)[i];
        }
        res->set_field("breaks", new ba_obj(ba_double_mat, bre));

        // coefs 分片多项式的系数，与用户提供的系数矩阵相同
        auto *coefs = new baltam::matrix<double>(m, order);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < order; j++) {
                (*coefs)(i, j) = (*pCoefs)(i, j);
            }
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // pieces 分片多项式的数量
        res->set_field("pieces", new ba_obj(static_cast<int>(pieces)));

        // order 分片多项式的阶数
        res->set_field("order", new ba_obj(static_cast<int>(order)));

        // dim 样条曲线的维数
        res->set_field("dim", new ba_obj(static_cast<int>(d)));

        *out_args[0] = ba_obj(ba_struct, res);
    }

    // 使用矩阵 pcoef 的第i行 构建一个1维的pp结构体
    baltam::structure *generate_one_dimension(const matrix<double>* pBreaks, const matrix<double>* pCoefs, int k)
    {
	if(k<0||k>pCoefs->rows()-1){
	    throw std::invalid_argument{"矩阵行数越界！"};
	}

	auto *res = new baltam::structure(); // 指向输出结果的指针
        // form : pp
        res->set_field("form", new ba_obj("pp", ba_char_mat));

        // breaks 节点
        auto *bre = new baltam::matrix<double>(1, pBreaks->cols());
        for (int i = 0; i < (pBreaks->cols()); i++) {
            (*bre)[i] = (*pBreaks)[i];
        }
        res->set_field("breaks", new ba_obj(ba_double_mat, bre));

        // coefs 分片多项式的系数
        int m = pBreaks->cols()-1; // 矩阵的行数,也即分片多项式的数量
        int n = (pCoefs->cols()) / m; // 矩阵的列数,即分片多项式的阶数
        int dim = 1; // 样条曲线的维数,这里只能为1

        auto *coefs = new baltam::matrix<double>(m, n);
        for (int i = 0; i < m; i++) {
          for (int j = 0; j < n; j++) {
	      (*coefs)(i,j) = (*pCoefs)(k,i*n+j);
            }
        }
        res->set_field("coefs", new ba_obj(ba_double_mat, coefs));

        // pieces 分片多项式的数量
        res->set_field("pieces", new ba_obj(static_cast<int64_t>(m)));

        // order 分片多项式的阶数
        res->set_field("order", new ba_obj(static_cast<int64_t>(n)));

        // dim 样条曲线的维数
        res->set_field("dim", new ba_obj(static_cast<int64_t>(dim)));

	return res;
    }

    // 第三种情形 [pp1,pp2,...] = ppmak(breaks,coefs)
    void ppmak_case3(std::vector<const_ba_obj_rawptr> &in_args, std::vector<ba_obj_rawptr> &out_args) {
        const_ba_obj_rawptr breaksTemp = in_args[0];
        const_ba_obj_rawptr coefsTemp = in_args[1];
	const_ba_obj_ptr pb_smart_ptr, pc_smart_ptr;
	spline_parameter_check_double(in_args[0],breaksTemp,pb_smart_ptr);
	spline_parameter_check_double(in_args[1],coefsTemp,pc_smart_ptr);

        auto pBreaks = breaksTemp->get<matrix<double>>();
        auto pCoefs = coefsTemp->get<matrix<double>>();

        // 对输入参数的合理性进行检查
	int t1 = pCoefs->cols();
	int t2 = pBreaks->cols()-1;
        if ((t1%t2) != 0)
            throw std::invalid_argument{"输入参数不匹配，length(breaks)-1 必须整除 coefs 的列数。"};
	
        // 对输出参数的合理性进行检查，输出参数的个数必须等于系数矩阵的行数
	int t = pCoefs->rows();
	int max_num = out_args.size();
	if(t!=max_num)
	    throw std::invalid_argument{"输出参数个数必须与 coefs 的行数相同"};
	
	for(int k=0; k<max_num; k++){
	    *out_args[k] = ba_obj(ba_struct, generate_one_dimension(pBreaks,pCoefs,k));
	}
    }

    void proto_ppmak_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
	
        // 检查参数个数，输入参数个数区间为[2,3]，输出参数个数区间为[1,INT_MAX]
	// 在这里先不对输出参数做限制，在下面具体执行过程中再判断输出参数个数是否正确
        BALTAM_PARAM_CHECK(2, 3, 1, INT_MAX)

        // 根据输入参数的个数选择执行不同的函数
        if (in_args.size() == 3)
            ppmak_case2(in_args, out_args); // 输入参数为3,从而执行 case2
        else {                              // 因为已经做过参数个数检查，所以这里输入参数个数只能为2
	    if(out_args.size() == 1){
		ppmak_case1(in_args, out_args);  // 输出参数个数等于1,从而执行 case1
	    }else{
		ppmak_case3(in_args, out_args);  // 输出参数个数大于1,执行case3,参数个数的合理性在case3内部进行检查
	    }
	}
    }
}


namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(ppmak)

    REGISTER_EXPORT_FUNCTION(ppmak, ppmak, g_pPpmakHelp)
}
