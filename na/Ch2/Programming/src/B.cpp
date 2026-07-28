#include "Interpolation.h"

using namespace std;

int main(){
    vector<double> n_values = {2,4,6,8};
    for (int n : n_values){
        Newton_Interpolation newton;
        for (int i = 0; i <= n; ++i){
            newton.addPoint(-5 + 10.0 * i / n, 1.0 / (1 + (-5 + 10.0 * i / n) * (-5 + 10.0 * i / n)));
        }
        ofstream A("B_" + to_string(n) + ".txt");
        newton.displayPolynomialPoint(A, 500);
    }
    return 0;
}