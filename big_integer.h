#ifndef BIG_INTEGER_LIBRARY_H
#define BIG_INTEGER_LIBRARY_H

#include <string>
#include <vector>

class big_integer {
    std::vector<uint32_t> vec;

    bool sign() const;

    friend std::string to_string(big_integer const &big_int);

    void compress();

    int compare(big_integer const &big_int) const;

    uint32_t get_digit(size_t pos) const;

    void set_digit(size_t pos, uint32_t value);

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

    big_integer operator+();

    big_integer operator-() const;

    big_integer operator~() const;

    friend big_integer operator+(big_integer first, big_integer const &second);

    friend big_integer operator-(big_integer first, big_integer const &second);

    friend big_integer operator*(big_integer first, big_integer const &second);

    friend big_integer operator/(big_integer first, big_integer const &second);

    friend big_integer operator%(big_integer first, big_integer const &second);

    std::pair<big_integer, uint32_t> divide_with_mod(int second);

    big_integer &operator+=(big_integer const &second);

    big_integer &operator-=(big_integer const &second);

    big_integer &operator*=(big_integer const &second);

    big_integer &operator/=(big_integer const &second);

    big_integer &operator%=(big_integer const &second);

    friend big_integer operator|(big_integer first, big_integer const &second);

    friend big_integer operator&(big_integer first, big_integer const &second);

    friend big_integer operator^(big_integer first, big_integer const &second);

    big_integer operator>>(int second);

    big_integer operator<<(int second);

    big_integer &operator&=(big_integer const &second);

    big_integer &operator|=(big_integer const &second);

    big_integer &operator^=(big_integer const &second);

    big_integer &operator>>=(int second);

    big_integer &operator<<=(int second);

    big_integer &operator++();

    big_integer &operator--();

    big_integer operator++(int);

    big_integer operator--(int);
};

#endif