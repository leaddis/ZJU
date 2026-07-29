#include "Polynomial.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){
  Polynomial<3,double> p1;
  Polynomial<2,Vec<double,2> > p2;
  Polynomial<2,Vec<double,2> > p3(Vec<double,2>(1));
  Vec<double,2> v1[3] = {{1,2},{2.3,-1.2},{-1,0}};
  std::vector<Vec<double,2> > sv(3);
  for (int i = 0 ; i < 3 ; i++)
    sv[i] = v1[i];
  Polynomial<3,Vec<double,2> > p4(sv);
  Polynomial<3,Vec<double,2> > p5({v1[0],v1[1],v1[2]});
  cout << "coef of p5: " << p5[0] << " , " << p5[1] << " , " << p5[2]
  << endl;
  cout << "set p5[0] = {2.3,-2.3}:" << endl;
  p5[0] = {2.3,-2.3};
  cout << "coef of p5: " << p5[0] << " , " << p5[1] << " , " << p5[2]
  << endl;
  cout << "p1 = " << p1 << endl;
  cout << "p2 = " << p2 << endl;
  cout << "p3 = " << p3 << endl;
  cout << "p4 = " << p4 << endl;
  Vec<double,2> v2[4] = {{-1.5,2},{2,3.3},{0,2},{3,0}};
  Polynomial<4,Vec<double,2> > p6({v2[0],v2[1],v2[2],v2[3]});
  cout << "p6 = " << p6 << endl;
  cout << "p4+p6 = " << p4+p6 << endl;
  cout << "p4-p6 = " << p4-p6 << endl;
  cout << "p4*p6 = " << p4*p6 << endl;
  cout << "p4*2.5 = " << p4*2.5 << endl;
  cout << "p3/2.0 = " << p3/2.0 << endl;
  cout << "(p6/2)-(p4*3)*p3 = " << (p6/2)-(p4*3)*p3 << endl;
  cout << "value at 1 of p4 = " << p4(1) << endl;
  cout << "value at 2.5 of p6 = " << p6(2.5) << endl;
  cout << "value at 0 of p4*p6 = " << (p4*p6)(0) << endl;
  cout << "integral of p4 = " << p4.integral() << endl;
  cout << "integral of p3 = " << p3.integral() << endl;
  cout << "derivation of p4 = " << p4.der() << endl;
  Polynomial<1,Vec<double,2> > p7({Vec<double,2>({1.2,2})});
  cout << "p7 = " << p7 << endl;
  cout << "derivation of p7 = " << p7.der() << endl;
  cout << "p6 = " << p6 << endl;
  cout << "p6.der().integral() = " << p6.der().integral() << endl;
  cout << "p6.integral().der() = " << p6.integral().der() << endl;
  cout << "adjust p6 with order 5 = " << adjust<5,4>(p6) << endl;
  cout << "adjust p6 with order 2 = " << adjust<2,4>(p6) << endl;
  cout << "Test for translate:" << endl;
  Polynomial<3,Vec<double,1> >
  p8({Vec<double,1>({1}),Vec<double,1>({-2}),Vec<double,1>({1})});
  cout << "p8 = " << p8 << endl;
  cout << "p8.translate(1) = " << p8.translate(1) << endl;
  cout << "p8.translate(2) = " << p8.translate(2) << endl;
  cout << "p6 = " << p6 << endl;
  cout << "p6.translate({1,-1}) = " <<
    p6.translate(Vec<double,2>({1,-1})) << endl;
}
