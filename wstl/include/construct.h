#ifndef WSTL_CONSTRUCT_H
#define WSTL_CONSTRUCT_H

// 该头文件包含了 placement new（construct）和 destroy 函数

#include <new>

#include "iterator.h"
#include "type_traits.h"
#include "util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100) // C4100: 未引用的形参
#endif

namespace wstl {

	// construct

	template <class Ty>
	void construct(Ty *ptr) {
		::new ((void *)ptr) Ty();
	}

	template <class Ty1, class Ty2>
	void construct(Ty1 *ptr, const Ty2 &value) {
		::new ((void *)ptr) Ty1(value);
	}

	template <class Ty, class... Args>
	void construct(Ty *ptr, Args &&...args) {
		::new ((void *)ptr) Ty(wstl::forward<Args>(args)...);
	}

	// destroy

	template <class Ty>
	void destroy_one(Ty *, std::true_type) {}

	template <class Ty>
	void destroy_one(Ty *pointer, std::false_type) {
		if (pointer != nullptr) {
			pointer->~Ty();
		}
	}

	template <class ForwardIterator>
	void destroy_cat(ForwardIterator, ForwardIterator, std::true_type) {}

	template <class ForwardIterator>
	void destroy_cat(ForwardIterator first, ForwardIterator last, std::false_type) {
		for (; first != last; ++first) {
			destroy(&*first);
		}
	}

	template <class Ty>
	void destroy(Ty *pointer) {
		destroy_one(pointer, std::is_trivially_destructible<Ty>()); // 没有析构函数，不需要手动析构，系统会自动析构，提高效率
	}

	template <class ForwardIterator>
	void destroy(ForwardIterator first, ForwardIterator last) {
		destroy_cat(first, last,
					std::is_trivially_destructible<typename iterator_traits<ForwardIterator>::value_type>());
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // WSTL_CONSTRUCT_H