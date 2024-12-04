//
// Created by wellwei on 2024/12/5.
//

#ifndef WSTL_ALGOBASE_H
#define WSTL_ALGOBASE_H

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace wstl {
// 取消宏定义
#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif

    /**
     * copy
     * @tparam InputIterator, OutputIterator
     * @param first, last, result
     * @return OutputIterator
     * @note 将[first, last)区间内的元素拷贝到[result, result + (last - first))内
     */

    // input_iterator_tag
    template<class InputIterator, class OutputIterator>
    OutputIterator
    uncheck_copy_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            *result = *first;
        }
        return result;
    }

    // random_access_iterator_tag
    template<class InputIterator, class OutputIterator>
    OutputIterator
    uncheck_copy_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *result = *first;
            ++first;
            ++result;
        }
        return result;
    }

    template<class InputIterator, class OutputIterator>
    OutputIterator
    uncheck_copy(InputIterator first, InputIterator last, OutputIterator result) {
        return uncheck_copy_cat(first, last, result, wstl::iterator_category(first));
    }

    template<class Tp, class Up>
    // 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_copy_assignable<Up>::value, Up *>::type
    uncheck_copy(Tp *first, Tp *last, Up *result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return result + n;
    }

    template<class InputIterator, class OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return uncheck_copy(first, last, result);
    }

    /**
     * copy_backward
     * @tparam BidirectionalIterator1, BidirectionalIterator2
     * @param first, last, result
     * @return BidirectionalIterator2
     * @note 将[first, last)区间内的元素拷贝到[result - (last - first), result)内
     */

    // bidirectional_iterator_tag
    template<class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2
    uncheck_copy_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
                              wstl::bidirectional_iterator_tag) {
        while (first != last) {
            *--result = *--last;
        }
        return result;
    }

    // random_access_iterator_tag
    template<class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2
    uncheck_copy_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
                              wstl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *--result = *--last;
        }
        return result;
    }

    template<class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2
    uncheck_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return uncheck_copy_backward_cat(first, last, result, wstl::iterator_category(first));
    }

    // 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
    template<class Tp, class Up>
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_copy_assignable<Up>::value, Up *>::type
    uncheck_copy_backward(Tp *first, Tp *last, Up *result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template<class BidrectionalIterator1, class BidrectionalIterator2>
    BidrectionalIterator2
    copy_backward(BidrectionalIterator1 first, BidrectionalIterator1 last, BidrectionalIterator2 result) {
        return uncheck_copy_backward(first, last, result);
    }


    /**
     * copy_n
     * @tparam InputIterator, Size, ForwardIterator
     * @param first, n, result
     * @return wstl::pair<InputIterator, ForwardIterator>
     * @note 将[first, first + n)区间内的元素拷贝到[result, result + n)内，返回一个pair，first指向[first + n), second指向[result + n)
     */

    template<class InputIterator, class Size, class ForwardIterator>
    wstl::pair<InputIterator, ForwardIterator>
    uncheck_copy_n(InputIterator first, Size n, ForwardIterator result, wstl::input_iterator_tag) {
        for (; n > 0; --n, ++first, ++result) {
            *result = *first;
        }
        return wstl::pair<InputIterator, ForwardIterator>(first, result);
    }

    template<class InputIterator, class Size, class ForwardIterator>
    wstl::pair<InputIterator, ForwardIterator>
    uncheck_copy_n(InputIterator first, Size n, ForwardIterator result, wstl::random_access_iterator_tag) {
        auto last = first + n;
        return wstl::pair<InputIterator, ForwardIterator>(last, wstl::copy(first, last, result));
    }

    template<class InputIterator, class Size, class ForwardIterator>
    wstl::pair<InputIterator, ForwardIterator>
    copy_n(InputIterator first, Size n, ForwardIterator result) {
        return uncheck_copy_n(first, n, result, wstl::iterator_category(first));
    }

    /**
     * copy_if
     * @tparam InputIterator, OutputIterator, UnaryPredicate
     * @param first, last, result, pred
     * @return OutputIterator
     * @note 将[first, last)区间内满足一元操作 pred 的元素拷贝到 result 为起始的区间内
     */
    template<class InputIterator, class OutputIterator, class UnaryPredicate>
    OutputIterator
    copy_if(InputIterator first, InputIterator last, OutputIterator result, UnaryPredicate pred) {
        for (; first != last; ++first) {
            if (pred(*first)) {
                *result = *first;
                ++result;
            }
        }
        return result;
    }

    /**
     * fill
     * @tparam ForwardIterator, T
     * @param first, last, value
     * @return void
     * @note 将[first, last)区间内的元素填充为 value
     */

    template<class ForwardIterator, class T>
    void fill_cat(ForwardIterator first, ForwardIterator last, const T &value, wstl::forward_iterator_tag) {
        for (; first != last; ++first) {
            *first = value;
        }
    }

    template<class RandomAccessIterator, class T>
    void
    fill_cat(RandomAccessIterator first, RandomAccessIterator last, const T &value, wstl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *first = value;
            ++first;
        }
    }
}

#endif //WSTL_ALGOBASE_H
