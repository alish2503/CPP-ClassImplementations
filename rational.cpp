#include <iostream>
#include <stdexcept>
#include <numeric> 

class Rational {
public:
    Rational(int numerator = 0, int denominator = 1) 
        : numerator(numerator), denominator(denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Denominator cannot be zero.");
        }
        normalize();
    }

    Rational& operator+=(const Rational& other) {
        numerator = numerator * other.denominator + other.numerator * denominator;
        denominator *= other.denominator;
        normalize();
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        *this += Rational(-other.numerator, other.denominator);
        return *this;
    }

    Rational& operator*=(const Rational& other) {
        numerator *= other.numerator;
        denominator *= other.denominator;
        normalize();
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        if (other.numerator == 0) {
            throw std::invalid_argument("Division by zero.");
        }
        *this *= Rational(other.denominator, other.numerator);
        return *this;
    }

    Rational operator+(const Rational& other) const {
        Rational result = *this;
        result += other;
        return result;
    }

    Rational operator-(const Rational& other) const {
        return *this + Rational(-other.numerator, other.denominator);
    }

    Rational operator*(const Rational& other) const {
        Rational result = *this;
        result *= other;
        return result;
    }

    Rational operator/(const Rational& other) const {
        if (other.numerator == 0) {
            throw std::invalid_argument("Division by zero.");
        }
        return *this * Rational(other.denominator, other.numerator);
    }

    bool operator==(const Rational& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }

    bool operator!=(const Rational& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        os << r.numerator;
        if(r.denominator != 1) os << "/" << r.denominator;
        return os;
    }

private:
    int numerator;
    int denominator;

    void normalize() {
        int gcd = std::gcd(numerator, denominator);
        numerator /= gcd;
        denominator /= gcd;

        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
    }
};

int main() {
    Rational r1(2, 3);
    Rational r2(1, 3);

    Rational r3 = r1 + r2;
    std::cout << "r1 + r2 = " << r3 << std::endl;

    r1 *= r2;
    std::cout << "r1 *= r2: " << r1 << std::endl;

    Rational r4 = r1 / r2;
    std::cout << "r1 / r2 = " << r4 << std::endl;

    Rational r5 = r1 - Rational(1, 4);
    std::cout << "r1 - 1/4: " << r5 << std::endl;

    return 0;
}
