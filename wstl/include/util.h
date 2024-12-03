
// File: util.h

// Description: 本文件定义了一些工具函数，包括 move, forward, swap, pair 等。

#ifndef WSTL_UTIL_H
#define WSTL_UTIL_H

#include <cstddef>

#include "type_traits.h"

namespace wstl {

	// move， 将左值转换为右值

	template <class T>
	typename wstl::remove_reference<T>::type &&move(T &&t) noexcept {
		typedef typename wstl::remove_reference<T>::type U;
		return static_cast<U &&>(t);
	}

	// forward, 利用引用折叠来实现完美转发，返回值类型是 T&&，表示既能接受左值，也能接受右值
	/*
		T& && 折叠为 T&
		T&& && 折叠为 T&&

		如果 T 是 U&，那么 T&& 会折叠为 U&，保持左值特性。
		如果 T 是 U&&，那么 T&& 会保持为 U&&，保持右值特性。

	*/

	// 接受左值引用，返回左值引用
	template <class T>
	T &&forward(typename wstl::remove_reference<T>::type &arg) noexcept {
		return static_cast<T &&>(arg);
	}

	// 接受右值引用，返回右值引用
	template <class T>
	T &&forward(typename wstl::remove_reference<T>::type &&arg) noexcept {
		static_assert(!std::is_lvalue_reference<T>::value, "forward called with an rvalue reference");
		return static_cast<T &&>(arg);
	}

	// swap

	template <class T>
	void swap(T &a, T &b) {
		T tmp(wstl::move(a));
		a = wstl::move(b);
		b = wstl::move(tmp);
	}

	// swap range
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2) {
			wstl::swap(*first1, *first2);
		}
		return first2;
	}

	// swap array
	template <class Tp, size_t N>
	void swap(Tp (&a)[N], Tp (&b)[N]) {
		wstl::swap_range(a, a + N, b);
	}

	// --------------pair----------------

	template <class T1, class T2>
	struct pair {
		typedef T1 first_type;
		typedef T2 second_type;

		first_type first;
		second_type second;

		// 利用SFINAE机制，自动推导对应的构造函数
		// SFINAE机制，在编译期间，根据模板参数的类型，选择合适的函数进行调用

		// 默认构造函数， 检查 T1 和 T2 是否有默认构造函数
		template <class U1 = T1, class U2 = T2,
				  typename = typename std::enable_if<
					  std::is_default_constructible<U1>::value &&
						  std::is_default_constructible<U2>::value,
					  void>::type>
		constexpr pair() : first(), second() {}

		// 拷贝构造
		pair(const pair &rhs) = default;

		// 移动构造
		pair(pair &&rhs) = default;

		// 构造，有多个版本，利用 SFINAE 机制，选择合适的构造函数

		// 隐式构造函数（同类型）
		template <class U1 = T1, class U2 = T2,
				  typename std::enable_if<
					  std::is_copy_constructible<U1>::value &&
						  std::is_copy_constructible<U2>::value &&
						  std::is_convertible<const U1 &, U1>::value &&
						  std::is_convertible<const U2 &, U2>::value,
					  int>::type = 0>
		constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

		// 显示构造函数（同类型）
		template <class U1 = T1, class U2 = T2,
				  typename std::enable_if<
					  std::is_copy_constructible<U1>::value &&
						  std::is_copy_constructible<U2>::value &&
						  (!std::is_convertible<const U1 &, U1>::value ||
						   !std::is_convertible<const U2 &, U2>::value),
					  int>::type = 0>
		explicit constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

		// 隐式构造函数（不同类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, Other1>::value &&
						  std::is_constructible<T2, Other2>::value &&
						  std::is_convertible<Other1, T1>::value &&
						  std::is_convertible<Other2, T2>::value,
					  int>::type = 0>
		constexpr pair(Other1 &&a, Other2 &&b) : first(wstl::forward<Other1>(a)), second(wstl::forward<Other2>(b)) {}

		// 显示构造函数（不同类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, Other1>::value &&
						  std::is_constructible<T2, Other2>::value &&
						  (!std::is_convertible<Other1, T1>::value ||
						   !std::is_convertible<Other2, T2>::value),
					  int>::type = 0>
		explicit constexpr pair(Other1 &&a, Other2 &&b) : first(wstl::forward<Other1>(a)), second(wstl::forward<Other2>(b)) {}

		// 隐式构造函数（其他pair类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, const Other1 &>::value &&
						  std::is_constructible<T2, const Other2 &>::value &&
						  std::is_convertible<const Other1 &, T1>::value &&
						  std::is_convertible<const Other2 &, T2>::value,
					  int>::type = 0>
		constexpr pair(const pair<Other1, Other2> &other) : first(other.first), second(other.second) {}

		// 显示构造函数（其他pair类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, const Other1 &>::value &&
						  std::is_constructible<T2, const Other2 &>::value &&
						  (!std::is_convertible<const Other1 &, T1>::value ||
						   !std::is_convertible<const Other2 &, T2>::value),
					  int>::type = 0>
		explicit constexpr pair(const pair<Other1, Other2> &other) : first(other.first), second(other.second) {}

		// 隐式移动构造函数（其他pair类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, Other1>::value &&
						  std::is_constructible<T2, Other2>::value &&
						  std::is_convertible<Other1, T1>::value &&
						  std::is_convertible<Other2, T2>::value,
					  int>::type = 0>
		constexpr pair(pair<Other1, Other2> &&other) : first(wstl::forward<Other1>(other.first)), second(wstl::forward<Other2>(other.second)) {}

		// 显示移动构造函数（其他pair类型）
		template <class Other1, class Other2,
				  typename std::enable_if<
					  std::is_constructible<T1, Other1>::value &&
						  std::is_constructible<T2, Other2>::value &&
						  (!std::is_convertible<Other1, T1>::value ||
						   !std::is_convertible<Other2, T2>::value),
					  int>::type = 0>
		explicit constexpr pair(pair<Other1, Other2> &&other) : first(wstl::forward<Other1>(other.first)), second(wstl::forward<Other2>(other.second)) {}

		// 拷贝赋值（同类型）
		pair &operator=(const pair &rhs) {
			if (this != &rhs) {
				first = rhs.first;
				second = rhs.second;
			}
			return *this;
		}

		// 移动赋值（同类型）
		pair &operator=(pair &&rhs) {
			if (this != &rhs) {
				first = wstl::move(rhs.first);
				second = wstl::move(rhs.second);
			}
			return *this;
		}

		// 拷贝赋值（其他pair类型）
		template <class Other1, class Other2>
		pair &operator=(const pair<Other1, Other2> &rhs) {
			first = rhs.first;
			second = rhs.second;
			return *this;
		}

		// 移动赋值（其他pair类型）
		template <class Other1, class Other2>
		pair &operator=(pair<Other1, Other2> &&rhs) {
			first = wstl::forward<Other1>(rhs.first);
			second = wstl::forward<Other2>(rhs.second);
			return *this;
		}

		// 默认析构函数
		~pair() = default;

		// 交换
		void swap(pair &other) {
			if (this != &other) {
				wstl::swap(first, other.first);
				wstl::swap(second, other.second);
			}
		}
	};

	// 重载比较操作符

	template <class T1, class T2>
	bool operator==(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	template <class T1, class T2>
	bool operator!=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return !(lhs == rhs);
	}

	template <class T1, class T2>
	bool operator<(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
	}

	template <class T1, class T2>
	bool operator>(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return rhs < lhs;
	}

	template <class T1, class T2>
	bool operator<=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return !(rhs < lhs);
	}

	template <class T1, class T2>
	bool operator>=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
		return !(lhs < rhs);
	}

	// 重载swap
	template <class T1, class T2>
	void swap(pair<T1, T2> &lhs, pair<T1, T2> &rhs) {
		lhs.swap(rhs);
	}

	// make_pair
	template <class T1, class T2>
	pair<T1, T2> make_pair(T1 &&t1, T2 &&t2) {
		return pair<T1, T2>(wstl::forward<T1>(t1), wstl::forward<T2>(t2));
	}

}

#endif // WSTL_UTIL_H
