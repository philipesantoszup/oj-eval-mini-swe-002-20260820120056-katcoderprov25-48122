#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

namespace sjtu {
class int2048 {
private:
    bool sign_;
    std::vector<unsigned int> digits_;
    
    static const unsigned long long BASE = 1000000000;
    static const int BASE_DIGITS = 9;
    
    void trim();
    int compare_abs(const int2048& other) const;
    void add_abs(const int2048& other, int2048& result) const;
    void sub_abs(const int2048& other, int2048& result) const;
    void mul_school(const int2048& other, int2048& result) const;
    void mul_karatsuba(const int2048& other, int2048& result) const;
    void div_abs(const int2048& divisor, int2048& quotient, int2048& remainder) const;
    bool is_zero() const;
    
public:
    int2048();
    int2048(long long);
    int2048(const std::string &);
    int2048(const int2048 &);

    void read(const std::string &);
    void print();

    int2048 &add(const int2048 &);
    friend int2048 add(int2048, const int2048 &);

    int2048 &minus(const int2048 &);
    friend int2048 minus(int2048, const int2048 &);

    int2048 operator+() const;
    int2048 operator-() const;

    int2048 &operator=(const int2048 &);

    int2048 &operator+=(const int2048 &);
    friend int2048 operator+(int2048, const int2048 &);

    int2048 &operator-=(const int2048 &);
    friend int2048 operator-(int2048, const int2048 &);

    int2048 &operator*=(const int2048 &);
    friend int2048 operator*(int2048, const int2048 &);

    int2048 &operator/=(const int2048 &);
    friend int2048 operator/(int2048, const int2048 &);

    int2048 &operator%=(const int2048 &);
    friend int2048 operator%(int2048, const int2048 &);

    friend std::istream &operator>>(std::istream &, int2048 &);
    friend std::ostream &operator<<(std::ostream &, const int2048 &);

    friend bool operator==(const int2048 &, const int2048 &);
    friend bool operator!=(const int2048 &, const int2048 &);
    friend bool operator<(const int2048 &, const int2048 &);
    friend bool operator>(const int2048 &, const int2048 &);
    friend bool operator<=(const int2048 &, const int2048 &);
    friend bool operator>=(const int2048 &, const int2048 &);
};
} // namespace sjtu

#endif
