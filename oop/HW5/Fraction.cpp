#include "Fraction.h"

// Helper function to simplify the fraction
void Fraction::simplify() {
    int gcd = std::gcd(numerator, denominator);
    numerator /= gcd;
    denominator /= gcd;

    // Ensure the denominator is positive
    if (denominator < 0) {
        numerator = -numerator;
        denominator = -denominator;
    }
}

// Default constructor
Fraction::Fraction() : numerator(0), denominator(1) {}

// Constructor with two integers
Fraction::Fraction(int num, int den) : numerator(num), denominator(den) {
    if (den == 0) throw std::invalid_argument("Denominator cannot be zero");
    simplify();
}

// Copy constructor
Fraction::Fraction(const Fraction& other) : numerator(other.numerator), denominator(other.denominator) {}

// Arithmetic operators
Fraction& Fraction::operator=(const Fraction& other) {
    if (this != &other) { // 防止自赋值
        numerator = other.numerator;
        denominator = other.denominator;
    }
    return *this;
}

Fraction Fraction::operator+(const Fraction& other) const {
    return Fraction(numerator * other.denominator + other.numerator * denominator,
                    denominator * other.denominator);
}

Fraction Fraction::operator-(const Fraction& other) const {
    return Fraction(numerator * other.denominator - other.numerator * denominator,
                    denominator * other.denominator);
}

Fraction Fraction::operator*(const Fraction& other) const {
    return Fraction(numerator * other.numerator, denominator * other.denominator);
}

Fraction Fraction::operator/(const Fraction& other) const {
    if (other.numerator == 0) throw std::invalid_argument("Division by zero");
    return Fraction(numerator * other.denominator, denominator * other.numerator);
}

// Relational operators
bool Fraction::operator<(const Fraction& other) const {
    return numerator * other.denominator < other.numerator * denominator;
}

bool Fraction::operator<=(const Fraction& other) const {
    return numerator * other.denominator <= other.numerator * denominator;
}

bool Fraction::operator==(const Fraction& other) const {
    return numerator * other.denominator == other.numerator * denominator;
}

bool Fraction::operator!=(const Fraction& other) const {
    return !(*this == other);
}

bool Fraction::operator>=(const Fraction& other) const {
    return !(*this < other);
}

bool Fraction::operator>(const Fraction& other) const {
    return !(*this <= other);
}

// Typecast to double
Fraction::operator double() const {
    return static_cast<double>(numerator) / denominator;
}

// To string
std::string Fraction::toString() const {
    return std::to_string(numerator) + "/" + std::to_string(denominator);
}

// Stream operators
std::ostream& operator<<(std::ostream& os, const Fraction& frac) {
    os << frac.toString();
    return os;
}

std::istream& operator>>(std::istream& is, Fraction& frac) {
    int num, den;
    char slash;
    is >> num >> slash >> den;
    if (slash != '/' || den == 0) {
        is.setstate(std::ios::failbit);
    } else {
        frac = Fraction(num, den);
    }
    return is;
}

// Conversion from decimal string
Fraction Fraction::fromDecimal(const std::string& decimal) {
    size_t dotPos = decimal.find('.');
    if (dotPos == std::string::npos) {
        return Fraction(std::stoi(decimal), 1);
    }

    int intPart = std::stoi(decimal.substr(0, dotPos));
    std::string fracPart = decimal.substr(dotPos + 1);
    int denom = static_cast<int>(std::pow(10, fracPart.length()));
    int numer = intPart * denom + std::stoi(fracPart);

    return Fraction(numer, denom);
}
