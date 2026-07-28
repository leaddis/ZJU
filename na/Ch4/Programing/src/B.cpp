#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

int main() 
{
    int beta = 2, p = 3, L = -1, U = 1;
    // Calculate the UFL and OFL.
    double UFL = pow(beta, L);
    double OFL = pow(beta, U) * (beta - pow(beta, 1 - p));
    cout << "UFL(F) is : " << UFL << endl;
    cout << "OFL(F) is : " << OFL << endl;
    ofstream fout;
    fout.open("B_data.txt");
    double m, x;
    int card = 0, e = 0;
    for (e = L; e <= U; e++)
    {
        m = 1;
        for (int i = 0; i < pow(beta,p-1)-1; i++)
        {
            x = m * pow(beta,e);
            fout << x << " " << -x << " ";
            card += 2;

            m += pow(beta, 1-p);
        }
        x = pow(m,e);
        fout << x << " " << -x << " ";
        card += 2;
    }
    fout <<  endl;
    //fout << 0 << endl;
    //card += 1; 0？
    cout << "The cardinality of F is : " << card << endl;

    //sub
    m = 0;
    double t = pow(beta, L);
    for (int i = 0; i < pow(beta,p-1)-1; i++)
    {
        m += pow(beta, 1-p);
        x = m * t;
        fout << x << " " << -x << " ";
    }
    fout << endl;
    fout.close();
    cout << "Data has been written to B_data.csv" << endl;

    return 0;
}