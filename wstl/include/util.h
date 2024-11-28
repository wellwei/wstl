//
// Created by wellwei on 2024/11/29.
//

#ifndef WSTL_UTIL_H
#define WSTL_UTIL_H

#include <cstddef>

#include "type_traits.h"

namespace wstl {

    // move

    template<class T>
    typename wstl::remove_reference<T>::type &&move(T &&t) noexcept {
        typedef typename wstl::remove_reference<T>::type U;
        return static_cast<U &&>(t);
    }

    template<class T1, class T2>
    struct pair {
        T1 first;
        T2 second;

        pair() : first(T1()), second(T2()) {}

        pair(const T1 &t1, const T2 &t2) : first(t1), second(t2) {}
    };

}

#endif //WSTL_UTIL_H
