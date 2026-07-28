#include "Interpolation.h"

using namespace std;

int main(){
    vector<double> n_values = {6,11,16,21};
    for (int n : n_values){
        Newton_Interpolation newton;
        for (int i = 1; i <= n; i++){
            newton.addPoint(cos((2*i-1)*M_PI/(2*n)), 1.0 / (1 + 25 * cos((2*i-1)*M_PI/(2*n)) * cos((2*i-1)*M_PI/(2*n))));
        }
        ofstream A("C_" + to_string(n) + ".txt");
        newton.displayPolynomialPoint(A, 500);
    }
    return 0;
}