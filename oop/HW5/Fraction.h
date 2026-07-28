#ifndef FRACTION_H
#define FRACTION_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <cmath>

class Fraction {
private:
    int numerator;
    int denominator;

    void simplify(); // Helper function to simplify the fraction

public:
    // Constructors
    Fraction();
    Fraction(int num, int den);
    Fraction(const Fraction& other);

    // Arithmetic operators
    Fraction& operator=(const Fraction& other);
    Fraction operator+(const Fraction& other) const;
    Fraction operator-(const Fraction& other) const;
    Fraction operator*(const Fraction& other) const;
    Fraction operator/(const Fraction& other) const;

    // Relational operators
    bool operator<(const Fraction& other) const;
    bool operator<=(const Fraction& other) const;
    bool operator==(const Fraction& other) const;
    bool operator!=(const Fraction& other) const;
    bool operator>=(const Fraction& other) const;
    bool operator>(const Fraction& other) const;

    // Typecast to double
    operator double() const;

    // To string
    std::string toString() const;

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const Fraction& frac);
    friend std::istream& operator>>(std::istream& is, Fraction& frac);

    // Conversion from decimal string
    static Fraction fromDecimal(const std::string& decimal);
};

#endif // FRACTION_H
