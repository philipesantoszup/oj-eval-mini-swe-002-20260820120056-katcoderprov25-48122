#include "int2048.h"
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace sjtu {

void int2048::trim() {
    while (digits_.size() > 1 && digits_.back() == 0) {
        digits_.pop_back();
    }
    if (digits_.size() == 1 && digits_[0] == 0) {
        sign_ = true;
    }
}

int int2048::compare_abs(const int2048& other) const {
    if (digits_.size() != other.digits_.size()) {
        return digits_.size() < other.digits_.size() ? -1 : 1;
    }
    for (int i = (int)digits_.size() - 1; i >= 0; --i) {
        if (digits_[i] != other.digits_[i]) {
            return digits_[i] < other.digits_[i] ? -1 : 1;
        }
    }
    return 0;
}

void int2048::add_abs(const int2048& other, int2048& result) const {
    result.digits_.clear();
    
    size_t n = std::max(digits_.size(), other.digits_.size());
    unsigned long long carry = 0;
    for (size_t i = 0; i < n || carry; ++i) {
        unsigned long long sum = carry + (i < digits_.size() ? digits_[i] : 0) 
                                       + (i < other.digits_.size() ? other.digits_[i] : 0);
        result.digits_.push_back(sum % BASE);
        carry = sum / BASE;
    }
    if (result.digits_.empty()) result.digits_.push_back(0);
}

void int2048::sub_abs(const int2048& other, int2048& result) const {
    result.digits_.clear();
    
    long long borrow = 0;
    for (size_t i = 0; i < digits_.size(); ++i) {
        long long diff = (long long)digits_[i] - borrow - (i < other.digits_.size() ? other.digits_[i] : 0);
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits_.push_back((unsigned int)diff);
    }
    result.trim();
}

void int2048::mul_school(const int2048& other, int2048& result) const {
    result.digits_.assign(digits_.size() + other.digits_.size(), 0);
    
    for (size_t i = 0; i < digits_.size(); ++i) {
        unsigned long long carry = 0;
        for (size_t j = 0; j < other.digits_.size() || carry; ++j) {
            unsigned long long cur = result.digits_[i + j] + 
                                  carry + 
                                  (unsigned long long)digits_[i] * (j < other.digits_.size() ? other.digits_[j] : 0);
            result.digits_[i + j] = cur % BASE;
            carry = cur / BASE;
        }
    }
    result.trim();
}

void int2048::mul_karatsuba(const int2048& other, int2048& result) const {
    size_t n = std::max(digits_.size(), other.digits_.size());
    if (n <= 64) {
        mul_school(other, result);
        return;
    }
    
    size_t k = n / 2;
    
    int2048 a1, a0, b1, b0;
    a1.sign_ = a0.sign_ = b1.sign_ = b0.sign_ = true;
    
    if (digits_.size() > k) {
        a0.digits_.assign(digits_.begin(), digits_.begin() + k);
        a1.digits_.assign(digits_.begin() + k, digits_.end());
    } else {
        a0 = *this;
        a1.digits_.push_back(0);
    }
    
    if (other.digits_.size() > k) {
        b0.digits_.assign(other.digits_.begin(), other.digits_.begin() + k);
        b1.digits_.assign(other.digits_.begin() + k, other.digits_.end());
    } else {
        b0 = other;
        b1.digits_.push_back(0);
    }
    
    a1.trim(); a0.trim(); b1.trim(); b0.trim();
    
    int2048 z0, z1, z2, tmp1, tmp2;
    a0.mul_karatsuba(b0, z0);
    a1.mul_karatsuba(b1, z2);
    
    int2048 a_sum, b_sum;
    if (a1.compare_abs(a0) >= 0) { a_sum.sign_ = true; a1.sub_abs(a0, a_sum); }
    else { a_sum.sign_ = false; a0.sub_abs(a1, a_sum); }
    if (b1.compare_abs(b0) >= 0) { b_sum.sign_ = true; b1.sub_abs(b0, b_sum); }
    else { b_sum.sign_ = false; b0.sub_abs(b1, b_sum); }
    
    a_sum.mul_karatsuba(b_sum, z1);
    
    z1.sign_ = !z1.sign_;
    z1.add_abs(z0, tmp1);
    tmp1.sign_ = z1.sign_;
    if (tmp1.compare_abs(z2) >= 0) { z1 = tmp1; z1.minus(z2); }
    else { z1 = z2; z1.sign_ = !z1.sign_; z1.add_abs(tmp1, tmp2); tmp2.sign_ = z1.sign_; z1 = tmp2; }
    
    result.digits_.assign(std::max({z0.digits_.size() + 2*k, z1.digits_.size() + k, z2.digits_.size()}), 0);
    
    
    for (size_t i = 0; i < z0.digits_.size(); ++i) result.digits_[i] = z0.digits_[i];
    for (size_t i = 0; i < z1.digits_.size(); ++i) result.digits_[i + k] += z1.digits_[i];
    for (size_t i = 0; i < z2.digits_.size(); ++i) result.digits_[i + 2*k] += z2.digits_[i];
    
    unsigned long long carry = 0;
    for (size_t i = 0; i < result.digits_.size(); ++i) {
        unsigned long long sum = result.digits_[i] + carry;
        result.digits_[i] = sum % BASE;
        carry = sum / BASE;
    }
    while (carry) {
        result.digits_.push_back(carry % BASE);
        carry /= BASE;
    }
    result.trim();
}

void int2048::div_abs(const int2048& divisor, int2048& quotient, int2048& remainder) const {
    if (divisor.is_zero()) return;
    
    int cmp = compare_abs(divisor);
    if (cmp < 0) {
        quotient = int2048(0);
        remainder = *this;
        return;
    }
    if (cmp == 0) {
        quotient = int2048(1);
        remainder = int2048(0);
        return;
    }
    
    size_t n = digits_.size();
    size_t m = divisor.digits_.size();
    
    // Fast path: single-digit divisor
    if (m == 1) {
        unsigned int dv = divisor.digits_[0];
        unsigned long long rem = 0;
        quotient.digits_.clear();
        quotient.sign_ = true;
        quotient.digits_.reserve(n);
        
        for (int i = (int)n - 1; i >= 0; --i) {
            unsigned long long cur = rem * BASE + digits_[i];
            unsigned int q = cur / dv;
            rem = cur % dv;
            quotient.digits_.push_back(q);
        }
        std::reverse(quotient.digits_.begin(), quotient.digits_.end());
        quotient.trim();
        remainder = int2048(0);
        if (rem > 0) remainder.digits_[0] = (unsigned int)rem;
        remainder.trim();
        return;
    }
    
    // Convert to big-endian vectors for Knuth's Algorithm D
    // u = dividend digits (most significant first)
    // v = divisor digits (most significant first)
    std::vector<unsigned int> u, v;
    u.reserve(n + 1);
    v.reserve(m);
    
    for (int i = (int)n - 1; i >= 0; --i) u.push_back(digits_[i]);
    for (int i = (int)m - 1; i >= 0; --i) v.push_back(divisor.digits_[i]);
    
    // Ensure u has a leading zero
    if (u.empty() || u[0] != 0) {
        u.insert(u.begin(), 0);
    }
    
    n = u.size() - 1; // u has n+1 digits now
    
    // Normalize: multiply both by d so that v[0] >= BASE/2
    unsigned int d = 1;
    unsigned long long lead = v[0];
    while (lead < BASE / 2) {
        lead <<= 1;
        d <<= 1;
    }
    
    if (d > 1) {
        unsigned long long k = 0;
        for (int i = (int)u.size() - 1; i >= 0; --i) {
            unsigned long long cur = (unsigned long long)u[i] * d + k;
            u[i] = cur % BASE;
            k = cur / BASE;
        }
        k = 0;
        for (int i = (int)v.size() - 1; i >= 0; --i) {
            unsigned long long cur = (unsigned long long)v[i] * d + k;
            v[i] = cur % BASE;
            k = cur / BASE;
        }
    }
    
    // Quotient has n - m + 1 digits
    std::vector<unsigned int> q(n - m + 1, 0);
    
    // Algorithm D (Knuth)
    for (size_t j = 0; j <= n - m; ++j) {
        // D3: Estimate q_hat
        unsigned long long q_hat = ((unsigned long long)u[j] * BASE + u[j + 1]) / v[0];
        unsigned long long r_hat = ((unsigned long long)u[j] * BASE + u[j + 1]) % v[0];
        
        // D4: Refine q_hat (at most 2 iterations needed)
        while (true) {
            if (q_hat >= BASE) { q_hat = BASE - 1; break; }
            if (m <= 1) break;
            // Check if q_hat * v[1] > (r_hat * BASE + u[j+2])
            unsigned long long lhs = q_hat * v[1];
            unsigned long long rhs = r_hat * BASE + (j + 2 <= n ? u[j + 2] : 0);
            if (lhs <= rhs) break;
            // More precise check: compare q_hat * (v[0], v[1]) with (u[j], u[j+1], u[j+2])
            unsigned long long prod_low = q_hat * v[1];
            unsigned long long prod_carry = prod_low / BASE;
            unsigned long long prod_high = q_hat * v[0] + prod_carry;
            unsigned long long u_high = (unsigned long long)u[j] * BASE + u[j + 1];
            if (prod_high < u_high) break;
            if (prod_high == u_high && (prod_low % BASE) <= (j + 2 <= n ? u[j + 2] : 0)) break;
            --q_hat;
            r_hat += v[0];
            if (r_hat >= BASE) break;
        }
        
        // D5: Multiply and subtract: u[j..j+m] -= q_hat * v
        if (q_hat > 0) {
            long long borrow = 0;
            long long carry_mult = 0;
            for (int i = (int)m - 1; i >= 0; --i) {
                unsigned long long prod = (unsigned long long)q_hat * v[i] + carry_mult;
                carry_mult = prod / BASE;
                unsigned int prod_d = prod % BASE;
                
                // Subtract from u[j + 1 + i] (v[i] aligns with u[j + 1 + i])
                int idx = j + 1 + i;
                long long sub = (long long)u[idx] - (long long)prod_d - borrow;
                if (sub < 0) {
                    long long num_borrows = (-sub + BASE - 1) / BASE;
                    sub += num_borrows * BASE;
                    borrow = num_borrows;
                } else {
                    borrow = 0;
                }
                u[idx] = (unsigned int)sub;
            }
            // Subtract carry from u[j]
            long long sub = (long long)u[j] - carry_mult - borrow;
            if (sub < 0) {
                // Need to add back (q_hat was too large)
                --q_hat;
                unsigned long long carry_add = 0;
                for (int i = (int)m - 1; i >= 0; --i) {
                    unsigned long long sum = (unsigned long long)u[j + 1 + i] + v[i] + carry_add;
                    u[j + 1 + i] = sum % BASE;
                    carry_add = sum / BASE;
                }
                u[j] += carry_add;
            } else {
                u[j] = (unsigned int)sub;
            }
        }
        
        q[j] = (unsigned int)q_hat;
    }
    
    // Build quotient (little-endian)
    quotient.digits_.clear();
    quotient.sign_ = true;
    for (int i = (int)q.size() - 1; i >= 0; --i) {
        quotient.digits_.push_back(q[i]);
    }
    quotient.trim();
    
    // Build remainder from u[1..m] (after the last step, u[1..m] has the remainder)
    // Actually, the remainder is in u[n-m+1..n] after all steps
    remainder.digits_.clear();
    remainder.sign_ = true;
    for (size_t i = n - m + 1; i <= n; ++i) {
        remainder.digits_.push_back(u[i]);
    }
    // Reverse to little-endian
    std::reverse(remainder.digits_.begin(), remainder.digits_.end());
    
    // Un-normalize remainder: divide by d
    if (d > 1) {
        unsigned long long rem_norm = 0;
        for (int i = (int)remainder.digits_.size() - 1; i >= 0; --i) {
            unsigned long long cur = remainder.digits_[i] + rem_norm * BASE;
            remainder.digits_[i] = (unsigned int)(cur / d);
            rem_norm = cur % d;
        }
    }
    remainder.trim();
}

bool int2048::is_zero() const {
    return digits_.size() == 1 && digits_[0] == 0;
}

int2048::int2048() : sign_(true) { digits_.push_back(0); }

int2048::int2048(long long v) : sign_(v >= 0) {
    if (v < 0) v = -v;
    if (v == 0) {
        digits_.push_back(0);
    } else {
        while (v > 0) {
            digits_.push_back(v % BASE);
            v /= BASE;
        }
    }
}

int2048::int2048(const std::string &s) : sign_(true) {
    read(s);
}

int2048::int2048(const int2048 &other) : sign_(other.sign_), digits_(other.digits_) {}

void int2048::read(const std::string &s) {
    digits_.clear();
    sign_ = true;
    
    size_t start = 0;
    if (!s.empty() && s[0] == '-') {
        sign_ = false;
        start = 1;
    }
    
    while (start < s.size() && s[start] == '0') start++;
    
    if (start >= s.size()) {
        digits_.push_back(0);
        sign_ = true;
        return;
    }
    
    std::string num = s.substr(start);
    while ((int)num.size() > BASE_DIGITS) {
        digits_.push_back(std::stoul(num.substr(num.size() - BASE_DIGITS)));
        num = num.substr(0, num.size() - BASE_DIGITS);
    }
    digits_.push_back(std::stoul(num));
    
    if (digits_.empty()) {
        digits_.push_back(0);
        sign_ = true;
    }
}

void int2048::print() {
    if (!sign_ && !is_zero()) std::cout << '-';
    if (digits_.empty()) { std::cout << '0'; return; }
    std::cout << digits_.back();
    for (int i = (int)digits_.size() - 2; i >= 0; --i) {
        std::cout << std::setw(BASE_DIGITS) << std::setfill('0') << digits_[i];
    }
}

int2048 &int2048::add(const int2048 &other) {
    return *this = *this + other;
}

int2048 &int2048::minus(const int2048 &other) {
    return *this = *this - other;
}

int2048 int2048::operator+() const { return *this; }

int2048 int2048::operator-() const {
    int2048 result = *this;
    if (!is_zero()) result.sign_ = !result.sign_;
    return result;
}

int2048 &int2048::operator=(const int2048 &other) {
    sign_ = other.sign_;
    digits_ = other.digits_;
    return *this;
}

int2048 &int2048::operator+=(const int2048 &other) {
    return *this = *this + other;
}

int2048 &int2048::operator-=(const int2048 &other) {
    return *this = *this - other;
}

int2048 &int2048::operator*=(const int2048 &other) {
    return *this = *this * other;
}

int2048 &int2048::operator/=(const int2048 &other) {
    return *this = *this / other;
}

int2048 &int2048::operator%=(const int2048 &other) {
    return *this = *this % other;
}

int2048 add(int2048 a, const int2048 &b) {
    return a + b;
}

int2048 minus(int2048 a, const int2048 &b) {
    return a - b;
}

int2048 operator+(int2048 a, const int2048 &b) {
    int2048 result;
    if (a.sign_ == b.sign_) {
        result.sign_ = a.sign_;
        a.add_abs(b, result);
    } else {
        int cmp = a.compare_abs(b);
        if (cmp >= 0) {
            result.sign_ = a.sign_;
            a.sub_abs(b, result);
        } else {
            result.sign_ = b.sign_;
            b.sub_abs(a, result);
        }
    }
    result.trim();
    return result;
}

int2048 operator-(int2048 a, const int2048 &b) {
    int2048 nb = b;
    if (!nb.is_zero()) nb.sign_ = !nb.sign_;
    return a + nb;
}

int2048 operator*(int2048 a, const int2048 &b) {
    if (a.is_zero() || b.is_zero()) return int2048(0);
    int2048 result;
    a.mul_karatsuba(b, result);
    result.sign_ = (a.sign_ == b.sign_);
    return result;
}

int2048 operator/(int2048 a, const int2048 &b) {
    if (a.is_zero()) return int2048(0);
    
    int2048 q, r;
    a.div_abs(b, q, r);
    
    bool q_sign = (a.sign_ == b.sign_);
    
    if (!r.is_zero()) {
        if (!q_sign) {
            q.sign_ = false;
            int2048 one(1);
            q = q - one;
        } else {
            q.sign_ = true;
        }
    } else {
        q.sign_ = q_sign;
    }
    
    q.trim();
    return q;
}

int2048 operator%(int2048 a, const int2048 &b) {
    int2048 q = a / b;
    int2048 prod = q * b;
    return a - prod;
}

std::istream &operator>>(std::istream &is, int2048 &n) {
    std::string s;
    is >> s;
    n.read(s);
    return is;
}

std::ostream &operator<<(std::ostream &os, const int2048 &n) {
    if (!n.sign_ && !n.is_zero()) os << '-';
    if (n.digits_.empty()) { os << '0'; return os; }
    os << n.digits_.back();
    for (int i = (int)n.digits_.size() - 2; i >= 0; --i) {
        os << std::setw(int2048::BASE_DIGITS) << std::setfill('0') << n.digits_[i];
    }
    return os;
}

bool operator==(const int2048 &a, const int2048 &b) {
    if (a.sign_ != b.sign_) return false;
    return a.digits_ == b.digits_;
}

bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
}

bool operator<(const int2048 &a, const int2048 &b) {
    if (a.sign_ != b.sign_) return !a.sign_ && !a.is_zero();
    if (a.sign_) {
        return a.compare_abs(b) < 0;
    } else {
        return a.compare_abs(b) > 0;
    }
}

bool operator>(const int2048 &a, const int2048 &b) {
    return b < a;
}

bool operator<=(const int2048 &a, const int2048 &b) {
    return !(b < a);
}

bool operator>=(const int2048 &a, const int2048 &b) {
    return !(a < b);
}

} // namespace sjtu
