#include "big_integer.h"
#include <iostream>

int main() {
    big_integer a(-20);
    big_integer b(4);
    std::cout << to_string(a / b);
}

