#ifndef BIG_INTEGER_LIBRARY_H
#define BIG_INTEGER_LIBRARY_H

#include <string>
#include <vector>

class big_integer {
    std::vector<unsigned int> vec;
    bool sign;
public:

    big_integer();
    big_integer(int value);
    explicit big_integer(std::string const& value);
    big_integer(big_integer const& big_int);

    big_integer& operator=(big_integer const& big_int);

    bool operator==(big_integer const& second);
    bool operator!=(big_integer const& second);
    bool operator<=(big_integer const& second);
    bool operator<(big_integer const& second);
    bool operator>=(big_integer const& second);
    bool operator>(big_integer const& second);

    friend std::string to_string(big_integer const& bigInteger);


    big_integer& operator+();
    big_integer& operator-();

    big_integer operator+(big_integer const& second);
    big_integer operator-(big_integer const& second);
    big_integer operator*(big_integer const& second);
    big_integer operator/(big_integer const& second);
    big_integer operator%(big_integer const& second);

    big_integer& operator+=(big_integer const& second);
    big_integer& operator-=(big_integer const& second);
    big_integer& operator*=(big_integer const& second);
    big_integer& operator/=(big_integer const& second);
    big_integer& operator%=(big_integer const& second);

    big_integer operator&(big_integer const& second);
    big_integer operator|(big_integer const& second);
    big_integer operator^(big_integer const& second);
    big_integer operator~(big_integer const& second);
    big_integer operator>>(int second);
    big_integer operator<<(int second);

    big_integer& operator&=(big_integer const& second);
    big_integer& operator|=(big_integer const& second);
    big_integer& operator^=(big_integer const& second);
    big_integer& operator>>=(int second);
    big_integer& operator<<=(int second);

    big_integer& operator++();
    big_integer& operator--();
    big_integer operator++(int);
    big_integer operator--(int);


};

#endif