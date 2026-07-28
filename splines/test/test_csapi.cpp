/*
 * ==========================================================================
 *
 *       Filename:  test_csapi.cpp
 *
 *    Description:  test for csapi
 *
 *        Version:  1.0
 *	      Created:  2022-12-16 18:17:38
 *       Revision:
 *       Compiler:  g++
 *
 *         Author:  Jingang Zhou, jingang.zhou@cqbdri.pku.edu.cn
 *      Copyright:  Copyright (c) 2022, Jingang Zhou
 *
 * ==========================================================================
 */

#include "csapi/csapi.h"
#include "print/obj2str.h"
#include "ba_obj/matrix.h"

using namespace baltam;
using namespace splines;
using baltam::internal::obj2str;

int main() {

	auto ret1(new ba_obj());
	/** objA 3*3 矩阵 */
	auto p1 = new baltam::matrix<double>(1, 5);
	*p1 << 0,1,2,3,4;
	auto objX(new ba_obj(ba_double_mat,p1));

	std::cout<< ">> x = ";
	std::cout<< obj2str(*objX);

    /** objA 3*3 矩阵 */
    auto p2 = new baltam::matrix<double>(1, 5);
    *p2 << 2.5,1.5,0.5,0.5,1.5;
    auto objY(new ba_obj(ba_double_mat,p2));

    std::cout<< ">> y = ";
    std::cout<< obj2str(*objY);

	std::vector<const_ba_obj_rawptr > in_args(2);
    in_args[0]=objX;
    in_args[1]=objY;
	std::vector<ba_obj_rawptr > out_args(1);
	out_args[0]=ret1;

    proto_csapi_baltam(in_args, out_args);

    std::cout<< ">> s=csapi(x,y) = ";
    std::cout<< obj2str(*out_args[0]);

//    for (auto &in : in_args) {
//        delete in;
//    }
//    for (auto &out : out_args) {
//        delete out;
//    }
	return 0;
}
