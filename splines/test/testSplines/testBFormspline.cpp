#include "Spline.h"
#include "SplineHelper.h"

using std::cout;
using std::endl;
template<int Order> using Bvec = std::vector<Bspline<1,Order> >;

int main(int argc, char* argv[]){
  cout << "--------------Test for basis function:--------------" << endl;
  Spline<1,3,BForm> BS1;
  std::vector<double> vv3(5);
  vv3[0] = 0;vv3[1] = 1;vv3[2] = 1; vv3[3] = 2;vv3[4] = 3;
  Bspline<1,4> B1(vv3);
  cout << "Bspline B1:" << endl;
  B1.show();
  std::vector<double> vv4(5);
  vv4[0] = 1;vv4[1] = 1;vv4[2] = 2; vv4[3] = 3;vv4[4] = 6;
  Bspline<1,4> B2(vv4);
  cout << "Bspline B2:" << endl;
  B2.show();
  Bvec<4> Bv1;
  Bv1.push_back(B1);
  Bv1.push_back(B2);
  std::vector<double> coe1;
  coe1.push_back(1);
  coe1.push_back(-2.3);
  Spline<1,4,BForm> BS2(Bv1,coe1);
  const Bvec<4>& Bv2 = BS2.getbasis();
  const std::vector<double>& coe2 = BS2.getcoes();
  cout << "First Bspline of BS2(B1):" << endl;
  Bv2[0].show();
  cout << "Coes of BS2:" << endl;
  cout << coe2[0] << "," << coe2[1] << endl;
  cout << "--------------Test for addbasis:--------------" << endl;
  std::vector<double> vv5(7);
  vv5[0] = 0;vv5[1] = 1;vv5[2] = 1; vv5[3] = 2;vv5[4] = 3;
  vv5[5] = 6;vv5[6] = 6;
  cout << "For knots: [ ";
  for (auto it = vv5.begin(); it != vv5.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Spline<1,4,BForm> BS3;
  BS3.addbasis(vv5);
  const Bvec<4>& Bv3 = BS3.getbasis();
  int i = 1;
  for (auto it = Bv3.cbegin(); it != Bv3.cend() ; it++){
    cout << "Basis " << i << " of BS3:" << endl;
    (*it).show();
    i++;
  }
  std::vector<double> vv6(9);
  vv6[0] = 0;vv6[1] = 0;vv6[2] = 0; vv6[3] = 0;vv6[4] = 1;
  vv6[5] = 2;vv6[6] = 2;vv6[7] = 2; vv6[8] = 2;
  cout << "For knots: [ ";
  for (auto it = vv6.begin(); it != vv6.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Spline<1,4,BForm> BS31;
  BS31.addbasis(vv6);
  const Bvec<4>& Bv31 = BS31.getbasis();
  i = 1;
  for (auto it = Bv31.cbegin(); it != Bv31.cend() ; it++){
    cout << "Basis " << i << " of BS31:" << endl;
    (*it).show();
    i++;
  }
  cout << "--------------Test for fitcurve:--------------" << endl;
  std::vector<double> knots0 = vv6;
  std::vector<double> x0(5);
  std::vector<double> y0(5);
  x0[0] = 0;x0[1] = 1;x0[2] = 1;x0[3] = 1;x0[4] = 2;
  y0[0] = 2;y0[1] = 0;y0[2] = 1;y0[3] = 2;y0[4] = -1;
  Spline<1,4,BForm> BS0 = fitCurve<4>(knots0,x0,y0);
  cout << "BS0 with knots,x,y:" << endl;
  cout << "[ ";
  for (auto it = knots0.begin(); it != knots0.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = x0.begin(); it != x0.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y0.begin(); it != y0.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  BS0.show();
  cout << "B(0) = " << BS0(0) << endl;
  cout << "B(1) = " << BS0(1) << endl;
  cout << "B(1.5) = " << BS0(1.5) << endl;
  cout << "B(2) = " << BS0(2) << endl;
  fnplt(BS0,"Outputfile/Bformtest0.m");
  cout << "Run Outputfile/Bformtest0.m by matlab to get result." <<
  endl;
  std::vector<double> knots1 = vv5;
  std::vector<double> x1(3);
  std::vector<double> y1(3);
  x1[0] = 1.2;x1[1] = 3.6;x1[2] = 5;
  y1[0] = 0;y1[1] = 3;y1[2] = 0;
  Spline<1,4,BForm> BS4 = fitCurve<4>(knots1,x1,y1);
  cout << "BS4 with knots,x,y:" << endl;
  cout << "[ ";
  for (auto it = knots1.begin(); it != knots1.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = x1.begin(); it != x1.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y1.begin(); it != y1.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  BS4.show();
  cout << "B(0) = " << BS4(0) << endl;
  cout << "B(1.2) = " << BS4(1.2) << endl;
  cout << "B(3.6) = " << BS4(3.6) << endl;
  cout << "B(4) = " << BS4(4) << endl;
  cout << "B(5) = " << BS4(5) << endl;
  fnplt(BS4,"Outputfile/Bformtest1.m");
  cout << "Run Outputfile/Bformtest1.m by matlab to get result." <<
  endl;
  std::vector<double> knots2(9);
  knots2[0] = 0;knots2[1] = 0;knots2[2] = 0;
  knots2[3] = 1;knots2[4] = 2;knots2[5] = 3;
  knots2[6] = 4;knots2[7] = 4;knots2[8] = 4;
  std::vector<double> x2(5);
  std::vector<double> y2(5);
  x2[0] = 1;x2[1] = 2;x2[2] = 2;x2[3] = 2;x2[4] = 3;
  y2[0] = 0;y2[1] = 2;y2[2] = 0;y2[3] = -1;y2[4] = -1;
  Spline<1,4,BForm> BS5 = fitCurve<4>(knots2,x2,y2);
  cout << "BS5 with knots,x,y:" << endl;
  cout << "[ ";
  for (auto it = knots2.begin(); it != knots2.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = x2.begin(); it != x2.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y2.begin(); it != y2.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  BS5.show();
  cout << "B(1) = " << BS5(1) << endl;
  cout << "B(2) = " << BS5(2) << endl;
  cout << "B(3) = " << BS5(3) << endl;
  fnplt(BS5,"Outputfile/Bformtest2.m");
  cout << "Run Outputfile/Bformtest2.m by matlab to get result." <<
  endl;
  cout << "--------------Test for fitcurve without\
  knots:--------------" << endl;
  Spline<1,4,BForm> BS6 = fitCurve<4>(x2,y2);
  cout << "BS6 with x,y, Order 4:" << endl;
  cout << "[ ";
  for (auto it = x2.begin(); it != x2.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y2.begin(); it != y2.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  BS6.show();
  cout << "B(1) = " << BS6(1) << endl;
  cout << "B(2) = " << BS6(2) << endl;
  cout << "B(3) = " << BS6(3) << endl;
  fnplt(BS6,"Outputfile/Bformtest3.m");
  cout << "Run Outputfile/Bformtest3.m by matlab to get result." <<
  endl;
  std::vector<double> x3(8);
  std::vector<double> y3(8);
  x3[0] = 0;x3[1] = 1;x3[2] = 2;x3[3] = 3;
  x3[4] = 4;x3[5] = 5;x3[6] = 6;x3[7] = 7;
  y3[0] = 0;y3[1] = 2;y3[2] = 0;y3[3] = -1;
  y3[4] = -4;y3[5] = -2;y3[6] = 0;y3[7] = 4;
  Spline<1,3,BForm> BS7 = fitCurve<3>(x3,y3);
  cout << "BS7 with x,y, Order 3:" << endl;
  cout << "[ ";
  for (auto it = x3.begin(); it != x3.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y3.begin(); it != y3.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  BS7.show();
  cout << "B(1) = " << BS7(1) << endl;
  cout << "B(4) = " << BS7(4) << endl;
  cout << "B(7) = " << BS7(7) << endl;
  fnplt(BS7,"Outputfile/Bformtest4.m");
  cout << "Run Outputfile/Bformtest4.m by matlab to get result." <<
  endl;
  cout << "--------------Test for fnval:--------------" << endl;
  cout << "fnval(BS6,2) = " << fnval(BS6,2) << endl;
  cout << "fnval(BS6,2.5) = " << fnval(BS6,2.5) << endl;
  cout << "fnval(BS7,1) = " << fnval(BS7,1) << endl;
  cout << "fnval(BS7,7) = " << fnval(BS7,7) << endl;
  cout << "fnval(BS7,10.2) = " << fnval(BS7,10.2) << endl;
  cout << "--------------Test for fnder:--------------" << endl;
  cout << "BS6:" << endl;
  BS6.show();
  cout << "fnder(BS6):" << endl;
  fnder(BS6).show();
  cout << "--------------Test for calknots:--------------" << endl;
  cout << "knots of BS5:" << endl;
  std::vector<double> r1 = calknots(BS5);
  cout << "[ ";
  for (auto it = r1.begin(); it != r1.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  cout << "knots of BS6:" << endl;
  std::vector<double> r2 = calknots(BS4);
  cout << "[ ";
  for (auto it = r2.begin(); it != r2.end(); it++)
    cout << *it << " ";
  cout << "]"<< endl;
  cout << "--------------Test for fn2pp, Dim 1:--------------" <<
  endl;
  cout << "BS5:" << endl;
  BS5.show();
  Spline<1,4,ppForm> sp1 = fn2pp(BS5);
  cout << "fn2pp(BS5): " << endl;
  sp1.show();
  fnplt(sp1,"Outputfile/Btest2toppform.m");
}
