#include "big_integer.h"
#include <string>
#include <vector>

big_integer::big_integer() {
    vec.push_back(0);
}

big_integer::big_integer(int value) {
    vec.push_back(value);
    //TODO negative
}

big_integer::big_integer(std::string const& value) {
    vec.push_back(0);
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

