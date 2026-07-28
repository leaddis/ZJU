#include <iostream>
#include "Fraction.h"

int main() {
    Fraction f1(1, 2);
    Fraction f2(3, 4);

    std::cout << "f1: " << f1 << ", f2: " << f2 << "\n";

    // Arithmetic
    std::cout << "f1 + f2: " << (f1 + f2) << "\n";
    std::cout << "f1 - f2: " << (f1 - f2) << "\n";
    std::cout << "f1 * f2: " << (f1 * f2) << "\n";
    std::cout << "f1 / f2: " << (f1 / f2) << "\n";

    // Relational
    std::cout << std::boolalpha;
    std::cout << "f1 < f2: " << (f1 < f2) << "\n";
    std::cout << "f1 == f2: " << (f1 == f2) << "\n";

    // Typecast
    std::cout << "f1 as double: " << static_cast<double>(f1) << "\n";

    // From decimal
    Fraction f3 = Fraction::fromDecimal("1.414");
    std::cout << "From decimal (1.414): " << f3 << "\n";

    return 0;
}
