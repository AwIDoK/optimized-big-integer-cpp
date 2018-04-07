#include "big_integer.h"
#include <string>
#include <algorithm>
#include <vector>

big_integer::big_integer() {
    vec.push_back(0);
}

big_integer::big_integer(int value) {
    vec.push_back((unsigned int)(value));
    if (value >= 0) {
        vec.push_back(0);
    } else {
        vec.push_back(0xFFFF);
    }
}

big_integer::big_integer(std::string const& value) {
    size_t start_pos = 0;
    if (value[0] == '-') {
        start_pos = 1;
    }
    uint64_t carry = 0;
    for (size_t i = start_pos; i < value.size(); i++) {
        carry = carry * 10 + value[i] - '0';
        vec.push_back(unsigned int(carry >> 32));
        carry &= 0xFFFF;
    }
    std::reverse(vec.begin(), vec.end());
    vec.push_back(0);
    compress();
    if (start_pos == 1) {

    }
    //TODO
}

big_integer::big_integer(big_integer const& big_int) {
    vec = big_int.vec;
    sign = big_int.sign;
}

big_integer& big_integer::operator=(big_integer const& big_int) {
    vec = big_int.vec;
    sign = big_int.sign;
    return *this;
}

big_integer big_integer::operator+() {
    return big_integer(*this);
}

big_integer big_integer::operator-() {
    return ~(*this) + 1;
}

big_integer big_integer::operator~() {
    auto result(*this);
    for (unsigned int &i : vec) {
        i = ~i;
    }
    compress();
    return result;
}

bool big_integer::operator==(big_integer const &second) {
    return vec == second.vec && sign == second.sign;
}

bool big_integer::operator!=(big_integer const &second) {
    return vec != second.vec || sign != second.sign;
}

big_integer big_integer::operator--(int) {
    return (*this) - 1;
}

big_integer big_integer::operator++(int) {
    return (*this) + 1;
}

big_integer &big_integer::operator--() {
    auto result = (*this) - 1;
    return result;
}

big_integer &big_integer::operator++() {
    auto result = (*this) + 1;
    return result;
}

std::string to_string(big_integer const &big_int) {
    std::string result;
    big_integer copy(big_int);
    if (big_int.sign) {
        result += "-";
        copy = -copy;
    }
    std::string reversed;
    while (copy > 0) {
        auto tmp = copy.divide_with_mod(1000000000);
        for (int i = 0; i < 9; i++) {
            reversed += char('0' + tmp.first % 10);
            tmp.first /= 10;
        }
    }
    while (reversed.size() > 1 && reversed.back() == 0) {
        reversed.pop_back();
    }
}

std::pair<int, int> big_integer::divide_with_mod(int second) {
    return std::pair<int, int>();
}

big_integer big_integer::operator+(big_integer const &second) {
    uint64_t carry = 0;
    big_integer result;
    for (size_t i = 0; i < std::max(vec.size(), second.vec.size()) || carry; i++) {
        if (result.vec.size() + 1 >= i) {
            result.vec.push_back(result.vec.back());
        }
        carry += (uint64_t)vec[i] + (uint64_t)second.vec[i];
        result.vec[i] = (unsigned int)carry;
        carry >>= 32;
    }
    compress();
}

big_integer &big_integer::operator&=(big_integer const &second) {
    auto result = (*this) & second;
    return result;
}

big_integer &big_integer::operator|=(big_integer const &second) {
    auto result = (*this) | second;
    return result;
}

big_integer &big_integer::operator>>=(int second) {
    auto result = (*this) >> second;
    return result;
}

big_integer &big_integer::operator<<=(int second) {
    auto result = (*this) << second;
    return result;
}

big_integer &big_integer::operator^=(big_integer const &second) {
    auto result = (*this) ^ second;
    return result;
}

big_integer &big_integer::operator+=(big_integer const &second) {
    auto result = (*this) + second;
    return result;
}

big_integer &big_integer::operator-=(big_integer const &second) {
    auto result = (*this) - second;
    return result;
}

big_integer &big_integer::operator*=(big_integer const &second) {
    auto result = (*this) * second;
    return result;
}

big_integer &big_integer::operator/=(big_integer const &second) {
    auto result = (*this) / second;
    return result;
}

big_integer &big_integer::operator%=(big_integer const &second) {
    auto result = (*this) % second;
    return result;
}

void big_integer::compress() {
    if (sign) {
        while (vec.back() == 0xFFFF) {
            vec.pop_back();
        }
        vec.push_back(0xFFFF);
    } else {
        while (vec.back() == 0) {
            vec.pop_back();
        }
        vec.push_back(0);
    }

}

big_integer big_integer::operator&(big_integer const &second) {
    big_integer result;
    for (size_t i = 0; i < std::min(vec.size(), second.vec.size()); i++) {
        result.vec.push_back(vec[i] & second.vec[i]);
    }
    result.compress();
    return result;
}

big_integer big_integer::operator|(big_integer const &second) {
    big_integer result;
    for (size_t i = 0; i < std::max(vec.size(), second.vec.size()); i++) {
        uint32_t tmp = 0;
        if (i < vec.size()) {
            tmp |= vec[i];
        }
        if (i < second.vec.size()) {
            tmp |= second.vec[i];
        }
        result.vec.push_back(tmp);
    }
    result.compress();
    return result;
}

bool big_integer::operator<(big_integer const &second) {
    return compare(second) < 0;
}

bool big_integer::operator<=(big_integer const &second) {
    return compare(second) <= 0;
}

bool big_integer::operator>(big_integer const &second) {
    return compare(second) > 0;
}

bool big_integer::operator>=(big_integer const &second) {
    return compare(second) >= 0;
}

int big_integer::compare(big_integer const &second) {
    if (sign && !second.sign) {
        return -1;
    }
    if (!sign && second.sign) {
        return 1;
    }
    if (vec.size() > second.vec.size()) {
        return 1;
    }
    if (vec.size() < second.vec.size()) {
        return -1;
    }
    for (size_t i = vec.size() - 1; i >= 0; i--) {
        if (vec[i] > second.vec[i]) {
            return 1;
        }
        if (vec[i] < second.vec[i]) {
            return -1;
        }
    }
    return 0;
}




