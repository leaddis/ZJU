#include "Spline.h"
#include "SplineHelper.h"

using Poly2Dim = Polynomial<2, Vec<double, 2>>;
using Poly4Dim = Polynomial<4, Vec<double, 2>>;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
	Spline<2, 2, ppForm> pps1;
	Vec<double, 2> v1({1, 2.3});
	Vec<double, 2> v2({-1.2, 0});
	Vec<double, 2> v3({0, 0.6});
	Vec<double, 2> v4({-2, -2.4});	
	Poly2Dim p1({v1, v2});
	Poly2Dim p2({v3, v4});
	Spline<2, 2, ppForm> pps2(p1, 2, 1);
	pps2.addknot(p2, 1.3);
	const std::vector<double> &vk2 = pps2.getknots();
	const std::vector<Poly2Dim> &vp2 = pps2.getpolys();
	cout << "-----------knots and polys of pps2:------------" << endl;
	for (int i = 0; i < (int)vp2.size(); i++)
	{
		cout << "[" << vk2[i] << "," << vk2[i + 1] << "]: " << vp2[i] << endl;
	}
	Vec<double, 2> v5({7.7, 2.6});
	Vec<double, 2> v6({-2, -1.4});
	Poly2Dim p3({v5, v6});
	pps1.addknot(p3, 0.2);
	pps1.addcurve(pps2);
	cout << "-----------knots and polys of pps1:-----------" << endl;
	pps1.show();
	cout << "Test for locatepiece:" << endl;
	cout << "locate 0.4 in pps1: " << pps1.locatePiece(0.4) << endl;
	cout << "locate 0.1 in pps1: " << pps1.locatePiece(0.1) << endl;
	cout << "locate 3.5 in pps1: " << pps1.locatePiece(3.5) << endl;
	cout << "locate 1 in pps2: " << pps2.locatePiece(1) << endl;
	cout << "-----------Test for operator ():-----------" << endl;
	cout << "pps1(0.2) = " << pps1(0.2) << endl;
	cout << "pps1(1) = " << pps1(1) << endl;
	cout << "pps2(1) = " << pps2(1) << endl;
	cout << "pps1(3) = " << pps1(3) << endl;
	cout << "-----------Test for fnplt: -----------" << endl;
	cout << "run Outputfile/test1.m and Outputfile/test2.m by matlab." << endl;
	fnplt(pps2, "Outputfile/test1.m");
	Vec<double, 2> vv1({0, 0.6});
	Vec<double, 2> vv2({-2, -2.4});
	Vec<double, 2> vv3({-1, -0.4});
	Vec<double, 2> vv4({-2, 2.4});
	Vec<double, 2> vv5({1, 0.2});
	Vec<double, 2> vv6({0.4, 2});
	Vec<double, 2> vv7({-1, -0.4});
	Vec<double, 2> vv8({-5, 0.2});
	Poly4Dim pp1({vv1, vv2, vv3, vv4});
	Poly4Dim pp2({vv5, vv6, vv7, vv8});
	Spline<2, 4, ppForm> pps3(pp1, 1);
	pps3.addknot(pp2, 1.3);
	cout << "While pps3:" << endl;
	pps3.show();
	fnplt(pps3, "Outputfile/test2.m");
	cout << "-----------Test for fitCurve with order 2:-----------" << endl;
	std::vector<Vec<double, 2>> svv1(6);
	svv1[0] = vv1;
	svv1[1] = vv3;
	svv1[2] = vv4;
	svv1[3] = vv8;
	svv1[4] = vv6;
	svv1[5] = vv5;
	Spline<2, 2, ppForm> sp1 = fitCurve<2, 2>(svv1);
	cout << "sp1:" << endl;
	sp1.show();
	fnplt(sp1, "Outputfile/test3.m");
	cout << "run Outputfile/test3.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , periodic condition:-----------" << endl;
	Vec<double, 2> vpd1({0, 0.6});
	Vec<double, 2> vpd2({1, 2.4});
	Vec<double, 2> vpd3({3, -0.4});
	Vec<double, 2> vpd4({2, -1});
	Vec<double, 2> vpd5({-1, 0.2});
	Vec<double, 2> vpd6({-0.5, 0.4});
	Vec<double, 2> vpd7({0, 0.6});
	std::vector<Vec<double, 2>> svv2(7);
	svv2[0] = vpd1;
	svv2[1] = vpd2;
	svv2[2] = vpd3;
	svv2[3] = vpd4;
	svv2[4] = vpd5;
	svv2[5] = vpd6;
	svv2[6] = vpd7;
	Spline<2, 4, ppForm> sp2 = fitCurve<2, 4>(svv2, periodic);
	cout << "sp2:" << endl;
	sp2.show();
	fnplt<4>(sp2, "Outputfile/test4.m", 500, 0, 1);
	cout << "run Outputfile/test4.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , complete condition:-----------" << endl;
	Vec<double, 2> vpd8({4, 0});
	Vec<double, 2> vpd9({5.5, -1});
	Vec<double, 2> vpd10({6, 2});
	std::vector<Vec<double, 2>> svv3(6);
	svv3[0] = vpd1;
	svv3[1] = vpd2;
	svv3[2] = vpd3;
	svv3[3] = vpd8;
	svv3[4] = vpd9;
	svv3[5] = vpd10;
	Vec<double, 2> left1st({1, 1});
	Vec<double, 2> right1st({0, 2});
	Spline<2, 4, ppForm> sp3 =
		fitCurve<2, 4>(svv3, complete, left1st, right1st);
	cout << "sp3: 1st der at start = (1,1), 1st der at end = (0,2)" << endl;
	sp3.show();
	fnplt(sp3, "Outputfile/test5.m", 500);
	cout << "run Outputfile/test5.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , second condition:-----------" << endl;
	Vec<double, 2> vpd11({-1, -1});
	Vec<double, 2> vpd12({0, -2.5});
	Vec<double, 2> vpd13({7, 6});
	std::vector<Vec<double, 2>> svv4(8);
	svv4[0] = vpd12;
	svv4[1] = vpd6;
	svv4[2] = vpd7;
	svv4[3] = vpd4;
	svv4[4] = vpd10;
	svv4[5] = vpd13;
	svv4[6] = vpd2;
	svv4[7] = vpd3;
	Vec<double, 2> left2nd({-1, -1});
	Vec<double, 2> right2nd({0, -1});
	Spline<2, 4, ppForm> sp4 =
		fitCurve<2, 4>(svv4, second, left2nd, right2nd);
	cout << "sp4: 2nd der at start = (-1,-1), 2nd der at end = (0,-1)" << endl;
	sp4.show();
	fnplt(sp4, "Outputfile/test6.m", 500);
	cout << "run Outputfile/test6.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , notAknot condition:-----------" << endl;
	Spline<2, 4, ppForm> sp5 =
		fitCurve<2, 4>(svv4, notAknot);
	cout << "sp5: the same points as sp4" << endl;
	sp5.show();
	fnplt(sp5, "Outputfile/test7.m", 500);
	cout << "run Outputfile/test7.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , mixed condition:-----------" << endl;
	Vec<double, 2> left0nd({0, 0});
	Spline<2, 4, ppForm> sp6 =
		fitCurve<2, 4>(svv4, mixed, left0nd, right2nd, std::make_pair(0, 2));
	cout << "sp6: the same points as sp4, using not-a-knot condition at\
  start and 2nd der at end = (0,-1)."
		 << endl;
	sp6.show();
	fnplt(sp6, "Outputfile/test8.m", 500);
	cout << "run Outputfile/test8.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 2 , Dim =\
  1:-----------"
		 << endl;
	std::vector<Vec<double, 2>> svv5(7);
	Vec<double, 2> vpd14({5, 0.2});
	svv5[0] = vv8;
	svv5[1] = vv2;
	svv5[2] = vv3;
	svv5[3] = vv6;
	svv5[4] = vv5;
	svv5[5] = vpd8;
	svv5[6] = vpd14;
	Spline<1, 2, ppForm> sp7 = fitCurve<1, 2>(svv5);
	cout << "sp7:" << endl;
	sp7.show();
	fnplt<2>(sp7, "Outputfile/test9.m");
	cout << "run Outputfile/test9.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , periodic\
  condition, Dim = 1:-----------"
		 << endl;
	Spline<1, 4, ppForm> sp8 = fitCurve<1, 4>(svv5, periodic);
	cout << "sp8:" << endl;
	sp8.show();
	fnplt(sp8, "Outputfile/test10.m");
	cout << "run Outputfile/test10.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , not-a-knot\
  condition, Dim = 1:-----------"
		 << endl;
	Spline<1, 4, ppForm> sp9 = fitCurve<1, 4>(svv5);
	cout << "sp9:" << endl;
	sp9.show();
	fnplt(sp9, "Outputfile/test11.m");
	cout << "run Outputfile/test11.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 , mixed\
  condition, Dim = 1:-----------"
		 << endl;
	const double Left1st = -2;
	const double Right2nd = 0;
	Spline<1, 4, ppForm> sp10 = fitCurve<1, 4>(svv5, mixed, Left1st, Right2nd, std::make_pair(1, 2));
	cout << "sp10:" << endl;
	sp10.show();
	fnplt(sp10, "Outputfile/test12.m");
	cout << "run Outputfile/test12.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 2 by\
  InterpConditions-----------"
		 << endl;
	InterpConditions<double, 1> IC1("Inputfile/testfile/test3");
	Spline<1, 2, ppForm> sp11 = interpolate<2, 1, ppForm>(IC1);
	cout << "sp11:" << endl;
	sp11.show();
	fnplt(sp11, "Outputfile/test13.m");
	cout << "run Outputfile/test13.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 by\
  InterpConditions, not-a-knot condition-----------"
		 << endl;
	Spline<1, 4, ppForm> sp12 = interpolate<4, 1, ppForm>(IC1);
	cout << "sp12:" << endl;
	sp12.show();
	fnplt(sp12, "Outputfile/test14.m");
	cout << "run Outputfile/test14.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 by\
  InterpConditions, complete condition-----------"
		 << endl;
	InterpConditions<double, 2> IC2("Inputfile/testfile/test1");
	Spline<1, 4, ppForm> sp13 = interpolate<4, 2, ppForm>(IC2, complete);
	cout << "sp13:" << endl;
	sp13.show();
	fnplt(sp13, "Outputfile/test15.m", 1000);
	cout << "run Outputfile/test15.m by matlab." << endl;
	cout << "-----------Test for fitCurve with order 4 by\
  InterpConditions, mixed condition-----------"
		 << endl;
	InterpConditions<double, 3> IC3("Inputfile/testfile/test2");
	Spline<1, 4, ppForm> sp14 = interpolate<4, 3, ppForm>(IC3, mixed, std::make_pair(2, 1));
	cout << "sp14:" << endl;
	sp14.show();
	fnplt(sp14, "Outputfile/test16.m", 1000);
	cout << "run Outputfile/test16.m by matlab." << endl;
	cout << "-----------Test for fnval: -----------" << endl;
	cout << "fnval(sp6,0) = " << fnval(sp6, 0) << endl;
	cout << "fnval(sp6,1.6) = " << fnval(sp6, 1.6) << endl;
	cout << "fnval(sp8,-2) = " << fnval(sp8, -2) << endl;
	cout << "fnval(sp14,0) = " << fnval(sp14, 0) << endl;
	std::vector<double> svd5{0, 0.4, 0.8, 1.2, 1.6};
	cout << "for x = [ ";
	for (auto x : svd5)
		cout << x << " , ";
	cout << "\b\b]," << endl;
	cout << "fnval(sp6,x) = ";
	std::vector<Vec<double, 2>> rsvd5 = fnval(sp6, svd5);
	for (auto x : rsvd5)
		cout << x << " , ";
	cout << "\b\b]," << endl;
	cout << "-----------Test for fnder: -----------" << endl;
	cout << "sp6:" << endl;
	sp6.show();
	cout << "fnder(sp6):" << endl;
	fnder(sp6).show();
	cout << "sp5:" << endl;
	sp5.show();
	cout << "fnder(sp5):" << endl;
	fnder(sp5).show();
	fnplt(fnder(sp5), "Outputfile/Der_test7.m");
	cout << "fnder(fnder(sp5)):" << endl;
	fnder(fnder(sp5)).show();
	fnplt(fnder(fnder(sp5)), "Outputfile/Der2_test7.m");
	cout << "-----------Test for bspline:-----------" << endl;
	std::vector<double> vv9(8);
	vv9[0] = 0;
	vv9[1] = 0;
	vv9[2] = 1.2;
	vv9[3] = 0.4;
	vv9[4] = 1.2;
	vv9[5] = 2;
	vv9[7] = 2;
	vv9[6] = 2;
	cout << "For knots: [ ";
	for (auto it = vv9.begin(); it != vv9.end(); it++)
		cout << *it << " ";
	cout << "]" << endl;
	Bspline<1, 7> Bs8(vv9);
	Spline<1, 7, ppForm> psp1 = bspline(Bs8);
	psp1.show();
	fnplt(psp1, "Outputfile/bspline_test5_ppform.m");
	cout << "-----------Test for * and /:-----------" << endl;
	cout << "sp12:" << endl;
	sp12.show();
	cout << "sp12 * 2.5:" << endl;
	(sp12 * 2.5).show();
	cout << "sp4: " << endl;
	sp4.show();
	cout << "sp4 / -1.5: " << endl;
	(sp4 / (-1.5)).show();
	cout << "-----------Test for + and -:-----------" << endl;
	cout << "sp12:" << endl;
	sp12.show();
	cout << "sp10:" << endl;
	sp10.show();
	cout << "sp12 + sp10 :" << endl;
	(sp12 + sp10).show();
	cout << "sp12 - sp10*2 :" << endl;
	(sp12 - sp10 * 2).show();
}
