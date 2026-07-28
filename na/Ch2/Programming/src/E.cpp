#include "Interpolation.h"

using namespace std;

int main(){
    vector<double> x={0,6,10,13,17,20,28};
    vector<double> y1={6.67,17.3,42.7,37.3,30.1,29.3,28.7};
    vector<double> y2{6.67,16.1,18.9,15.0,10.6,9.44,8.89};

    Newton_Interpolation newton1;
    for(int i=0;i<x.size();i++){
        newton1.addPoint(x[i],y1[i]);
    }

    Newton_Interpolation newton2;
    for(int i=0;i<x.size();i++){
        newton2.addPoint(x[i],y2[i]);
    }

    ofstream newtonFile1("E_1.txt");
    newton1.displayPolynomial(newtonFile1);
    newton1.displayPolynomialPoint(newtonFile1, 500);

    ofstream newtonFile2("E_2.txt");
    newton2.displayPolynomial(newtonFile2);
    newton2.displayPolynomialPoint(newtonFile2, 500);

    return 0;
}   