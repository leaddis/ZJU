#include "KnotsHelper.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char* argv[]){
  std::vector<double> v1(5);
  v1[0] = 1;v1[1] = 2;v1[2] = 3;v1[3] =3;v1[4] = 3;
  cout << "v1 = [ ";
  for (auto it = v1.begin(); it != v1.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> av1 = aveknt(v1,3);
  cout << "k = 3 , avarage of v1 = [ ";
  for (auto it = av1.begin(); it != av1.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> av2 = aveknt(v1,5);
  cout << "k = 5 , avarage of v1 = [ ";
  for (auto it = av2.begin(); it != av2.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> ev1 = augknt(v1,2);
  cout << "k = 2 , augmented knot sequence of v1 = [ ";
  for (auto it = ev1.begin(); it != ev1.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> ev2 = augknt(v1,4);
  cout << "k = 4 , augmented knot sequence of v1 = [ ";
  for (auto it = ev2.begin(); it != ev2.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> apv1 = aptknt(v1,4);
  cout << "k = 4 , acceptable knot sequence of v1 = [ ";
  for (auto it = apv1.begin(); it != apv1.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> v2(8);
  v2[0] = 1;v2[1] = 2;v2[2] = 2;v2[3] =3;
  v2[4] = 3;v2[5] = 5;v2[6] = 6;v2[7] =10;
  cout << "v2 = [ ";
  for (auto it = v2.begin(); it != v2.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> av3 = aveknt(v2,4);
  cout << "k = 4 , avarage of v2 = [ ";
  for (auto it = av3.begin(); it != av3.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> ev3 = augknt(v2,4);
  cout << "k = 4 , augmented knot sequence of v2 = [ ";
  for (auto it = ev3.begin(); it != ev3.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> apv2 = aptknt(v2,4);
  cout << "k = 4 , acceptable knot sequence of v2 = [ ";
  for (auto it = apv2.begin(); it != apv2.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> v3(5);
  v3[0] = 0;v3[1] = 1;v3[2] = 2;v3[3] =3;
  v3[4] = 4;
  cout << "v3 = [ ";
  for (auto it = v3.begin(); it != v3.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> apv3 = aptknt(v3,4);
  cout << "k = 4 , acceptable knot sequence of v3 = [ ";
  for (auto it = apv3.begin(); it != apv3.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> v4(5);
  v4[0] = 0;v4[1] = 1;v4[2] = 2.5;v4[3] =3;
  v4[4] = 6;
  cout << "v4 = [ ";
  for (auto it = v4.begin(); it != v4.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> apv4 = aptknt(v4,3);
  cout << "k = 3 , acceptable knot sequence of v4 = [ ";
  for (auto it = apv4.begin(); it != apv4.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> x4(10);
  x4[0] = 0;x4[1] = 2;x4[2] = 5;x4[3] = 8;x4[4] = 10;
  x4[5] = 13;x4[6] = 17;x4[7] = 20;x4[8] = 25;x4[9] = 28;
  cout << "x4 = [ ";
  for (auto it = x4.begin(); it != x4.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
  std::vector<double> apv5 = perknt(x4,4);
  cout << "k = 4 , periodic ver. acceptable knot sequence of x4 = [ ";
  for (auto it = apv5.begin(); it != apv5.end() ; it++)
    cout << *it << " ";
  cout << "]" << endl;
}
