#include <iostream>
#include <utility>

#include "type_traits.h"
#include "alloc.h"
#include "util.h"

int main() {

    std::cout << wstl::is_pair<wstl::pair<int, int>>::value << std::endl;

    int a = 10;

    int &&b = wstl::move(a);

    a = 20;

    std::cout << a << std::endl;
    std::cout << b << std::endl;

    return 0;
}