#include "Bspline.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){
  std::vector<double> knt1(4);
  knt1[0] = 2.0;knt1[1] = 3.0;knt1[2] = 1.0;
  knt1[3] = 2.0;
  std::vector<double>::iterator it1 = knt1.begin();
  std::set<double> set1(it1,it1+4);
  std::vector<Polynomial<3,double> > polys1(2);
  Polynomial<3,double> p1({1,-2,1});
  Polynomial<3,double> p2({1,-6,9});
  polys1[0] = p1;polys1[1] = p2;
  Bspline<1,3> Bs1(set1,polys1);
  const std::set<double> set2 = Bs1.getknots();
  const std::vector<Polynomial<3,double> > polys2 = Bs1.getpolys();
  std::set<double>::iterator sit1;
  std::vector<Polynomial<3,double> >::const_iterator vpit1;
  cout << "knots of Bs1:" << endl;
  for (sit1 = set2.begin(); sit1 != set2.end(); sit1++)
    cout << *sit1 << endl;
  cout << "polys of Bs1:" << endl;
  for (vpit1 = polys2.cbegin(); vpit1 != polys2.cend(); vpit1++)
    cout << *vpit1 << endl;
  Bspline<1,3> Bs2(knt1);
  const std::set<double> set3 = Bs2.getknots();
  cout << "knots of Bs2:" << endl;
  for (auto sit = set3.cbegin(); sit != set3.cend(); sit++)
    cout << *sit << endl;
  Bspline<1,3> Bs3;
  cout << "Test for empty():" << endl;
  std::vector<Bspline<1,3> > vB2(3);
  cout << "expect for 1: " << vB2[2].empty() << endl;
  cout << "expect for 0: " << Bs2.empty() << endl;
  cout << "Test for generate Bspline by knots:" << endl;
  std::vector<double> vv1(4);
  vv1[0] = 0;vv1[1] = 1;vv1[2] = 2; vv1[3] = 3;
  cout << "For knots: [ ";
  for (auto it = vv1.begin(); it != vv1.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Bspline<1,3> Bs4(vv1);
  Bs4.show();
  cout << "B(-1) = " << Bs4(-1) << " , ";
  cout << "B(0) = " << Bs4(0) << " , ";
  cout << "B(0.2) = " << Bs4(0.2) << endl;
  cout << "B(0.5) = " << Bs4(0.5) << " , ";
  cout << "B(1) = " << Bs4(1) << " , ";
  cout << "B(1.6) = " << Bs4(1.6) << endl;
  cout << "B(2) = " << Bs4(2) << " , ";
  cout << "B(3) = " << Bs4(3) << " , ";
  cout << "B(3.2) = " << Bs4(3.2) << endl;
  bsplineplt<3>(Bs4,"Outputfile/bsplinetest1.m");
  cout << "run Outputfile/bsplinetest1.m by matlab to get result." << endl;
  std::vector<double> vv2(4);
  vv2[0] = 0;vv2[1] = 1;vv2[2] = 1; vv2[3] = 2;
  cout << "For knots: [ ";
  for (auto it = vv2.begin(); it != vv2.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Bspline<1,3> Bs5(vv2);
  Bs5.show();
  cout << "DB(-1) = " << Bs5.rder(-1,1) << " , ";
  cout << "DB(0) = " << Bs5.rder(0,1) << " , ";
  cout << "DB(0.5) = " << Bs5.rder(0.5,1) << endl;
  cout << "DB(0.99) = " << Bs5.rder(0.99,1) << " , ";
  cout << "DB(1) = " << Bs5.rder(1,1) << " , ";
  cout << "DB(2) = " << Bs5.rder(2,1) << endl;
  bsplineplt<3>(Bs5,"Outputfile/bsplinetest2.m");
  cout << "run Outputfile/bsplinetest2.m by matlab to get result." << endl;
  std::vector<double> vv3(5);
  vv3[0] = 0;vv3[1] = 1;vv3[2] = 1; vv3[3] = 2;vv3[4] = 2;
  cout << "For knots: [ ";
  for (auto it = vv3.begin(); it != vv3.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Bspline<1,4> Bs6(vv3);
  Bs6.show();
  cout << "DB(0) = " << Bs6.rder(0,1) << " , ";
  cout << "DB(0.5) = " << Bs6.rder(0.5,1) << " , ";
  cout << "DB(0.99) = " << Bs6.rder(0.99,1) << endl;
  cout << "DB(1) = " << Bs6.rder(1,1) << " , ";
  cout << "DB(1.5) = " << Bs6.rder(1.5,1) << " , ";
  cout << "DB(2) = " << Bs6.rder(2,1) << endl;
  cout << "D2B(0) = " << Bs6.rder(0,2) << " , ";
  cout << "D2B(0.5) = " << Bs6.rder(0.5,2) << " , ";
  cout << "D2B(0.99) = " << Bs6.rder(0.99,2) << endl;
  cout << "D2B(1) = " << Bs6.rder(1,2) << " , ";
  cout << "D2B(1.5) = " << Bs6.rder(1.5,2) << " , ";
  cout << "D2B(2) = " << Bs6.rder(2,2) << endl;
  bsplineplt<4>(Bs6,"Outputfile/bsplinetest3.m");
  cout << "run Outputfile/bsplinetest3.m by matlab to get result." << endl;
  std::vector<double> vv4(5);
  vv4[0] = 0;vv4[1] = 1;vv4[2] = 1; vv4[3] = 1;vv4[4] = 2;
  cout << "For knots: [ ";
  for (auto it = vv4.begin(); it != vv4.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Bspline<1,4> Bs7(vv4);
  Bs7.show();
  bsplineplt<4>(Bs7,"Outputfile/bsplinetest4.m");
  cout << "run Outputfile/bsplinetest4.m by matlab to get result." << endl;
  std::vector<double> vv5(8);
  vv5[0] = 0;vv5[1] = 0;vv5[2] = 1.2; vv5[3] = 0.4;vv5[4] = 1.2;
  vv5[5] = 2;vv5[7] = 2;vv5[6] = 2;
  cout << "For knots: [ ";
  for (auto it = vv5.begin(); it != vv5.end(); it++)
    cout << *it << " ";
  cout << "]" << endl;
  Bspline<1,7> Bs8(vv5);
  Bs8.show();
  cout << "B(-1) = " << Bs8(-1) << " , ";
  cout << "B(0) = " << Bs8(0) << " , ";
  cout << "B(0.2) = " << Bs8(0.2) << endl;
  cout << "B(0.4) = " << Bs8(0.4) << " , ";
  cout << "B(1) = " << Bs8(1) << " , ";
  cout << "B(1.2) = " << Bs8(1.2) << endl;
  cout << "B(1.6) = " << Bs8(1.6) << " , ";
  cout << "B(2) = " << Bs8(2) << " , ";
  cout << "B(3.2) = " << Bs8(3.2) << endl;
  cout << "DB(-1) = " << Bs8.rder(-1,1) << " , ";
  cout << "D2B(0) = " << Bs8.rder(0,2) << " , ";
  cout << "D3B(0.2) = " << Bs8.rder(0.2,3) << endl;
  cout << "DB(0.4) = " << Bs8.rder(0.4,1) << " , ";
  cout << "D0B(1) = " << Bs8.rder(1,0) << " , ";
  cout << "DB(1.2) = " << Bs8.rder(1.2,1) << endl;
  cout << "D7B(1.6) = " << Bs8.rder(1.6,7) << " , ";
  cout << "D2B(2) = " << Bs8.rder(2,2) << " , ";
  cout << "D3B(3.2) = " << Bs8.rder(3.2,3) << endl;
  bsplineplt<7>(Bs8,"Outputfile/bsplinetest5.m");
  cout << "run Outputfile/bsplinetest5.m by matlab to get result." <<
  endl;
  cout << "Test for erase:" << endl;
  cout << "After erasing:" << endl;
  for (auto it = Bs4.getpolys().cbegin(); it !=Bs4.getpolys().cend() ; it++ )
    cout << *it << endl;
  auto it = Bs4.getpolys().cbegin();
  it++;
  Bs4.erase(it);
  cout << "Before erasing:" << endl;
  for (auto it2 = Bs4.getpolys().cbegin(); it2 !=Bs4.getpolys().cend() ; it2++ )
    cout << *it2 << endl;
  cout << "*it : " << *it << endl;
  
}
