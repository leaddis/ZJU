#include "Spline.h"
#include "SplineHelper.h"

using std::cout;
using std::endl;

#define PI 3.1415926

int main(){
  cout << "-----------------Test for HighOrderfitCurve:-----------------" <<
  endl;
  Vec<double,2> vpd1({0,0.6});
  Vec<double,2> vpd10({0.2,1});
  Vec<double,2> vpd11({0.5,2});
  Vec<double,2> vpd2({1,2.4});
  Vec<double,2> vpd20({1.33,3});
  Vec<double,2> vpd21({2,0});
  Vec<double,2> vpd22({2.2,0});
  Vec<double,2> vpd23({2.5,-0.1});
  Vec<double,2> vpd3({3,-0.4});
  Vec<double,2> vpd31({3,-0.6});
  Vec<double,2> vpd32({2.4,-0.75});
  Vec<double,2> vpd4({2,-1});
  Vec<double,2> vpd41({0,-0.8});
  Vec<double,2> vpd42({-0.25,-0.6});
  Vec<double,2> vpd5({-1,0.2});
  Vec<double,2> vpd51({-0.8,0.4});
  Vec<double,2> vpd6({-0.5,0.4});
  Vec<double,2> vpd61({-0.2,0.5});
  Vec<double,2> vpd7({0,0.6});
  std::vector<Vec<double,2> > svv2{vpd1,vpd10,vpd11,vpd2,vpd20,vpd21,vpd22,vpd23,vpd3,vpd31,vpd32,vpd4,vpd41,vpd42,vpd5,vpd51,vpd6,vpd61,vpd7};
  Spline<2,2,ppForm> res2 = fitCurve<2,2>(svv2);
  cout << "res2 with fitCurve,Order 2" << endl;
  res2.show();
  fnplt(res2,"Outputfile/D2O2test1.m",500,0,1);
  cout << "run Outputfile/D2O2test1.m by matlab." << endl;
  Spline<2,4,ppForm> res3 = fitCurve<2,4>(svv2,periodic);
  cout << "res3 with fitCurve,Order 4" << endl;
  res3.show();
  fnplt(res3,"Outputfile/D2O4test1.m",500,0,1);
  cout << "run Outputfile/D2O4test1.m by matlab." << endl;
  Spline<2,4,ppForm> res4 = HighOrderfitCurve<4>(svv2);
  cout << "res4 with HighOrderfitCurve,Order 4" << endl;
  res4.show();
  fnplt(res4,"Outputfile/D2O4test2.m",1000,0,1);
  cout << "run Outputfile/D2O4test2.m by matlab." << endl;
  Spline<2,6,ppForm> res5 = HighOrderfitCurve<6>(svv2);
  cout << "res5 with HighOrderfitCurve,Order 6" << endl;
  res5.show();
  fnplt(res5,"Outputfile/D2O6test2.m",1000,0,1);
  cout << "run Outputfile/D2O6test2.m by matlab." << endl;
  Spline<2,8,ppForm> res6 = HighOrderfitCurve<8>(svv2);
  cout << "res6 with HighOrderfitCurve,Order 8" << endl;
  res6.show();
  fnplt(res5,"Outputfile/D2O8test2.m",1000,0,1);
  cout << "run Outputfile/D2O8test2.m by matlab." << endl;
  std::vector<Vec<double,2> > vv(101);
  for (int i = 0 ; i < 100 ; i++){
    vv[i] = Vec<double,2>{std::cos(i*2*PI/100),std::sin(i*2*PI/100)};
  }
  vv[100] = vv[0];
  Spline<2,2,ppForm> res7 = fitCurve<2,2>(vv);
  cout << "Circle test, res7 with fitCurve,Order 2" << endl;
  res7.show();
  fnplt(res7,"Outputfile/D2O2test3.m",500,0,1);
  cout << "run Outputfile/D2O2test3.m by matlab." << endl;
  Spline<2,4,ppForm> res8 = fitCurve<2,4>(vv,periodic);
  cout << "Circle test, res8 with fitCurve,Order 4" << endl;
  res8.show();
  fnplt(res8,"Outputfile/D2O4test3.m",500,0,1);
  cout << "run Outputfile/D2O4test3.m by matlab." << endl;
  Spline<2,4,ppForm> res9 = HighOrderfitCurve<4>(vv);
  cout << "Circle test, res9 with HighOrderfitCurve,Order 4" << endl;
  res9.show();
  fnplt(res9,"Outputfile/D2O4test4.m",1000,0,1);
  cout << "run Outputfile/D2O4test4.m by matlab." << endl;
  Spline<2,6,ppForm> res10 = HighOrderfitCurve<6>(vv);
  cout << "Circle test, res10 with HighOrderfitCurve,Order 6" << endl;
  res10.show();
  fnplt(res10,"Outputfile/D2O6test4.m",1000,0,1);
  cout << "run Outputfile/D2O6test4.m by matlab." << endl;
  Spline<2,8,ppForm> res11 = HighOrderfitCurve<8>(vv);
  cout << "Circle test, res11 with HighOrderfitCurve,Order 8" << endl;
  res11.show();
  fnplt(res11,"Outputfile/D2O8test4.m",1000,0,1);
  cout << "run Outputfile/D2O8test4.m by matlab." << endl;
}
