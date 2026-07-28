/*
 * ==========================================================================
 *
 *       FileName:  csape.cpp
 *
 *    Description:  source file for csape
 *
 *        Version:  1.0
 *        Created:  2022.3.9
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  shaozhen in ZJU
 *      Copyright:  non
 *
 * ==========================================================================
 */

#include "csapi/csapi.h"
#include "csape.h"
#include "Splines/Spline.h"
#include "ba_obj/ba_obj.h"
#include "ba_obj/matrix.h"
#include "proto/param_checker.h"
#include "private/utils.h"
#include "BasicHeadFile/PolyInterpolation.h"
#include "query/query.h"


namespace baltam::splines {
    void csape_check_parameters(std::vector<const_ba_obj_rawptr> &in_args);

    double csape_left_default_value(const std::vector<double> &x, const std::vector<double> &y);

    double csape_right_default_value(const std::vector<double> &x, const std::vector<double> &y);

    void proto_csape_baltam(std::vector<const_ba_obj_rawptr> &in_args,
                            std::vector<ba_obj_rawptr> &out_args) {
        // 检查参数个数，输入参数个数区间为[2,3]，输出参数个数区间为[1,1]
        BALTAM_PARAM_CHECK(2, 3, 1, 1)
        // --------------------------- 2 个输入参数 -----------------------------------------
        // 若输入参数个数为2，则理解为 csape(x,y)形式，递交给 csapi 函数处理
        if (static_cast<int>(in_args.size()) == 2) {
            proto_csapi_baltam(in_args, out_args);
            return; // 递交给 csapi 函数后本函数直接结束
        }
        // --------------------------- 3 个输入参数 ----------------------------------------
        // 以下处理输入参数个数为3的情形，此时函数被理解为
        // caspe(x, [e1 y e2], conds) 或者 csape(x,y,conds)

        // ----------------------------- 参数检查 -------------------------------------------

        csape_check_parameters(in_args);

        // ----------------------------- 函数主体 -------------------------------------------

        auto pX = in_args[0]->get<matrix<double>>();
        auto pY = in_args[1]->get<matrix<double>>();

        InterpCondition c{};  // 插值条件，其具体的值根据输入参数确定
        // 输入纵坐标向量大小是输入横坐标向量大小加2，说明用户提供了边界条件值
        bool boundary_condition_provided = ((pX->cols() + 2) == pY->cols());
        std::vector<double> x_left_default; // 左侧边界条件默认值所需的横坐标
        std::vector<double> y_left_default; // 左侧边界条件默认值所需的纵坐标
        std::vector<double> x_right_default;// 右侧边界条件默认值所需的横坐标
        std::vector<double> y_right_default;// 右侧边界条件默认值所需的纵坐标

        if (!boundary_condition_provided) {
            // 输入横纵坐标向量大小相同时，即没有指定两端的边界条件值时，输入纵坐标向量就是插值纵坐标
            for (int i = 0; i < pX->cols(); ++i) {
                c.sites.push_back((*pX)[i]);                     // 将 pX 数据放入插值条件的横坐标中
                c.function_values.push_back((*pY)[i]);           // 将 pY 数据放入插值条件的纵坐标(函数值)中
            }
            for (int i = 0; i <= 3; ++i) {
                x_left_default.push_back((*pX)[i]);              // 横坐标的前四个
                y_left_default.push_back((*pY)[i]);              // 纵坐标的前四个
                x_right_default.push_back((*pX)[pX->cols() - 1 - i]);  // 横坐标的后四个
                y_right_default.push_back((*pY)[pY->cols() - 1 - i]);  // 纵坐标的后四个
            }
            c.derivative1 = csape_left_default_value(x_left_default,y_left_default);
            c.derivative2 = csape_right_default_value(x_right_default,y_right_default);
        } else {
            // 当指定了两端的边界条件值时，插值纵坐标为输入纵坐标向量除去首尾元素之后的向量
            for (int i = 0; i < pX->cols(); ++i) {
                c.sites.push_back((*pX)[i]);                     // 将 pX 数据放入插值条件的横坐标中
                c.function_values.push_back((*pY)[i + 1]);       // 将 pY 数据放入插值条件的纵坐标(函数值)中
            }
            c.derivative1 = (*pY)[0];                            // 输入纵坐标向量第一个元素作为左端边界条件值
            c.derivative2 = (*pY)[pY->cols() - 1];               // 输入纵坐标向量最后一个元素作为右端边界条件值

            for (int i = 0; i <= 3; ++i) {
                x_left_default.push_back((*pX)[i]);                    // 横坐标的前四个
                y_left_default.push_back((*pY)[i + 1]);                // 纵坐标忽略边界条件后的前四个
                x_right_default.push_back((*pX)[pX->cols() - 1 - i]);  // 横坐标的后四个
                y_right_default.push_back((*pY)[pY->cols() - 2 - i]);  // 纵坐标忽略边界条件后的后四个
            }
        }

        if (in_args[2]->is_convertable_to_string()) 
        {
            std::string cond = in_args[2]->as_string();
            if (cond == "complete" || cond == "clamped") // 完全三次样条
            {
                c.left = BCType::complete;
                c.right = BCType::complete;
                if (!boundary_condition_provided) // 用户未输入边界条件值就以默认值计算
                {
                    c.derivative1 = csape_left_default_value(x_left_default,y_left_default); 
                    c.derivative2 = csape_right_default_value(x_right_default,y_right_default);
                }
            } else if (cond == "not-a-knot") // 非节点边界条件，即使用户输入边界条件值，也直接忽略
            {
                c.left = BCType::not_a_knot;
                c.right = BCType::not_a_knot;
            } else if (cond == "second") // 这个边界条件(specified second derivative)好像没有中文名字，用户未输入边界条件值就以默认值0计算
            {
                c.left = BCType::specified_2nd;
                c.right = BCType::specified_2nd;
                if (!boundary_condition_provided) {
                    c.derivative1 = 0;
                    c.derivative2 = 0;
                }
            } else if (cond == "periodic") // 周期边界条件，即使用户输入边界条件值，也直接忽略
            {
                c.left = BCType::periodic;
                c.right = BCType::periodic;
            } else if (cond == "variational") // 自然边界条件，即使用户输入边界条件值，也直接以0处理
            {
                c.left = BCType::specified_2nd;
                c.right = BCType::specified_2nd;
                c.derivative1 = 0;
                c.derivative2 = 0;
            } else
                throw std::invalid_argument{"未定义的边界类型。"};
        } else {
            // 第三个参数是矩阵形式，可以指定混合边界条件
            auto cond = in_args[2]->get<matrix<double>>();
            // 运行至此说明第三个参数是一个一行两列的矩阵
            if ((*cond)[0] != 0 &&                //左侧边界条件取默认值（最左侧生成的第一个三次样条）
                (*cond)[0] != 1 &&
                (*cond)[0] != 2) {
                c.left = BCType::complete;
                c.derivative1 = csape_left_default_value(x_left_default,y_left_default);
                std::string str = "不合法的边界条件值： ";
                str.append(std::to_string((*cond)[0]));
                str.append(" ，其被视为 1 且左侧边界条件值取默认值。");
                baltam::write_to_cout(str);

            }

            if ((*cond)[1] != 0 &&                 //右侧边界条件取默认值（最左侧生成的第一个三次样条）
                (*cond)[1] != 1 &&
                (*cond)[1] != 2) {
                c.right = BCType::complete;
                c.derivative2 = csape_right_default_value(x_right_default,y_right_default);


                std::string str = "不合法的边界条件值： ";
                str.append(std::to_string((*cond)[1]));
                str.append(" ，其被视为 1 且右侧边界条件值取默认值。");
                baltam::write_to_cout(str);
            }
            // 通过强制类型转化设置两端边界条件，BCType是一个枚举类型，其元素排序为
            // periodic, complete, specified_2nd, natural, not_a_knot 强制类型转化可以完成对应
            c.left = static_cast<BCType>((*cond)[0]);
            c.right = static_cast<BCType>((*cond)[1]);
            // 运行至此说明第三个参数是一个一行两列的矩阵，元素为0，1，2
            // 排除周期边界条件与其他边界条件混用的情况（混用就取默认边界条件）
            if ((*cond)[0] == 0) {
                if ((*cond)[1] != 0) // 说明输入是[0,1]或[0,2]
                {
                    c.left = BCType::complete;
                    c.derivative1 = csape_left_default_value(x_left_default,y_left_default);
                    std::string str = "周期边界条件 0 不可与其他边界条件混用，输入参数 : [";
                    str.append(std::to_string((*cond)[0]));
                    str.append(" , ");
                    str.append(std::to_string((*cond)[1]));
                    str.append("] 被视为 [1 , ");
                    str.append(std::to_string((*cond)[1]));
                    str.append( "], 且左侧边界条件值取默认值。");
                    baltam::write_to_cout(str);
                }
            }
            if ((*cond)[1] == 0) {
                if ((*cond)[0] != 0) // 说明输入是[1,0]或[2,0]
                {
                    c.right = BCType::complete;
                    c.derivative2 = csape_right_default_value(x_right_default,y_right_default);

                    std::string str = "周期边界条件 0 不可与其他边界条件混用，输入参数 : [";
                    str.append(std::to_string((*cond)[0]));
                    str.append(" , ");
                    str.append(std::to_string((*cond)[1]));
                    str.append("] 被视为[");
                    str.append(std::to_string((*cond)[1]));
                    str.append( ", 1], 且右侧边界条件值取默认值。");
                    baltam::write_to_cout(str);

                }
            }
        }
        auto S = Spline<3, SplineType::ppForm>{c}; //生成ppForm形式的三次样条
        *out_args[0] = ba_obj(ba_struct, S.spline_to_baltam_structure());

    } // end of proto_csape_baltam

    void csape_check_parameters(std::vector<const_ba_obj_rawptr> &in_args){
        const_ba_obj_rawptr xTemp = in_args[0];
        const_ba_obj_rawptr yTemp = in_args[1];
        const_ba_obj_rawptr cond_Temp = in_args[2];
        const_ba_obj_ptr pX_smart_ptr;
        const_ba_obj_ptr pY_smart_ptr; // 智能指针，防止内存泄露
        const_ba_obj_ptr pCond_smart_ptr;
        // 检查输入的前两个 x，y 参数是否为 double 类型矩阵，或者是否可以转化为 double 类型矩阵，若均不可以，提示输入参数错误
        try {
            // 如果不是double类型矩阵，就转化为double类型矩阵，转化不成功会抛出异常
            if (in_args[0]->type() != ba_double_mat) {
                // 这里转化失败会返回一个空指针
                xTemp = internal::matrix_numeric_cast(in_args[0], ba_double_mat);
                if (xTemp == nullptr)
                    throw std::invalid_argument{"仅支持实数数值矩阵的插值点输入。"};
                pX_smart_ptr = const_ba_obj_ptr(xTemp);
            }

            if (in_args[1]->type() != ba_double_mat) {
                yTemp = internal::matrix_numeric_cast(in_args[1], ba_double_mat);
                if (yTemp == nullptr)
                    throw std::invalid_argument{"仅支持实数数值矩阵的插值点输入。"};
                pY_smart_ptr = const_ba_obj_ptr(yTemp);
            }
        }
        catch (std::invalid_argument &) {
            throw std::invalid_argument("仅支持实数数值矩阵的插值点输入。");
        }

        // 检查横坐标向量大小，目前仅支持至少为4
        // 检查横坐标向量和纵坐标向量大小是否兼容，仅接受两种情况： x.size = y.size OR x.size + 2 = y.size
        auto pX = xTemp->get<matrix<double>>();
        auto pY = yTemp->get<matrix<double>>();
        if (pX->cols() < 4)  
            throw std::invalid_argument{"插值节点过少，请至少输入 四个节点。"};
        if (pX->cols() != pY->cols() && (pX->cols() + 2) != pY->cols()) {
            std::string error_message{"插值横坐标向量大小: "};
            error_message += std::to_string(static_cast<int>(pX->cols()));
            error_message += " 和纵坐标向量大小: ";
            error_message += std::to_string(static_cast<int>(pY->cols()));
            error_message += " 不兼容。";
            throw std::invalid_argument{error_message};
        }

        // 检查输入的第三个 conds 参数是否为 1*2 int 类型矩阵或者字符串，若均不是，提示输入参数错误
        std::string cond{};
        try {
            std::string cond = cond_Temp->as_string(); // 不是 string 类型会抛出异常，进入以下 catch 分支
        }
        catch (std::invalid_argument &) {
            // 运行至此表示第三个参数不是字符串，则检查其是否为 1*2 int 类型矩阵
            if (cond_Temp->type() != ba_double_mat)
                throw std::invalid_argument{"第三个参数必须为 字符串类型 或 1*2 int 类型矩阵。"};
            // 运行至此表示第三个参数是 int 类型矩阵，再检查其大小和元素取值合法性
            auto pCond = cond_Temp->get<matrix<double>>();
            if (pCond->rows() != 1 || pCond->cols() != 2) {
                // 第三个参数是矩阵，但是行数不为1 或者 列数不是2
                //（列数为1不可接受，只能接受1*2的矩阵）
                std::string error_message{"第三个参数为矩阵时，其大小必须为 1*2 ，输入参数大小："};
                error_message += std::to_string(pCond->rows());
                error_message += " * ";
                error_message += std::to_string(pCond->cols());
                throw std::invalid_argument{error_message};
            }
        }
    } // end of csape_check_parameters

    double csape_left_default_value(const std::vector<double> &x, const std::vector<double> &y) {
        NewtonInterpolation N(x,y);
        auto temp =x;
        std::sort(temp.begin(),temp.end());
        return N.diff(temp[0]);
    }

    double csape_right_default_value(const std::vector<double> &x, const std::vector<double> &y) {
        NewtonInterpolation N(x,y);
        auto temp = x;
        std::sort(temp.begin(),temp.end());
        return N.diff(temp.back());
    }


} // end of namespace baltam::splines







static const char *g_pCsapeHelp = R"(
csape 函数根据给定的插值点列和边界条件信息生成对应的三次样条函数(ppForm 形式。

    csape 函数的用法为 S = csape(x, [e1 y e2], conds),其中
        • x y 分别为样条插值坐标点的横纵坐标构成的向量(建议将 x 进行单增排列后作为参数输入);
        • e1 e2 分别为样条插值在左右两侧的边界条件值(根据 conds 的取值将会被理解为不同的含义,甚至被忽略);
        • conds 指定样条插值的边界条件,其值可以为字符串和 1 × 2 int 类型矩阵；
        • S 为一个结构体,包含生成的三次样条函数的节点,分片多项式系数等信息。
    (下文中左侧指在样条函数的第一个节点处,右侧指在最后一个节点处)

    I. 当不指定 conds 时,即使用 S = csape(x, y) 时,函数将默认在两侧使用 not-a-knot 边界条件,函数行为与 csapi 相同。

    II. 当 conds 为字符串时,其对应的取值和意义如下:
        "complete" or "clamped" : 指定样条函数左右两侧的一阶导数分别为 e1 e2，
                                  若未提供 e1 e2 的输入,将会以“默认值”进行计算。
        "not-a-knot"            : 样条函数在第二个节点和导数第二个节点三次连续，
                                  此边界条件会忽略输入的 e1 e2。
        "second"                : 指定样条函数左右两侧的二阶导数分别为 e1 e2，若未提供 e1 e2 的输入，
                                  将会默认 e1 = e2 = 0,此情形下函数行为与边界条件为 "variational"时相同。
        "periodic"              : 指定样条函数左右两侧的一阶导数和二阶导数均相等。
        "variational"           : 指定样条函数左右两侧的二阶导数均为 0, 此边界条件会忽略输入的 e1 e2。

    其他字符串内容(大小写敏感)会被认定为非法输入。“默认值”说明详见下文。

    III. 当 conds 为 1 × 2 int 类型矩阵时,矩阵元素只能取值于 0,1,2,当出现其他取值的矩阵元素
         或未指定指的矩阵元素时,函数会将对应的元素认定为 1 并以“默认值”进行计算。例如:
        S = csape(x, [e1 y e2], [-1 2]) 等价于 S = csape(x, [default y e2], [1 2]);
        S = csape(x, [e1 y e2], [2 , ]) 等价于 S = csape(x, [e1 y default], [2 ,1])

    0,1,2 取值分别对应于 "periodic","complete","second",意为指定两侧对应阶数的导数值,例如:
        S = csape(x, [e1 y e2], [1 2]) 将会指定三次样条函数 S 左侧一阶导数为 e1,右侧二阶导数为 e2;
        S = csape(x, [e1 y e2], [2 1]) 将会指定三次样条函数 S 左侧二阶导数为 e1,右侧一阶导数为 e2;

    "periodic"边界条件即矩阵元素 0 不能与其他条件混用,当 conds 矩阵中出现 0 元素时，
     必须两个元素均为 0,否则为 0 的元素将被视为 1 并以“默认值”进行计算。例如:
        S = csape(x, [e1 y e2], [0 0]) 等价于 S = csape(x, [e1 y e2], "periodic");
        S = csape(x, [e1 y e2], [0 1]) 等价于 S = csape(x, [default y e2], [1 1]);
        S = csape(x, [e1 y e2], [2 0]) 等价于 S = csape(x, [e1 y default], [2 1]);

    关于“默认值”的说明:当函数输入参数错误或者不足时,函数会自动启动默认值计算。以左侧为例,默认计算指
    将样条函数左侧的一阶导数值设定为前四个插值点唯一确定的三次多项式在第一个节点处的一阶导数值,右侧同理。
)";

namespace baltam::splines {
    GENERATE_SPLINES_PLUGIN_FUNC(csape)

    REGISTER_EXPORT_FUNCTION(csape, csape, g_pCsapeHelp)
}
