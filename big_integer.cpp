#include "big_integer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>

big_integer::big_integer() noexcept : big_integer(0) {}

big_integer::big_integer(int value) noexcept : number() {
    small[0] = static_cast<uint32_t>(value);
    small[1] = value >= 0 ? 0 : 0xFFFFFFFF;
    if (value == 0 || value == -1) {
        small_size = 1;
    } else {
        small_size = 2;
    }
}

big_integer::big_integer(std::string const &value) : big_integer() {
    if (value.empty()) {
        return;
    }
    if (value[0] != '-' && !isdigit(value[0])) {
        number = nullptr;
        throw std::runtime_error("Unknown symbol");
    }
    size_t start_pos = 0;
    bool sign;
    if (value[0] == '-') {
        start_pos = 1;
        sign = true;
    } else {
        sign = false;
    }
    size_t i = start_pos;
    uint32_t delta;
    while (i < value.size()) {
        delta = i + 9 <= value.size() ? 9 : 1;
        for (; i + delta <= value.size(); i += delta) {
            if (!isdigit(value[i])) {
                number = nullptr;
                throw std::runtime_error("Unknown symbol");
            }
            multiply_by_short(delta == 1 ? 10u : 1000000000u);
            add_or_sub(std::stoi(value.substr(i, delta)));
        }
    }
    push_back(0);
    normalize();
    if (sign) {
        negate();
    }
}

big_integer::big_integer(big_integer const &big_int) noexcept : number() {
    small_size = big_int.small_size;
    if (small_size == 3) {
        number = big_int.number;
    } else {
        for (size_t i = 0; i < small_size; i++) {
            small[i] = big_int.small[i];
        }
    }
}

big_integer &big_integer::operator=(big_integer const &big_int) noexcept {
    char buffer[sizeof(big_integer)];
    big_integer tmp(big_int);
    std::memcpy(buffer, &tmp, sizeof(big_integer));
    std::memcpy(&tmp, this, sizeof(big_integer));
    std::memcpy(this, buffer, sizeof(big_integer));
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    auto result = *this;
    result.make_unique();
    result.negate();
    return result;
}

big_integer big_integer::operator~() const {
    auto result = *this;
    result.make_unique();
    result.bitwise_not();
    return result;
}

big_integer const big_integer::operator--(int) {
    auto res = *this;
    make_unique();
    *this -= 1;
    return res;
}

big_integer const big_integer::operator++(int) {
    auto res = *this;
    make_unique();
    *this += 1;
    return res;
}

big_integer &big_integer::operator--() {
    make_unique();
    *this -= 1;
    return *this;
}

big_integer &big_integer::operator++() {
    make_unique();
    *this += 1;
    return *this;
}

std::string to_string(big_integer const &big_int) {
    big_integer copy;
    if (big_int.sign()) {
        copy = -big_int;
    } else {
        copy = big_int;
    }
    std::string result;
    do {
        auto tmp = copy.divide_by_short_with_remainder(1000000000);
        for (int i = 0; i < 9; i++) {
            result += char('0' + tmp % 10);
            tmp /= 10;
        }
    } while (copy.size() != 1 || copy.get_digit(0) != 0);
    while (result.size() > 1 && result.back() == '0') {
        result.pop_back();
    }
    if (big_int.sign()) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}

uint32_t big_integer::divide_by_short_with_remainder(uint32_t second) {
    uint64_t carry = 0;
    for (size_t i = size(); i-- > 0;) {
        carry = (carry << 32u) | get_digit(i);
        set_digit(i, static_cast<uint32_t>(carry / second));
        carry %= second;
    }
    normalize();
    return static_cast<uint32_t>(carry);
}

big_integer &big_integer::operator+=(big_integer const &second) {
    make_unique();
    add_or_sub(second, 0);
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &second) {
    make_unique();
    add_or_sub(second, 0, true);
    return *this;
}

template<class fun>
big_integer &big_integer::bitwiseOperation(fun func, big_integer const &second) {
    make_unique();
    size_t len = std::max(size(), second.size());
    for (size_t i = len; i-- > 0;) {
        set_digit_with_resize(i, func(get_digit_with_check(i), second.get_digit_with_check(i)));
    }
    pop_back();
    normalize();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &second) {
    return bitwiseOperation(std::bit_and<>(), second);
}

big_integer &big_integer::operator|=(big_integer const &second) {
    return bitwiseOperation(std::bit_or<>(), second);
}

big_integer &big_integer::operator^=(big_integer const &second) {
    return bitwiseOperation(std::bit_xor<>(), second);
}

big_integer &big_integer::operator>>=(int second) {
    make_unique();
    auto delta_full = static_cast<size_t>(second / 32);
    auto delta_local = static_cast<size_t>(second % 32);
    for (size_t i = 0; i < size(); i++) {
        uint32_t digit = 0;
        digit |= (get_digit_with_check(i + delta_full)) >> delta_local;
        if (delta_local > 0) {
            digit |= (get_digit_with_check(i + delta_full + 1) << (32 - delta_local));
        }
        set_digit(i, digit);
    }
    normalize();
    return *this;
}

big_integer &big_integer::operator<<=(int second) {
    make_unique();
    auto delta_full = static_cast<size_t>(second / 32);
    auto delta_local = static_cast<size_t>(second % 32);
    vector_resize(size() + delta_full);
    for (size_t i = size(); i-- > delta_full;) {
        uint32_t digit = 0;
        digit |= get_digit(i - delta_full) << delta_local;
        if (i > delta_full && delta_local > 0) {
            digit |= (get_digit(i - delta_full - 1) >> (32 - delta_local));
        }
        set_digit(i, digit);
    }
    for (size_t i = 0; i < delta_full; i++) {
        set_digit(i, 0);
    }
    normalize();
    return *this;
}


void big_integer::multiply_by_big(big_integer const &second) {
    std::vector<uint32_t> result(size() + second.size());
    uint64_t carry = 0;
    for (size_t i = 0; i < size(); i++) {
        carry = 0;
        for (size_t j = 0; j < second.size(); j++) {
            carry += result[i + j] + get_digit(i) * static_cast<uint64_t>(second.get_digit(j));
            result[i + j] = static_cast<uint32_t>(carry);
            carry >>= 32;
        }
        result[i + second.size()] = static_cast<uint32_t>(carry);
    }
    small_size = 3;
    number = std::make_shared<std::vector<uint32_t>>(result);
    normalize();
}

big_integer &big_integer::operator*=(big_integer const &second) {
    make_unique();
    bool result_sign = sign() != second.sign();
    big_integer a;
    if (sign()) {
        negate();
    }
    big_integer b;
    if (second.sign()) {
        b = -second;
    } else {
        b = second;
    }
    if (b.size() == 2) {
        multiply_by_short(b.get_digit(0));
    } else {
        multiply_by_big(b);
    }
    if (result_sign) {
        negate();
    }
    return *this;
}

void big_integer::divide_by_big(big_integer &dividend) {
    if (compare(dividend) == -1) {
        *this = 0;
        return;
    }
    auto norm = static_cast<uint32_t>((1ull << 32u) / (dividend.get_digit(dividend.size() - 2) + 1));
    multiply_by_short(norm);
    dividend.make_unique();
    dividend.multiply_by_short(norm);
    size_t N = dividend.size() - 1;
    size_t M = size() - N;
    std::vector<uint32_t> result;
    result.reserve(M + 2);
    result.resize(M + 1);
    for (size_t i = M + 1; i-- > 0;) {
        uint64_t q =
                ((static_cast<uint64_t>(get_digit_with_check(i + N)) << 32u) + get_digit_with_check(i + N - 1)) /
                dividend.get_digit(N - 1);
        uint64_t r =
                ((static_cast<uint64_t>(get_digit_with_check(i + N)) << 32u) + get_digit_with_check(i + N - 1)) %
                dividend.get_digit(N - 1);
        q = std::min(q, static_cast<uint64_t>(0xFFFFFFFF));
        while (q == (1ull << 32u) || (q * (dividend.get_digit(N - 2)) > (r << 32u) + get_digit_with_check(i + N - 2))) {
            q--;
            r += dividend.get_digit(N - 1);
            if (r >= (1ull << 32u)) {
                break;
            }
        }
        big_integer d(dividend);
        d.make_unique();
        d.multiply_by_short(static_cast<uint32_t> (q));
        add_or_sub(d, i, true);
        while (sign()) {
            add_or_sub(dividend, i);
            q--;
        }
        result[i] = static_cast<uint32_t>(q);
    }
    result.push_back(0);
    small_size = 3;
    number = std::make_shared<std::vector<uint32_t>>(result);
    normalize();
}

big_integer &big_integer::operator/=(big_integer const &second) {
    make_unique();
    if (second.size() == 1 && second.get_digit(0) == 0) {
        number = nullptr;
        throw std::runtime_error("Division by zero");
    }
    if (size() == 1 && get_digit(0) == 0) {
        return *this = 0;
    }
    bool res_sign = sign() ^second.sign();
    if (sign()) {
        negate();
    }
    big_integer dividend;
    if (second.sign()) {
        dividend = -second;
    } else {
        dividend = second;
    }
    if (dividend.size() == 2) {
        uint32_t value = dividend.get_digit(0);
        divide_by_short_with_remainder(value);
    } else {
        divide_by_big(dividend);
    }
    if (res_sign) {
        negate();
    }
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &second) {
    make_unique();
    return *this -= *this / second * second;
}

void big_integer::normalize() {
    auto last_digit = get_digit(size() - 1);
    while (size() > 0 && get_digit(size() - 1) == last_digit) {
        pop_back();
    }
    push_back(last_digit);
}

big_integer operator&(big_integer first, const big_integer &second) {
    first.make_unique();
    return first &= second;
}

big_integer operator|(big_integer first, const big_integer &second) {
    first.make_unique();
    return first |= second;
}

big_integer operator^(big_integer first, big_integer const &second) {
    first.make_unique();
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
    int return_value = sign() ? -1 : 1;
    if (sign() != second.sign()) {
        return return_value;
    }
    if (size() > second.size()) {
        return return_value;
    }
    if (size() < second.size()) {
        return -return_value;
    }
    for (size_t i = size(); i-- > 0;) {
        if (get_digit(i) > second.get_digit(i)) {
            return return_value;
        }
        if (get_digit(i) < second.get_digit(i)) {
            return -return_value;
        }
    }
    return 0;
}


big_integer operator+(big_integer first, const big_integer &second) {
    first.make_unique();
    return first += second;
}

big_integer operator-(big_integer first, const big_integer &second) {
    first.make_unique();
    return first -= second;
}

big_integer operator*(big_integer first, const big_integer &second) {
    first.make_unique();
    return first *= second;
}

big_integer operator/(big_integer first, const big_integer &second) {
    first.make_unique();
    return first /= second;
}

big_integer operator%(big_integer first, const big_integer &second) {
    first.make_unique();
    return first %= second;
}

bool big_integer::sign() const {
    return get_digit(size() - 1) == 0xFFFFFFFF;
}

uint32_t big_integer::get_digit_with_check(size_t pos) const {
    return pos < size() ? get_digit(pos) : sign() ? 0xFFFFFFFF : 0;
}

void big_integer::set_digit_with_resize(size_t pos, uint32_t value) {
    if (small_size != 3 && pos + 1 >= 2) {
        make_big();
    }
    if (small_size == 3) {
        if (pos + 1 >= number->size()) {
            number->resize(pos + 2, sign() ? 0xFFFFFFFF : 0);
        }
    } else {
        for (uint8_t i = small_size; i < pos + 1; i++) {
            small[i] = sign() ? 0xFFFFFFFF : 0;
        }
        small_size = static_cast<uint8_t>(pos + 2);
    }
    set_digit(pos, value);
}

big_integer big_integer::operator>>(int second) const {
    auto result = *this;
    result.make_unique();
    result >>= second;
    return result;
}

big_integer big_integer::operator<<(int second) const {
    auto result = *this;
    result.make_unique();
    result <<= second;
    return result;
}

void big_integer::negate() {
    bitwise_not();
    *this += 1;
}

void big_integer::bitwise_not() {
    if (small_size == 3) {
        for (auto &digit : *number) {
            digit = ~digit;
        }
    } else {
        for (uint8_t i = 0; i < small_size; i++) {
            small[i] = ~small[i];
        }
    }
}

void big_integer::multiply_by_short(uint32_t second) {
    uint64_t carry = 0;
    for (size_t i = 0; i < size(); i++) {
        carry += get_digit(i) * static_cast<uint64_t>(second);
        set_digit(i, static_cast<uint32_t>(carry));
        carry >>= 32u;
    }
    push_back(0);
    normalize();
}

void big_integer::vector_resize(size_t new_size) {
    if (size() < new_size) {
        if (small_size != 3 && new_size > 2) {
            make_big();
        }
        uint32_t value = sign() ? 0xFFFFFFFF : 0;
        if (small_size != 3) {
            for (uint8_t i = small_size; i < new_size; i++) {
                small[i] = value;
            }
            small_size = static_cast<uint8_t>(new_size);
        } else {
            number->resize(new_size, value);
        }
    }

}

void big_integer::add_or_sub(big_integer const &second, const size_t delta_second, const bool is_sub) {
    auto carry = static_cast<uint64_t>(is_sub);
    size_t len = std::max(size(), second.size() + delta_second);
    size_t min_len = std::min(size(), second.size() + delta_second);
    vector_resize(len);
    for (size_t i = delta_second; (carry || i < min_len) && i < len; i++) {
        carry += get_digit_with_check(i) + static_cast<uint64_t>(is_sub ? ~second.get_digit_with_check(i - delta_second)
                                                                        : second.get_digit_with_check(
                        i - delta_second));
        set_digit(i, static_cast<uint32_t>(carry));
        carry >>= 32u;
    }
    normalize();
}

void big_integer::make_unique() {
    if (small_size == 3 && number.use_count() > 1) {
        number = std::make_shared<std::vector<uint32_t>>(*number);
    }
}

big_integer::~big_integer() {
    number = nullptr;
}

void big_integer::push_back(uint32_t value) {
    if (small_size == 3) {
        number->push_back(value);
    } else if (small_size == 2) {
        make_big();
        number->push_back(value);
    } else {
        small[small_size] = value;
        small_size++;
    }

}

void big_integer::make_big() {
    std::vector<uint32_t> tmp;
    for (uint8_t i = 0; i < small_size; i++) {
        tmp.push_back(small[i]);
    }
    number = std::make_shared<std::vector<uint32_t>>(tmp);
    small_size = 3;
}

void big_integer::pop_back() {
    if (small_size == 3) {
        number->pop_back();
    } else {
        small_size--;
    }
}

size_t big_integer::size() const {
    if (small_size == 3) {
        return number->size();
    } else {
        return small_size;
    }
}

uint32_t big_integer::get_digit(size_t pos) const {
    if (small_size == 3) {
        return (*number)[pos];
    } else {
        return small[pos];
    }
}

void big_integer::set_digit(size_t pos, uint32_t value) {
    if (small_size == 3) {
        (*number)[pos] = value;
    } else {
        small[pos] = value;
    }
}



