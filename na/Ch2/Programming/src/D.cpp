#include "Interpolation.h"

using namespace std;

int main(){
    Hermite_Interpolation hermite;
    hermite.addPoint(0, 0, 75);
    hermite.addPoint(3, 225, 77);
    hermite.addPoint(5, 383, 80);
    hermite.addPoint(8, 623, 74);
    hermite.addPoint(13, 993, 72);
    cout<<"the distance at "<<10<<" is "<<hermite.interpolate(10)<< '\n';
    ofstream hermiteFile("D.txt");
    hermite.displayPolynomial(hermiteFile);
    hermite.printPoints(hermiteFile);
    hermite.displayPolynomialPoint(hermiteFile, 500);

    return 0;
}