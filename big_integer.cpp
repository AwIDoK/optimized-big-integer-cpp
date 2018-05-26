#include "big_integer.h"
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>

big_integer::big_integer() {
    vec.push_back(0);
}

big_integer::big_integer(int value) {
    vec.push_back(static_cast<uint32_t>(value));
    vec.push_back(value >= 0 ? 0 : 0xFFFFFFFF);
    compress();
}

big_integer::big_integer(std::string const &value) : big_integer() {
    if (value.empty()) {
        return;
    }
    size_t start_pos = 0;
    bool sign;
    if (value[0] == '-') {
        start_pos = 1;
        sign = true;
    } else {
        sign = false;
    }
    big_integer TEN(10);
    for (size_t i = start_pos; i < value.size(); i++) {
        *this *= TEN;
        *this += big_integer(value[i] - '0');
    }
    vec.push_back(0);
    compress();
    if (sign) {
        *this = -(*this);
    }

}

big_integer::big_integer(big_integer const &big_int) = default;

big_integer &big_integer::operator=(big_integer const &big_int) = default;

big_integer big_integer::operator+() {
    return *this;
}

big_integer big_integer::operator-() const {
    auto result = ~(*this);
    result++;
    return result;
}

big_integer big_integer::operator~() const {
    big_integer result = (*this);
    for (auto &digit : result.vec) {
        digit = ~digit;
    }
    result.compress();
    return result;
}


big_integer big_integer::operator--(int) {
    auto res = *this;
    *this -= 1;
    return res;
}

big_integer big_integer::operator++(int) {
    auto res = *this;
    *this += 1;
    return res;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

std::string to_string(big_integer const &big_int) {
    big_integer copy(big_int);
    if (big_int.sign()) {
        copy = -copy;
    }
    std::string result;
    do {
        auto tmp = copy.divide_with_mod(1000000000);
        for (int i = 0; i < 9; i++) {
            result += char('0' + tmp.second % 10);
            tmp.second /= 10;
        }
        copy = tmp.first;
    } while (copy.vec.size() != 1 || copy.get_digit(0) != 0);
    while (result.size() > 1 && result.back() == '0') {
        result.pop_back();
    }
    if (big_int.sign()) {
        result += "-";
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::pair<big_integer, uint32_t> big_integer::divide_with_mod(int second) {
    uint64_t carry = 0;
    big_integer result;
    for (int64_t i = static_cast<int64_t>(vec.size()) - 1; i >= 0; i--) {
        carry = (carry << 32u) + get_digit(static_cast<size_t>(i));
        result.vec.push_back(static_cast<uint32_t>(carry / second));
        carry %= second;
    }
    std::reverse(result.vec.begin(), result.vec.end());
    result.vec.push_back(0);
    result.compress();
    return std::make_pair(result, static_cast<uint32_t>(carry));
}

big_integer &big_integer::operator+=(big_integer const &second) {
    uint64_t carry = 0;
    size_t len = std::max(vec.size(), second.vec.size()) + 1;
    for (size_t i = 0; i < len; i++) {
        carry += get_digit(i) + static_cast<uint64_t>(second.get_digit(i));
        set_digit(i, static_cast<uint32_t>(carry));
        carry >>= 32;
    }
    vec.resize(len);
    compress();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &second) {
    (*this) += -second;
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &second) {
    size_t len = std::max(vec.size(), second.vec.size());
    for (size_t i = 0; i < len; i++) {
        set_digit(i, get_digit(i) & second.get_digit(i));
    }
    vec.pop_back();
    compress();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &second) {
    size_t len = std::max(vec.size(), second.vec.size());
    for (size_t i = 0; i < len; i++) {
        set_digit(i, get_digit(i) | second.get_digit(i));
    }
    vec.pop_back();
    compress();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &second) {
    size_t len = std::max(vec.size(), second.vec.size());
    for (size_t i = 0; i < len; i++) {
        set_digit(i, get_digit(i) ^ second.get_digit(i));
    }
    vec.pop_back();
    compress();
    return *this;
}

big_integer &big_integer::operator>>=(int second) {
    auto first = static_cast<size_t>(second / 32);
    auto delta = static_cast<size_t>(second % 32);
    for (size_t i = 0; i < vec.size(); i++) {
        uint32_t digit = 0;
        digit |= (get_digit(i + first)) >> delta;
        digit |= (get_digit(i + first + 1) << (32 - delta));
        vec[i] = digit;
    }
    compress();
    return *this;
}

big_integer &big_integer::operator<<=(int second) {
    int first = (second / 32);
    int delta = (second % 32);
    for (size_t i = vec.size(); i-- > first;) {
        uint32_t digit = 0;
        digit |= (get_digit(i - first)) << delta;
        if (i > first) {
            digit |= (get_digit(i - first - 1) >> (32 - delta));
        }
        vec[i] = digit;
    }
    for (size_t i = 0; i < first; i++) {
        vec[i] = 0;
    }
    compress();
    return *this;
}


big_integer &big_integer::operator*=(big_integer const &second) {
    bool result_sign = sign() != second.sign();
    big_integer a;
    if (sign()) {
        a = -(*this);
    } else {
        a = *this;
    }
    big_integer b;
    if (second.sign()) {
        b = -second;
    } else {
        b = second;
    }
    std::vector<uint32_t> result(vec.size() + second.vec.size());
    uint64_t carry = 0;
    for (size_t i = 0; i < vec.size(); i++) {
        for (size_t j = 0; j < second.vec.size(); j++) {
            carry += result[i + j] + a.get_digit(i) * static_cast<uint64_t>(b.get_digit(j));
            result[i + j] = static_cast<uint32_t>(carry);
            carry >>= 32;
        }
    }
    vec = result;
    compress();
    if (result_sign) {
        *this = -(*this);
    }
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &second) {
    bool res_sign = sign() ^second.sign();
    big_integer left = 0, right;
    if (sign()) {
        right = -(*this);
    } else {
        right = *this;
    }
    auto need = right;
    while (left < right) {
        big_integer mid = (left + right + 1) >> 1;
        auto prod = mid * second;
        if (prod > need) {
            right = mid - 1;
        } else {
            left = mid;
        }
    }
    if (res_sign) {
        left = -left;
    }
    this->vec = left.vec;
    compress();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &second) {
    *this -= (*this) / second * second;
    return *this;
}

void big_integer::compress() {
    auto last = vec.back();
    while (!vec.empty() && vec.back() == last) {
        vec.pop_back();
    }
    vec.push_back(last);
}

big_integer operator&(big_integer first, const big_integer &second) {
    return first &= second;
}

big_integer operator|(big_integer first, const big_integer &second) {
    return first |= second;
}

big_integer operator^(big_integer first, big_integer const &second) {
    return first ^= second;
}

bool operator<(big_integer const &first, big_integer const &second) {
    return first.compare(second) < 0;
}

bool operator<=(big_integer const &first, big_integer const &second) {
    return first.compare(second) <= 0;
}

bool operator>(big_integer const &first, big_integer const &second) {
    return first.compare(second) > 0;
}

bool operator>=(big_integer const &first, big_integer const &second) {
    return first.compare(second) >= 0;
}

bool operator==(big_integer const &first, big_integer const &second) {
    return first.compare(second) == 0;
}

bool operator!=(big_integer const &first, big_integer const &second) {
    return first.compare(second) != 0;
}

int big_integer::compare(big_integer const &second) const {
    int ret = sign() ? -1 : 1;
    if (sign() != second.sign()) {
        return ret;
    }
    if (vec.size() > second.vec.size()) {
        return ret;
    }
    if (vec.size() < second.vec.size()) {
        return -ret;
    }
    for (size_t i = vec.size(); i-- > 0;) {
        if (vec[i] > second.vec[i]) {
            return ret;
        }
        if (vec[i] < second.vec[i]) {
            return -ret;
        }
    }
    return 0;
}


big_integer operator+(big_integer first, const big_integer &second) {
    return first += second;
}

big_integer operator-(big_integer first, const big_integer &second) {
    return first -= second;
}

big_integer operator*(big_integer first, const big_integer &second) {
    return first *= second;
}

big_integer operator/(big_integer first, const big_integer &second) {
    return first /= second;
}

big_integer operator%(big_integer first, const big_integer &second) {
    return first %= second;
}

bool big_integer::sign() const {
    return static_cast<bool>((vec.back() >> 31) & 1);
}

uint32_t big_integer::get_digit(size_t pos) const {
    if (pos < vec.size()) {
        return vec[pos];
    }
    return sign() ? 0xFFFFFFFF : 0;
}

void big_integer::set_digit(size_t pos, uint32_t value) {
    if (pos + 1 >= vec.size()) {
        vec.resize(pos + 2, sign() ? 0xFFFFFFFF : 0);
    }
    vec[pos] = value;
}


big_integer big_integer::operator>>(int second) {
    auto result = *this;
    result >>= second;
    return result;
}

big_integer big_integer::operator<<(int second) {
    auto result = *this;
    result <<= second;
    return result;
}




