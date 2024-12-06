#ifndef WSTL_UNINITIALIZED_H
#define WSTL_UNINITIALIZED_H

// 这个头文件用于对未初始化的内存进行操作

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace wstl {

	/**
	 * uninitialized_copy
	 * @tparam InputIterator, ForwardIterator
	 * @param first, last, result
	 * @return ForwardIterator
	 * @note 将[first, last)区间内的元素拷贝到[result, result + (last - first))内
	 */

	template <class InputIterator, class ForwardIterator>
	ForwardIterator
	unchecked_uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, std::true_type) {
		return wstl::copy(first, last, result);
	}

	template <class InputIterator, class ForwardIterator>
	ForwardIterator
	unchecked_uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, std::false_type) {
		ForwardIterator cur = result;
		try {
			for (; first != last; ++first, ++cur) {
				wstl::construct(&*cur, *first);
			}
		} catch (...) {
			wstl::destroy(result, cur);
			cur = result;
		}
		return cur;
	}

	template <class InputIterator, class ForwardIterator>
	ForwardIterator
	uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) { // 平凡拷贝可直接复制内存，不需调用构造函数
		return unchecked_uninit_copy(first, last, result,
									 std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>());
	}

	/**
	 * uninitialized_copy_n
	 * @tparam InputIterator, Size, ForwardIterator
	 * @param first, n, result
	 * @return ForwardIterator
	 * @note 将[first, first + n)区间内的元素拷贝到[result, result + n)内，返回的是拷贝结束的位置
	 */

	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator
	unchecked_uninit_copy_n(InputIterator first, Size n, ForwardIterator result, std::true_type) {
		return wstl::copy_n(first, n, result).second;
	}

	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator
	unchecked_uninit_copy_n(InputIterator first, Size n, ForwardIterator result, std::false_type) {
		ForwardIterator cur = result;
		try {
			for (; n > 0; --n, ++first, ++cur) {
				wstl::construct(&*cur, *first);
			}
		} catch (...) {
			wstl::destroy(result, cur);
			cur = result;
		}
		return cur;
	}

	// 平凡拷贝可直接复制内存，不需调用构造函数
	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator
	uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result) {
		return unchecked_uninit_copy_n(first, n, result,
									   std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>());
	}

	/**
	 * uninitialized_fill
	 * @tparam ForwardIterator, T
	 * @param first, last, value
	 * @return void
	 * @note 将[first, last)区间内的元素填充为 value
	 */

	template <class ForwardIterator, class T>
	void unchecked_uninit_fill(ForwardIterator first, ForwardIterator last, const T &value, std::true_type) {
		wstl::fill(first, last, value);
	}

	template <class ForwardIterator, class T>
	void unchecked_uninit_fill(ForwardIterator first, ForwardIterator last, const T &value, std::false_type) {
		ForwardIterator cur = first;
		try {
			for (; cur != last; ++cur) {
				wstl::construct(&*cur, value);
			}
		} catch (...) {
			wstl::destroy(first, cur);
		}
	}

	template <class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value) {
		unchecked_uninit_fill(first, last, value,
							  std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>());
	}

	/**
	 * uninitialized_fill_n
	 * @tparam ForwardIterator, Size, T
	 * @param first, n, value
	 * @return ForwardIterator
	 * @note 将[first, first + n)区间内的元素填充为 value, 返回填充结束得位置
	 */

	template <class ForwardIterator, class Size, class T>
	ForwardIterator unchecked_uninit_fill_n(ForwardIterator first, Size n, const T &value, std::true_type) {
		return wstl::fill_n(first, n, value);
	}

	template <class ForwardIterator, class Size, class T>
	ForwardIterator unchecked_uninit_fill_n(ForwardIterator first, Size n, const T &value, std::false_type) {
		ForwardIterator cur = first;
		try {
			for (; n > 0; --n, ++cur) {
				wstl::construct(&*cur, value);
			}
		} catch (...) {
			wstl::destroy(first, cur);
			cur = first;
		}
		return cur;
	}

	template <class ForwardIterator, class Size, class T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &value) {
		return unchecked_uninit_fill_n(first, n, value,
									   std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>());
	}

	/**
	 * uninitialized_move
	 * @tparam InputIterator, InputIterator, ForwardIterator
	 * @param first, last, result
	 * @return ForwardIterator
	 * @note 将[first, last)区间得内容移动到以result为起始得区间中，返回移动结束得位置
	 */

	template <class InputIterator, class ForwardIterator>
	ForwardIterator unchecked_uninit_move(InputIterator first, InputIterator last, ForwardIterator result, std::true_type) {
		return wstl::move(first, last, result);
	}

	template <class InputIterator, class ForwardIterator>
	ForwardIterator unchecked_uninit_move(InputIterator first, InputIterator last, ForwardIterator result, std::false_type) {
		ForwardIterator cur = result;
		try {
			for (; first != last; ++first, ++cur) {
				wstl::construct(&*cur, wstl::move(*first));
			}
		} catch (...) {
			wstl::destroy(result, cur);
			cur = result;
		}
		return cur;
	}

	template <class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
		return wstl::unchecked_uninit_move(first, last, result,
										   std::is_trivially_move_assignable<typename iterator_traits<InputIterator>::value_type>());
	}

	/**
	 * uninitialized_move_n
	 * @tparam InputIterator, Size, ForwardIterator
	 * @param first, n, result
	 * @return ForwardIterator
	 * @note 将[first, first + n)区间得内容移动到以result为起始得区间中，返回移动结束得位置
	 */

	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator unchecked_uninit_move_n(InputIterator first, Size n, ForwardIterator result, std::true_type) {
		return wstl::move(first, first + n, result);
	}

	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator unchecked_uninit_move_n(InputIterator first, Size n, ForwardIterator result, std::false_type) {
		ForwardIterator cur = result;
		try {
			for (; n > 0; --n, ++first, ++cur) {
				wstl::construct(&*cur, wstl::move(*first));
			}
		} catch (...) {
			wstl::destroy(result, cur);
			cur = result;
		}
		return cur;
	}

	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator uninitialized_move_n(InputIterator first, Size n, ForwardIterator result) {
		return wstl::unchecked_uninit_move_n(first, n, result,
											 std::is_trivially_move_assignable<typename iterator_traits<InputIterator>::value_type>());
	}
}

#endif // WSTL_UNINITIALIZED_H