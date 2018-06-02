#ifndef BIG_INTEGER_LIBRARY_H
#define BIG_INTEGER_LIBRARY_H

#include <string>
#include <vector>

class big_integer {
    std::vector<uint32_t> number;

    bool sign() const;

    friend std::string to_string(big_integer const &big_int);

    void normalize();

    int compare(big_integer const &big_int) const;

    uint32_t get_digit(size_t pos) const;

    void set_digit(size_t pos, uint32_t value);

    template<class fun>
    big_integer &bitwiseOperation(fun func, big_integer const &second);

    uint32_t divide_by_short_with_remainder(uint32_t second);

    void divide_by_big(big_integer &dividend);

    void multiply_by_short(uint32_t second);

    void multiply_by_big(big_integer const &second);

    void bitwise_not();

    void negate();

public:
    big_integer();

    big_integer(int value);

    big_integer(std::string const &value);

    big_integer(big_integer const &big_int);

    big_integer &operator=(big_integer const &big_int);

    friend bool operator==(big_integer const &first, big_integer const &second);

    friend bool operator!=(big_integer const &first, big_integer const &second);

    friend bool operator<(big_integer const &first, big_integer const &second);

    friend bool operator<=(big_integer const &first, big_integer const &second);

    friend bool operator>(big_integer const &first, big_integer const &second);

    friend bool operator>=(big_integer const &first, big_integer const &second);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    friend big_integer operator+(big_integer first, big_integer const &second);

    friend big_integer operator-(big_integer first, big_integer const &second);

    friend big_integer operator*(big_integer first, big_integer const &second);

    friend big_integer operator/(big_integer first, big_integer const &second);

    friend big_integer operator%(big_integer first, big_integer const &second);

    big_integer &operator+=(big_integer const &second);

    big_integer &operator-=(big_integer const &second);

    big_integer &operator*=(big_integer const &second);

    big_integer &operator/=(big_integer const &second);

    big_integer &operator%=(big_integer const &second);

    friend big_integer operator|(big_integer first, big_integer const &second);

    friend big_integer operator&(big_integer first, big_integer const &second);

    friend big_integer operator^(big_integer first, big_integer const &second);

    big_integer operator>>(int second) const;

    big_integer operator<<(int second) const;

    big_integer &operator&=(big_integer const &second);

    big_integer &operator|=(big_integer const &second);

    big_integer &operator^=(big_integer const &second);

    big_integer &operator>>=(int second);

    big_integer &operator<<=(int second);

    big_integer &operator++();

    big_integer &operator--();

    big_integer const operator++(int);

    big_integer const operator--(int);
};

#endif