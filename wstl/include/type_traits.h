/*
	该文件主要实现类型萃取
*/

#ifndef WSTL_TYPE_TRAITS_H
#define WSTL_TYPE_TRAITS_H

#include <cstddef>
#include <type_traits>

namespace wstl {

	// helper struct
	template <class T, T v>
	struct w_integral_constant {
		static constexpr T value = v;
	};

	template <bool B>
	using w_bool_constant = w_integral_constant<bool, B>;

	typedef w_bool_constant<true> w_true_type;
	typedef w_bool_constant<false> w_false_type;

	// --------------type_traits----------------

	// remove_reference

	template <typename T>
	struct remove_reference {
		typedef T type;
	};

	template <typename T>
	struct remove_reference<T &> {
		typedef T type;
	};

	template <typename T>
	struct remove_reference<T &&> {
		typedef T type;
	};

	// is_pair

	template <typename T1, typename T2>
	struct pair;

	template <typename T>
	struct is_pair : wstl::w_false_type {
	};

	template <typename T1, typename T2>
	struct is_pair<wstl::pair<T1, T2>> : wstl::w_true_type {
	};
}

#endif // WSTL_TYPE_TRAITS_H