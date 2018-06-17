#ifndef BIG_INTEGER_LIBRARY_H
#define BIG_INTEGER_LIBRARY_H

#include <string>
#include <vector>
#include <memory>

class big_integer {
    union {
        std::shared_ptr<std::vector<uint32_t>> number;
        uint32_t small[2];
    };
    uint8_t small_size;

    bool sign() const;

    friend std::string to_string(big_integer const &big_int);

    void normalize();

    int compare(big_integer const &big_int) const;

    uint32_t get_digit_with_check(size_t pos) const;

    uint32_t get_digit(size_t pos) const;

    void set_digit_with_resize(size_t pos, uint32_t value);

    template<class fun>
    big_integer &bitwiseOperation(fun func, big_integer const &second);

    uint32_t divide_by_short_with_remainder(uint32_t second);

    void divide_by_big(big_integer &dividend);

    void multiply_by_short(uint32_t second);

    void multiply_by_big(big_integer const &second);

    void bitwise_not();

    void negate();

    void vector_resize(size_t size);

    void add_or_sub(big_integer const &second, size_t start_pos = 0, bool is_sub = false);

    void make_unique();

    void push_back(uint32_t value);

    void pop_back();

    void make_big();

    size_t size() const;

    void set_digit(size_t pos, uint32_t value);

public:
    big_integer() noexcept;

    big_integer(int value) noexcept;

    explicit big_integer(std::string const &value);

    big_integer(big_integer const &big_int) noexcept;

    big_integer &operator=(big_integer const &big_int) noexcept;

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

    ~big_integer();
};

#endif