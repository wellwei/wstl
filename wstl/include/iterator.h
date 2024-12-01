#ifndef WSTL_ITERATOR_H
#define WSTL_ITERATOR_H

#include <cstddef>

#include "type_traits.h"

namespace wstl {
	// 迭代器标识

	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// iterator template
	template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
	struct iterator {
		typedef Category iterator_category;
		typedef T value_type;
		typedef Pointer pointer;
		typedef Reference reference;
		typedef Distance difference_type;
	};

	// iterator traits

	template <class T>
	struct has_iterator_cat { // 判断是否有iterator_category
	private:
		struct two {
			char a;
			char b;
		};

		template <class U>
		static two test(...);

		template <class U>
		static char test(typename U::iterator_category * = 0);

	public:
		static constexpr bool value = sizeof(test<T>(0)) == 1; // 如果有iterator_category，那么调用test<T>(0)返回的是char，sizeof(char) == 1，即value = 1
	};

	template <class Iterator, bool>
	struct iterator_traits_impl {};

	// 通过偏特化来实现iterator_traits
	template <class Iterator>
	struct iterator_traits_impl<Iterator, true> {
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type value_type;
		typedef typename Iterator::pointer pointer;
		typedef typename Iterator::reference reference;
		typedef typename Iterator::difference_type difference_type;
	};

	template <class Iterator, bool>
	struct iterator_traits_helper {};
	// 判断Iterator是否是有效的迭代器（基于input_iterator或output_iterator）
	template <class Iterator>
	struct iterator_traits_helper<Iterator, true>
		: public iterator_traits_impl<Iterator,
									  std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
										  std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};

	template <class Iterator>
	struct iterator_traits : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {
	};

	// 偏特化支持原生指针

	template <class T>
	struct iterator_traits<T *> {
		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef T *pointer;
		typedef T &reference;
		typedef ptrdiff_t difference_type;
	};

	template <class T>
	struct iterator_traits<const T *> {
		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef const T *pointer;
		typedef const T &reference;
		typedef ptrdiff_t difference_type;
	};

	// 是某种迭代器

	template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of : public wstl::w_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value> {};

	template <class T, class U>
	struct has_iterator_cat_of<T, U, false> : public wstl::w_false_type {};

	template <class Iterator>
	struct is_input_iterator : public has_iterator_cat_of<Iterator, input_iterator_tag> {};

	template <class Iterator>
	struct is_output_iterator : public has_iterator_cat_of<Iterator, output_iterator_tag> {};

	template <class Iterator>
	struct is_forward_iterator : public has_iterator_cat_of<Iterator, forward_iterator_tag> {};

	template <class Iterator>
	struct is_bidirectional_iterator : public has_iterator_cat_of<Iterator, bidirectional_iterator_tag> {};

	template <class Iterator>
	struct is_random_access_iterator : public has_iterator_cat_of<Iterator, random_access_iterator_tag> {};

	// 获取迭代器类型
	template <class Iterator>
	typename iterator_traits<Iterator>::iterator_category
	iterator_category(const Iterator &) {
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();
	}

	// 获取迭代器value_type
	template <class Iterator>
	typename iterator_traits<Iterator>::value_type *
	value_type(const Iterator &) {
		return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
	}

	// 获取迭代器distance_type
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type *
	distance_type(const Iterator &) {
		return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
	}

	// 计算迭代器距离

	// input_iterator_tag
	template <class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
	__distance(InputIterator first, InputIterator last, input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last) {
			++first;
			++n;
		}
		return n;
	}

	// random_access_iterator_tag
	template <class RandomAccessIterator>
	typename iterator_traits<RandomAccessIterator>::difference_type
	__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		return last - first;
	}

	template <class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
	distance(InputIterator first, InputIterator last) {
		return __distance(first, last, iterator_category(first));
	}

	// 迭代器前进n步

	// input_iterator_tag
	template <class InputIterator, class Distance>
	void __advance(InputIterator &i, Distance n, input_iterator_tag) {
		while (n--) {
			++i;
		}
	}

	// bidirectional_iterator_tag
	template <class BidirectionalIterator, class Distance>
	void __advance(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
		if (n > 0) {
			while (n--) {
				++i;
			}
		} else {
			while (n++) {
				--i;
			}
		}
	}

	// random_access_iterator_tag
	template <class RandomAccessIterator, class Distance>
	void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag) {
		i += n;
	}

	template <class InputIterator, class Distance>
	void advance(InputIterator &i, Distance n) {
		__advance(i, n, iterator_category(i));
	}

	// -------------------reverse_iterator-------------------

	// reverse_iterator 模板类， 逆向迭代器
	template <class Iterator>
	class reverse_iterator {
	private:
		Iterator current; // 对应的正向迭代器
	public:
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<Iterator>::value_type value_type;
		typedef typename iterator_traits<Iterator>::difference_type difference_type;
		typedef typename iterator_traits<Iterator>::pointer pointer;
		typedef typename iterator_traits<Iterator>::reference reference;

		typedef Iterator iterator_type;
		typedef reverse_iterator<Iterator> self;

	public:
		// 构造函数
		reverse_iterator() {}
		explicit reverse_iterator(iterator_type x) : current(x) {}
		reverse_iterator(const self &x) : current(x.current) {}

		// 获取对应的正向迭代器
		iterator_type base() const { return current; }

		// 重载运算符

		reference operator*() const {
			Iterator tmp = current;
			return *--tmp; // 实际上是返回当前迭代器的前一个位置的值
		}

		pointer operator->() const { return &(operator*()); }

		// 前进

		self &operator++() {
			--current;
			return *this;
		}

		self operator++(int) {
			self tmp = *this;
			--current;
			return tmp;
		}

		// 后退

		self &operator--() {
			++current;
			return *this;
		}

		self operator--(int) {
			self tmp = *this;
			++current;
			return tmp;
		}

		// 前进n步

		self &operator+=(difference_type n) {
			current -= n;
			return *this;
		}

		self operator+(difference_type n) const {
			return self(current - n);
		}

		// 后退n步

		self &operator-=(difference_type n) {
			current += n;
			return *this;
		}

		self operator-(difference_type n) const {
			return self(current + n);
		}

		// 访问当前位置的元素

		reference operator[](difference_type n) const { return *(*this + n); }
	};

	// 重载 operator-
	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type
	operator-(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return rhs.base() - lhs.base();
	}

	// 重载比较运算符

	template <class Iterator>
	bool operator==(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() == rhs.base();
	}

	template <class Iterator>
	bool operator!=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() != rhs.base();
	}

	template <class Iterator>
	bool operator<(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() > rhs.base();
	}

	template <class Iterator>
	bool operator<=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() >= rhs.base();
	}

	template <class Iterator>
	bool operator>(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() < rhs.base();
	}

	template <class Iterator>
	bool operator>=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
		return lhs.base() <= rhs.base();
	}
}

#endif // WSTL_ITERATOR_H