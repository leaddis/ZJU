#include "Spline.h"

using std::cout;
using std::endl;
template<int Order> using Bvec = std::vector<Bspline<1,Order> >;

int main(int argc, char* argv[]){
  cout << "--------------Test for basic function:--------------" << endl;
  Spline<1,3,cardinalB> CB1;
  std::vector<double> vv3(5);
  vv3[0] = 0;vv3[1] = 1;vv3[2] = 2; vv3[3] = 3;vv3[4] = 4;
  Bspline<1,4> B1(vv3);
  cout << "Bspline B1:" << endl;
  B1.show();
  std::vector<double> vv4(5);
  vv4[0] = 1;vv4[1] = 2;vv4[2] = 3; vv4[3] = 4;vv4[4] = 5;
  Bspline<1,4> B2(vv4);
  cout << "Bspline B2:" << endl;
  B2.show();
  Bvec<4> Bv1;
  Bv1.push_back(B1);
  Bv1.push_back(B2);
  std::vector<double> coe1;
  coe1.push_back(1.6);
  coe1.push_back(-2.5);
  Spline<1,4,cardinalB> CB2(Bv1,coe1);
  const Bvec<4>& Bv2 = CB2.getbasis();
  const std::vector<double>& coe2 = CB2.getcoes();
  cout << "First Bspline of CB2(B1):" << endl;
  Bv2[0].show();
  cout << "Coes of CB2:" << endl;
  cout << coe2[0] << "," << coe2[1] << endl;
  cout << "--------------Test for fitcurve:--------------" << endl;
  std::vector<int> vv6(9);
  vv6[0] = 0;vv6[1] = 1;vv6[2] = 2; vv6[3] = 3;vv6[4] = 4;
  vv6[5] = 5;vv6[6] = 6;vv6[7] = 7; vv6[8] = 8;
  std::vector<int> knots0 = vv6;
  std::vector<double> x0(5);
  std::vector<double> y0(5);
  x0[0] = 1;x0[1] = 2;x0[2] = 3.5;x0[3] = 5;x0[4] = 7;
  y0[0] = 2;y0[1] = 0;y0[2] = 1;y0[3] = 2;y0[4] = -1;
  Spline<1,4,cardinalB> CB0 = fitCurve<4>(knots0,x0,y0);
  cout << "CB0 with knots,x,y:" << endl;
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
  CB0.show();
  cout << "B(1) = " << CB0(1) << endl;
  cout << "B(2) = " << CB0(2) << endl;
  cout << "B(3.5) = " << CB0(3.5) << endl;
  cout << "B(5) = " << CB0(5) << endl;
  cout << "B(7) = " << CB0(7) << endl;
  fnplt<4>(CB0,"Outputfile/cardinalBtest1.m");
  cout << "Run Outputfile/cardinalBtest1.m by matlab to get result." <<
  endl;
  std::vector<int> vv5(7);
  vv5[0] = -3;vv5[1] = -2;vv5[2] = -1; vv5[3] = 0;vv5[4] = 1;
  vv5[5] = 2;vv5[6] = 3;
  std::vector<int> knots1 = vv5;
  std::vector<double> x1(4);
  std::vector<double> y1(4);
  x1[0] = -1.2;x1[1] = 0;x1[2] = 1;x1[3] = 1.8;
  y1[0] = 0;y1[1] = 3;y1[2] = 0;y1[3] = -2;
  Spline<1,3,cardinalB> CB4 = fitCurve<3>(knots1,x1,y1);
  cout << "CB4 with knots,x,y:" << endl;
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
  CB4.show();
  cout << "B(-1.2) = " << CB4(-1.2) << endl;
  cout << "B(0) = " << CB4(0) << endl;
  cout << "B(1) = " << CB4(1) << endl;
  cout << "B(1.8) = " << CB4(1.8) << endl;
  fnplt<3>(CB4,"Outputfile/cardinalBtest2.m");
  cout << "Run Outputfile/cardinalBtest2.m by matlab to get result." <<
  endl;
  std::vector<double> x2(6);
  std::vector<double> y2(6);
  x2[0] = 1;x2[1] = 1;x2[2] = 2;x2[3] = 3;x2[4] = 4;x2[5] = 4;
  y2[0] = 2;y2[1] = 0;y2[2] = 1;y2[3] = -1;y2[4] = -1;y2[5] = 1;
  Spline<1,4,cardinalB> CB5 = fitCurve<4>(-2,7,x2,y2);
  cout << "CB5 with knots from -2 to 7 and x,y:" << endl;
  cout << "[ ";
  for (auto it = x2.begin(); it != x2.end(); it++)
    cout << *it << " ";
  cout << "] , [ ";
  for (auto it = y2.begin(); it != y2.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  CB5.show();
  cout << "B(1) = " << CB5(1) << endl;
  cout << "B(2) = " << CB5(2) << endl;
  cout << "B(3) = " << CB5(3) << endl;
  cout << "B(4) = " << CB5(4) << endl;
  fnplt<4>(CB5,"Outputfile/cardinalBtest3.m");
  cout << "Run Outputfile/cardinalBtest3.m by matlab to get result." <<
  endl;
}
