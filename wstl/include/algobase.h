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

	/*****************************************************************************************/
	// 										辅助函数
	/*****************************************************************************************/

	/**
	 * copy
	 * @tparam InputIterator, OutputIterator
	 * @param first, last, result
	 * @return OutputIterator
	 * @note 将[first, last)区间内的元素拷贝到[result, result + (last - first))内
	 */

	// input_iterator_tag
	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_copy_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::input_iterator_tag) {
		for (; first != last; ++first, ++result) {
			*result = *first;
		}
		return result;
	}

	// random_access_iterator_tag
	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_copy_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*result = *first;
			++first;
			++result;
		}
		return result;
	}

	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_copy(InputIterator first, InputIterator last, OutputIterator result) {
		return unchecked_copy_cat(first, last, result, wstl::iterator_category(first));
	}

	template <class Tp, class Up>
	// 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
	typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
								std::is_trivially_copy_assignable<Up>::value,
							Up *>::type
	unchecked_copy(Tp *first, Tp *last, Up *result) {
		const auto n = static_cast<size_t>(last - first);
		if (n != 0) {
			std::memmove(result, first, n * sizeof(Up));
		}
		return result + n;
	}

	template <class InputIterator, class OutputIterator>
	OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return unchecked_copy(first, last, result);
	}

	/**
	 * copy_backward
	 * @tparam BidirectionalIterator1, BidirectionalIterator2
	 * @param first, last, result
	 * @return BidirectionalIterator2
	 * @note 将[first, last)区间内的元素拷贝到[result - (last - first), result)内
	 */

	// bidirectional_iterator_tag
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_copy_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
								wstl::bidirectional_iterator_tag) {
		while (first != last) {
			*--result = *--last;
		}
		return result;
	}

	// random_access_iterator_tag
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_copy_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
								wstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*--result = *--last;
		}
		return result;
	}

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
		return unchecked_copy_backward_cat(first, last, result, wstl::iterator_category(first));
	}

	// 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
	template <class Tp, class Up>
	typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
								std::is_trivially_copy_assignable<Up>::value,
							Up *>::type
	unchecked_copy_backward(Tp *first, Tp *last, Up *result) {
		const auto n = static_cast<size_t>(last - first);
		if (n != 0) {
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator1 result) {
		return unchecked_copy_backward(first, last, result);
	}

	/**
	 * copy_if
	 * @tparam InputIterator, OutputIterator, UnaryPredicate
	 * @param first, last, result, pred
	 * @return OutputIterator
	 * @note 将[first, last)区间内满足一元操作 pred 的元素拷贝到 result 为起始的区间内
	 */
	template <class InputIterator, class OutputIterator, class UnaryPredicate>
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
	 * copy_n
	 * @tparam InputIterator, Size, OutputIterator
	 * @param first, n, result
	 * @return wstl::pair<InputIterator, OutputIterator>
	 * @note 将[first, first + n)区间内的元素拷贝到[result, result + n)内，返回一个pair，first指向[first + n), second指向[result + n)
	 */

	template <class InputIterator, class Size, class OutputIterator>
	wstl::pair<InputIterator, OutputIterator>
	unchecked_copy_n(InputIterator first, Size n, OutputIterator result, wstl::input_iterator_tag) {
		for (; n > 0; --n, ++first, ++result) {
			*result = *first;
		}
		return wstl::pair<InputIterator, OutputIterator>(first, result);
	}

	template <class InputIterator, class Size, class OutputIterator>
	wstl::pair<InputIterator, OutputIterator>
	unchecked_copy_n(InputIterator first, Size n, OutputIterator result, wstl::random_access_iterator_tag) {
		auto last = first + n;
		return wstl::pair<InputIterator, OutputIterator>(last, wstl::copy(first, last, result));
	}

	template <class InputIterator, class Size, class OutputIterator>
	wstl::pair<InputIterator, OutputIterator>
	copy_n(InputIterator first, Size n, OutputIterator result) {
		return unchecked_copy_n(first, n, result, wstl::iterator_category(first));
	}

	/**
	 * fill_n
	 * @tparam OutputIterator, Size, T
	 * @param first, n, value
	 * @return OutputIterator
	 * @note 将[first, first + n)区间内的元素填充为 value, 返回填充结束得位置
	 */

	template <class OutputIterator, class Size, class T>
	OutputIterator unchecked_fill_n(OutputIterator first, Size n, const T &value) {
		for (; n > 0; --n, ++first) {
			*first = value;
		}
		return first;
	}

	// one-byte 类型的特化版本，直接使用 std::memset 填充
	template <class Tp, class Size, class Up>
	typename std::enable_if<
		std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
			!std::is_same<Tp, bool>::value &&
			std::is_integral<Up>::value && sizeof(Up) == 1,
		Up *>::type
	unchecked_fill_n(Tp *first, Size n, Up value) {
		if (n > 0) {
			std::memset(first, (unsigned char)value, n);
		}
		return first + n;
	}

	template <class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
		return unchecked_fill_n(first, n, value);
	}

	/**
	 * fill
	 * @tparam ForwardIterator, T
	 * @param first, last, value
	 * @return void
	 * @note 将[first, last)区间内的元素填充为 value
	 */

	template <class ForwardIterator, class T>
	void fill_cat(ForwardIterator first, ForwardIterator last, const T &value, wstl::forward_iterator_tag) {
		for (; first != last; ++first) {
			*first = value;
		}
	}

	template <class RandomAccessIterator, class T>
	void
	fill_cat(RandomAccessIterator first, RandomAccessIterator last, const T &value, wstl::random_access_iterator_tag) {
		fill_n(first, last - first, value);
	}

	template <class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T &value) {
		fill_cat(first, last, value, wstl::iterator_category(first));
	}

	/**
	 * move
	 * @tparam InputIterator, OutputIterator
	 * @param first, last, result
	 * @return OutputIterator
	 * @note 将[first, last)区间内的元素移动到[result, result + (last - first))内, 返回移动结束的位置
	 */

	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_move_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::input_iterator_tag) {
		for (; first != last; ++first, ++result) {
			*result = wstl::move(*first);
		}
		return result;
	}

	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_move_cat(InputIterator first, InputIterator last, OutputIterator result, wstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*result = wstl::move(*first);
			++first;
			++result;
		}
		return result;
	}

	template <class InputIterator, class OutputIterator>
	OutputIterator
	unchecked_move(InputIterator first, InputIterator last, OutputIterator result) {
		return unchecked_move_cat(first, last, result, wstl::iterator_category(first));
	}

	// 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
			std::is_trivially_copy_assignable<Up>::value,
		Up *>::type
	unchecked_move(Tp *first, Tp *last, Up *result) {
		const auto n = static_cast<size_t>(last - first);
		if (n != 0) {
			std::memmove(result, first, n * sizeof(Up));
		}
		return result + n;
	}

	template <class InputIterator, class OutputIterator>
	OutputIterator move(InputIterator first, InputIterator last, OutputIterator result) {
		return unchecked_move(first, last, result);
	}

	/**
	 * move_backward
	 * @tparam BidirectionalIterator1, BidirectionalIterator2
	 * @param first, last, result
	 * @return BidirectionalIterator2
	 * @note 将[first, last)区间内的元素移动到[result - (last - first), result)内, 返回移动结束的位置
	 */

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_move_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
								wstl::bidirectional_iterator_tag) {
		while (first != last) {
			*--result = wstl::move(*--last);
		}
		return result;
	}

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_move_backward_cat(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result,
								wstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*--result = wstl::move(*--last);
		}
		return result;
	}

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	unchecked_move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
		return unchecked_move_backward_cat(first, last, result, wstl::iterator_category(first));
	}

	// 特化版本，类型相同且是 trivially_copy_assignable 直接拷贝内存
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
			std::is_trivially_copy_assignable<Up>::value,
		Up *>::type
	unchecked_move_backward(Tp *first, Tp *last, Up *result) {
		const auto n = static_cast<size_t>(last - first);
		if (n != 0) {
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2
	move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator1 result) {
		return unchecked_move_backward(first, last, result);
	}

	/*****************************************************************************************/
	// 										简单常用函数
	/*****************************************************************************************/

	// min

	template <class T>
	const T &min(const T &a, const T &b) {
		return b < a ? b : a;
	}

	template <class T, class Compare>
	const T &min(const T &a, const T &b, Compare comp) {
		return comp(b, a) ? b : a;
	}

	// max

	template <class T>
	const T &max(const T &a, const T &b) {
		return a < b ? b : a;
	}

	template <class T, class Compare>
	const T &max(const T &a, const T &b, Compare comp) {
		return comp(a, b) ? b : a;
	}

	// iter_swap, 交换迭代器指向的元素

	template <class ForwardIterator1, class ForwardIterator2>
	void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
		wstl::swap(*a, *b);
	}

	// equal, 比较两个区间内的元素是否相等

	template <class InputIterator1, class InputIterator2>
	bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2) {
			if (*first1 != *first2) {
				return false;
			}
		}
		return true;
	}

	template <class InputIterator1, class InputIterator2, class Compare>
	bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Compare comp) {
		for (; first1 != last1; ++first1, ++first2) {
			if (!comp(*first1, *first2)) {
				return false;
			}
		}
		return true;
	}

	// lexicographical_compare, 比较两个区间的字典序大小
	// 第一个区间的元素小于第二个区间的元素，返回 true，否则返回 false
	// 如果第一个区间是第二个区间的前缀，返回 true
	// 如果第二个区间是第一个区间的前缀，返回 false
	// 如果两个区间相等，返回 false

	template <class InputIterator1, class InputIterator2>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (*first1 < *first2) {
				return true;
			}
			if (*first2 < *first1) {
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}

	template <class InputIterator1, class InputIterator2, class Compare>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
								 Compare comp) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (comp(*first1, *first2)) {
				return true;
			}
			if (comp(*first2, *first1)) {
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}

	// const unsigned char * 版本的特化版本，直接使用 std::memcmp 比较
	template <>
	bool lexicographical_compare(const unsigned char *first1, const unsigned char *last1, const unsigned char *first2,
								 const unsigned char *last2) {
		const auto len1 = static_cast<size_t>(last1 - first1);
		const auto len2 = static_cast<size_t>(last2 - first2);
		const auto result = std::memcmp(first1, first2, wstl::min(len1, len2));
		return result != 0 ? result < 0 : len1 < len2;
	}

	// mismatch, 在两个区间中找到第一个不匹配的元素

	template <class InputIterator1, class InputIterator2>
	wstl::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return wstl::pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <class InputIterator1, class InputIterator2, class Compare>
	wstl::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
														Compare comp) {
		while (first1 != last1 && comp(*first1, *first2)) {
			++first1;
			++first2;
		}
		return wstl::pair<InputIterator1, InputIterator2>(first1, first2);
	}
}

#endif // WSTL_ALGOBASE_H
