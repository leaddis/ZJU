#include "Interpolation.h"

using namespace std;

/******************************************************************************************************
*************************************************BASE**************************************************
******************************************************************************************************/

// 添加插值点
void Interpolation::addPoint(double x, double y) {
    x_values.push_back(x);
    y_values.push_back(y);
}

// 删除插值点
void Interpolation::removePoint(int index) {
    if (index < 0 || index >= x_values.size()) {
        throw out_of_range("Index out of range");
    }
    x_values.erase(x_values.begin() + index);
    y_values.erase(y_values.begin() + index);
}

// 将插值点打印到文件
void Interpolation::printPoints(ofstream& file) const {
    file << "x values: \n";
    for (const auto& x : x_values)
        file << x << " ";
    file << "\ny values: \n";
    for (const auto& y : y_values)
        file << y << " ";
    file << "\n";
    cout << "Points written to file successfully." << endl;
}

// 将插值点打印到输出流
void Interpolation::printPoints(ostream& os) const {
    os << "x values: \n";
    for (const auto& x : x_values)
        os << x << " ";
    os << "\ny values: \n";
    for (const auto& y : y_values)
        os << y << " ";
    os << "\n";
}

// 检查 x 是否在插值范围内
void Interpolation::isInrange(double x) const {
    if (x_values.empty()) {
        throw std::out_of_range("No points available");
    }
    // 使用 std::min_element 和 std::max_element 来找到最小值和最大值
    auto minIt = std::min_element(x_values.begin(), x_values.end());
    auto maxIt = std::max_element(x_values.begin(), x_values.end());
    double minX = *minIt;
    double maxX = *maxIt;

    if (x < minX || x > maxX) {
        std::cerr << "Error: x for interpolating value out of range." << std::endl;
        exit(1);
    }
}

/*********************************************************************************************************
*************************************************NEWTON***************************************************
*********************************************************************************************************/

// 计算差商表
void Newton_Interpolation::calculate_diff_table() {
    int n = x_values.size();
    coeff.resize(n, vector<double>(n, 0));

    for (int i = 0; i < n; ++i) {
        coeff[i][0] = y_values[i];
    }

    for (int j = 1; j < n; ++j) {
        for (int i = 0; i < n - j; ++i) {
            coeff[i][j] = (coeff[i + 1][j - 1] - coeff[i][j - 1]) / (x_values[i + j] - x_values[i]);
        }
    }
}

// 插值计算
double Newton_Interpolation::interpolate(double x) {
    if (x_values.empty() || y_values.empty()) {
        cerr << "Error: No data points available for interpolation." << endl;
        return NAN;
    }

    //isInrange(x);

    if (coeff.empty()) {
        calculate_diff_table();
    }
    double result = coeff[0][0];  // 插值结果
    double term = 1.0;            // 用于计算插值项的乘积

    for (int i = 1; i < x_values.size(); ++i) {
        term *= (x - x_values[i - 1]);
        result += coeff[0][i] * term;
    }

    return result;
}

// 打印插值多项式系数到文件中
void Newton_Interpolation::displayPolynomial(ofstream& file) {
    if (coeff.empty()) {
        calculate_diff_table();
    }
    int n = x_values.size();
    file << "Newton Interpolation Polynomial Coefficients: " << endl;
    file << "Polynomial degree: " << n - 1 << endl;
    file << fixed << setprecision(8);
    for (int i = 0; i < n; ++i) {
        file << setw(10) << coeff[0][i] << " ";
    }
    file << endl;
    cout << "Polynomials written to file successfully." << endl;
}

// 输出n个插值结果的坐标
void Newton_Interpolation::displayPolynomialPoint(ofstream& file, int n) {
    if (x_values.empty() || y_values.empty()) {
        cerr << "Error: No data points available for interpolation." << endl;
        return;
    }
    file << "Interpolation Points: " << endl;
    file << fixed << setprecision(8);
    for (int i = 0; i < n; ++i) {
        double x = x_values[0] + (x_values.back() - x_values[0]) * i / (n - 1);
        double y = interpolate(x);
        file << setw(10) << x << " " << y << endl;
    }
    cout << "Interpolation points written to file successfully." << endl;
}

/*******************************************************************************************************
*************************************************HERMITE************************************************
*******************************************************************************************************/

// Hermite 插值：添加点，包括 x, y 和 y'
void Hermite_Interpolation::addPoint(double x, double y, double y_prime) {
    x_values.push_back(x);
    y_values.push_back(y);
    y_prime_values.push_back(y_prime);
}

// 删除 Hermite 插值点
void Hermite_Interpolation::removePoint(int index) {
    if (index < x_values.size()) {
        x_values.erase(x_values.begin() + index);
        y_values.erase(y_values.begin() + index);
        y_prime_values.erase(y_prime_values.begin() + index);
    }
}

// 将 Hermite 插值点打印到输出流
void Hermite_Interpolation::printPoints(ostream& os) const {
    os << "x values: \n";
    for (const auto& x : x_values)
        os << x << " ";
    os << "\ny values: \n";
    for (const auto& y : y_values)
        os << y << " ";
    os << "\nDerivative values: \n";
    for (const auto& y_prime : y_prime_values)
        os << y_prime << " ";
    os << "\nx for calculating: \n";
    for (const auto& x : x_cal)
        os << x << " ";
    os << "\n";
}

// 将 Hermite 插值点打印到文件
void Hermite_Interpolation::printPoints(ofstream& file) const {
    file << "x values: \n";
    for (const auto& x : x_values)
        file << x << " ";
    file << "\ny values: \n";
    for (const auto& y : y_values)
        file << y << " ";
    file << "\nDerivative values: \n";
    for (const auto& y_prime : y_prime_values)
        file << y_prime << " ";
    file << "\nx for calculating: \n";
    for (const auto& x : x_cal)
        file << x << " ";
    file << "\n";
    cout << "Points written to file successfully." << endl;
}

// 计算 Hermite 插值的差商表
void Hermite_Interpolation::calculate_diff_table() {
    int n = x_values.size() * 2;
    coeff.resize(n, vector<double>(n, 0.0));
    x_cal.clear();
    x_cal.reserve(n);
    for (int i = 0; i < x_values.size(); i++) {
        x_cal.push_back(x_values[i]);
        x_cal.push_back(x_values[i]);
    }
    for (int i = 0; i < y_values.size(); i++) {
        coeff[2 * i][0] = y_values[i];
        coeff[2 * i + 1][0] = y_values[i];
        coeff[2 * i][1] = y_prime_values[i];
    }

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (x_cal[j] != x_cal[j + i])
                coeff[j][i] = (coeff[j + 1][i - 1] - coeff[j][i - 1]) / (x_cal[j + i] - x_cal[j]);
        }
    }
}

// Hermite 插值计算
double Hermite_Interpolation::interpolate(double x) {
    if (x_values.empty() || y_values.empty()) {
        cerr << "Error: No data points available for interpolation." << endl;
        return NAN;
    }
    //isInrange(x);
    if (coeff.empty()) {
        calculate_diff_table();
    }
    double result = coeff[0][0];
    double term = 1.0;

    for (int i = 1; i < x_cal.size(); i++) {
        term *= (x - x_cal[i - 1]);
        result += coeff[0][i] * term;
    }
    return result;
}

// 打印 Hermite 插值多项式的系数
void Hermite_Interpolation::displayPolynomial(ofstream& file) {
    if (coeff.empty()) {
        calculate_diff_table();
    }
    int n = x_cal.size();
    file << "Hermite Interpolation Polynomial Coefficients: " << endl;
    file << "Polynomial degree: " << n - 1 << endl;
    file << fixed << setprecision(8);
    for (int i = 0; i < 2 * n; ++i) {
        file << setw(10) << coeff[0][i] << " ";
    }
    file << endl;
    cout << "Polynomials written to file successfully." << endl;
}

// 输出n个插值结果的坐标
void Hermite_Interpolation::displayPolynomialPoint(ofstream& file, int n) {
    if (x_values.empty() || y_values.empty()) {
        cerr << "Error: No data points available for interpolation." << endl;
        return;
    }
    file << "Interpolation Points: " << endl;
    file << fixed << setprecision(8);
    for (int i = 0; i < n; ++i) {
        double x = x_values[0] + (x_values.back() - x_values[0]) * i / (n - 1);
        double y = interpolate(x);
        file << setw(10) << x << " " << y << endl;
    }
    cout << "Interpolation points written to file successfully." << endl;
}