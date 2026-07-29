#include "Vec.h"

int main(){
  Vec<double,2> v1(2);
  std::cout << "v1 = " << v1 << std::endl;
  Vec<double,2> v2 = {4.5,2.3,4.6};
  std::cout << "v2 = " << v2 << std::endl;
  Vec<double,2> v3 = {-2,3};
  std::cout << "v3 = " << v3 << std::endl;
  std::cout << "v4 = v1.*v3 = " << v1*v3 << std::endl;
}
