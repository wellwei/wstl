#ifndef WSTL_VECTOR_H
#define WSTL_VECTOR_H

/*
	该文件实现 vector 容器及其相关操作

	异常保证：
	vector<T> 满足基本异常保证，部分函数无异常保证，以下函数强异常安全保证：
		emplace，emplace_back，push_back
	当 std::is_nothrow_move_constructible<T>::value 为 true 时，以下函数提供强异常安全保证：
		insert，resize，reserve
*/

#include <initializer_list>

#include "algo.h"
#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "util.h"

namespace wstl {

#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif

	// vector 类模板
	template <class T, class Alloc = wstl::allocator<T>>
	class vector {
		static_assert(!std::is_same<T, bool>::value, "vector<bool> is abandoned in wstl");

	public:
		// vector 的嵌套型别定义
		typedef Alloc allocator_type;
		typedef Alloc data_allocator;

		typedef typename allocator_type::value_type value_type;
		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::const_pointer const_pointer;
		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::const_reference const_reference;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;

		typedef pointer iterator;
		typedef const_pointer const_iterator;
		typedef wstl::reverse_iterator<iterator> reverse_iterator;
		typedef wstl::reverse_iterator<const_iterator> const_reverse_iterator;

		allocator_type get_allocator() const {
			return data_allocator();
		}

	private:
		iterator begin_;
		iterator end_;
		iterator cap_;

	public:
		// 构造、复制、移动、析构函数

		vector() noexcept {
			try_init();
		}

		explicit vector(size_type n) {
			fill_init(n, value_type());
		}

		vector(size_type n, const value_type &value) {
			fill_init(n, value);
		}

		template <class InputIterator, typename std::enable_if<wstl::is_input_iterator<InputIterator>::value, int>::type = 0>
		vector(InputIterator first, InputIterator last) {
			WSTL_DEBUG(!(last < first));
			range_init(first, last);
		}

		vector(std::initializer_list<value_type> il) {
			range_init(il.begin(), il.end());
		}

		vector(const vector &rhs) {
			range_init(rhs.begin_, rhs.end_);
		}

		vector(vector &&rhs) noexcept {
			begin_ = rhs.begin_;
			end_ = rhs.end_;
			cap_ = rhs.cap_;
			rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
		}

		vector &operator=(const vector &rhs);

		vector &operator=(vector &&rhs) noexcept;

		vector &operator=(std::initializer_list<value_type> il) {
			vector tmp(il.begin(), il.end());
			swap(tmp);
			return *this;
		}

		~vector() {
			destroy_and_recover(begin_, end_, cap_ - begin_);
			begin_ = end_ = cap_ = nullptr;
		}

	public:
		// 迭代器相关操作

		iterator begin() noexcept {
			return begin_;
		}

		const_iterator begin() const noexcept {
			return begin_;
		}

		iterator end() noexcept {
			return end_;
		}

		const_iterator end() const noexcept {
			return end_;
		}

		reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}

		const_iterator cbegin() const noexcept {
			return begin();
		}

		const_iterator cend() const noexcept {
			return end();
		}

		const_reverse_iterator crbegin() const noexcept {
			return rbegin();
		}

		const_reverse_iterator crend() const noexcept {
			return rend();
		}

		// 容量相关操作

		size_type size() const noexcept {
			return static_cast<size_type>(end_ - begin_);
		}

		size_type capacity() const noexcept {
			return static_cast<size_type>(cap_ - begin_);
		}

		bool empty() const noexcept {
			return begin_ == end_;
		}

		void reserve(size_type n);

		void shrink_to_fit();

		size_type max_size() const noexcept {
			return static_cast<size_type>(-1) / sizeof(T);
		}

		// 访问元素相关操作

		reference operator[](size_type n) {
			WSTL_DEBUG(n < size());
			return *(begin_ + n);
		}

		const_reference operator[](size_type n) const {
			WSTL_DEBUG(n < size());
			return *(begin_ + n);
		}

		reference at(size_type n) {
			THROW_OUT_OF_RANGE_IF(n >= size(), "vector<T> : out of range");
			return (*this)[n];
		}

		const_reference at(size_type n) const {
			THROW_OUT_OF_RANGE_IF(n >= size(), "vector<T> : out of range");
			return (*this)[n];
		}

		reference front() {
			WSTL_DEBUG(!empty());
			return *begin_;
		}

		const_reference front() const {
			WSTL_DEBUG(!empty());
			return *begin_;
		}

		reference back() {
			WSTL_DEBUG(!empty());
			return *(end_ - 1);
		}

		const_reference back() const {
			WSTL_DEBUG(!empty());
			return *(end_ - 1);
		}

		pointer data() noexcept {
			return begin_;
		}

		const_pointer data() const noexcept {
			return begin_;
		}

		// 修改容器相关操作

		// assign

		void assign(size_type n, const value_type &value) {
			fill_assign(n, value);
		}

		template <class InputIterator, typename std::enable_if<wstl::is_input_iterator<InputIterator>::value, int>::type = 0>
		void assign(InputIterator first, InputIterator last) {
			WSTL_DEBUG(!(last < first));
			copy_assign(first, last);
		}

		void assign(std::initializer_list<value_type> il) {
			copy_assign(il.begin(), il.end());
		}

		// emplace / emplace_back

		template <class... Args>
		iterator emplace(const_iterator position, Args &&...args);

		template <class... Args>
		void emplace_back(Args &&...args);

		// push_back / pop_back

		void push_back(const value_type &value);

		void push_back(value_type &&value) {
			emplace_back(wstl::move(value));
		}

		void pop_back();

		// insert

		iterator insert(const_iterator position, const value_type &value);

		iterator insert(const_iterator position, value_type &&value) {
			return emplace(position, wstl::move(value));
		}

		iterator insert(const_iterator position, size_type n, const value_type &value) {
			WSTL_DEBUG(position >= begin() && position <= end());
			return fill_insert(const_cast<iterator>(position), n, value);
		}

		template <class InputIterator, typename std::enable_if<wstl::is_input_iterator<InputIterator>::value, int>::type = 0>
		void insert(const_iterator position, InputIterator first, InputIterator last) {
			WSTL_DEBUG(position >= begin() && position <= end());
			WSTL_DEBUG(!(last < first));
			copy_insert(const_cast<iterator>(position), first, last);
		}

		iterator insert(const_iterator position, std::initializer_list<value_type> il) {
			return insert(position, il.begin(), il.end());
		}

		// erase / clear

		iterator erase(const_iterator position);

		iterator erase(const_iterator first, const_iterator last);

		void clear() {
			erase(begin(), end());
		}

		// resize / reverse

		void resize(size_type new_size, const value_type &value);

		void resize(size_type new_size) {
			resize(new_size, value_type());
		}

		void reverse() {
			wstl::reverse(begin(), end());
		}

		// swap

		void swap(vector &rhs) noexcept;

	private:
		// helper functions

		// initialize / destroy

		void try_init() noexcept;

		void init_space(size_type size, size_type cap);

		void fill_init(size_type n, const value_type &value);

		template <class InputIterator>
		void range_init(InputIterator first, InputIterator last);

		void destroy_and_recover(iterator first, iterator last, size_type n);

		// calculate the growth size
		size_type get_new_cap(size_type add_size);

		// assign

		void fill_assign(size_type n, const value_type &value);

		template <class InputIterator>
		void copy_assign(InputIterator first, InputIterator last, input_iterator_tag);

		template <class ForwardIterator>
		void copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

		// reallocate

		template <class... Args>
		void reallocate_emplace(iterator position, Args &&...args);

		void reallocate_insert(iterator position, const value_type &value);

		void reallocate_insert(iterator position, value_type &&value);

		// insert

		iterator fill_insert(iterator position, size_type n, const value_type &value);

		template <class InputIterator>
		void copy_insert(iterator position, InputIterator first, InputIterator last);

		// shrink_to_fit
		void reinsert(size_type size);
	};

	/******************************************************************************************************/

	// copy assignment
	template <class T, class Alloc>
	vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &rhs) {
		if (this != &rhs) {
			const auto len = rhs.size();
			if (len > capacity()) {
				vector tmp(rhs.begin(), rhs.end());
				swap(tmp);
			} else if (len <= size()) {
				wstl::copy(rhs.begin(), rhs.end(), begin());
				data_allocator::destroy(begin() + len, end());
				end_ = begin() + len;
			} else {
				wstl::copy(rhs.begin(), rhs.begin() + size(), begin());
				wstl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
				end_ = cap_ = begin() + len;
			}
		}
		return *this;
	}

	// move assignment
	template <class T, class Alloc>
	vector<T, Alloc> &vector<T, Alloc>::operator=(vector &&rhs) noexcept {
		if (this != &rhs) {
			destroy_and_recover(begin_, end_, cap_ - begin_);
			begin_ = rhs.begin_;
			end_ = rhs.end_;
			cap_ = rhs.cap_;
			rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
		}
		return *this;
	}

	// reserve
	template <class T, class Alloc>
	void vector<T, Alloc>::reserve(size_type n) {
		if (capacity() < n) {
			THROW_LENGTH_ERROR_IF(n > max_size(), "vector<T> : exceed max_size() in vector::reserve");
			const auto old_size = size();
			auto tmp = data_allocator::allocate(n);
			wstl::uninitialized_move(begin_, end_, tmp);
			destroy_and_recover(begin_, end_, cap_ - begin_);
			begin_ = tmp;
			end_ = begin_ + old_size;
			cap_ = begin_ + n;
		}
	}

	// shrink_to_fit, 放弃多余的容量
	template <class T, class Alloc>
	void vector<T, Alloc>::shrink_to_fit() {
		if (end_ < cap_) {
			reinsert(size());
		}
	}

	// emplace, 在 position 处构造元素
	template <class T, class Alloc>
	template <class... Args>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::emplace(const_iterator position, Args &&...args) {
		WSTL_DEBUG(position >= begin() && position <= end());
		iterator pos = const_cast<iterator>(position);
		if (end_ != cap_ && pos == end_) {
			data_allocator::construct(wstl::address_of(*end_), wstl::forward<Args>(args)...);
			++end_;
		} else if (end_ != cap_) {
			data_allocator::construct(wstl::address_of(*end_), wstl::move(back()));
			++end_;
			wstl::move_backward(pos, end_ - 2, end_ - 1);
			*pos = value_type(wstl::forward<Args>(args)...);
		} else {
			reallocate_emplace(pos, wstl::forward<Args>(args)...);
		}
		return pos;
	}

	// emplace_back, 在末尾构造元素
	template <class T, class Alloc>
	template <class... Args>
	void vector<T, Alloc>::emplace_back(Args &&...args) {
		if (end_ < cap_) {
			data_allocator::construct(wstl::address_of(*end_), wstl::forward<Args>(args)...);
			++end_;
		} else {
			reallocate_emplace(end_, wstl::forward<Args>(args)...);
		}
	}

	// push_back, 在末尾插入元素
	template <class T, class Alloc>
	void vector<T, Alloc>::push_back(const value_type &value) {
		if (end_ < cap_) {
			data_allocator::construct(wstl::address_of(*end_), value);
			++end_;
		} else {
			reallocate_insert(end_, value);
		}
	}

	// pop_back, 删除末尾元素
	template <class T, class Alloc>
	void vector<T, Alloc>::pop_back() {
		WSTL_DEBUG(!empty());
		data_allocator::destroy(end_ - 1);
		--end_;
	}

	// insert, 在 position 处插入元素
	template <class T, class Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::insert(const_iterator position, const value_type &value) {
		WSTL_DEBUG(position >= begin() && position <= end());
		iterator pos = const_cast<iterator>(position);
		if (end_ != cap_ && pos == end_) {
			data_allocator::construct(wstl::address_of(*end_), value);
			++end_;
		} else if (end_ != cap_) {
			data_allocator::construct(wstl::address_of(*end_), wstl::move(back()));
			++end_;
			wstl::move_backward(pos, end_ - 2, end_ - 1);
			*pos = wstl::move(value);
		} else {
			reallocate_insert(pos, value);
		}
		return pos;
	}

	// erase, 删除 position 处的元素
	template <class T, class Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::erase(const_iterator position) {
		WSTL_DEBUG(position >= begin() && position < end());
		iterator pos = const_cast<iterator>(position);
		wstl::move(pos + 1, end_, pos);
		data_allocator::destroy(end_ - 1);
		--end_;
		return pos;
	}

	// erase, 删除 [first, last) 区间的元素
	template <class T, class Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::erase(const_iterator first, const_iterator last) {
		WSTL_DEBUG(first >= begin() && first <= last && last <= end());
		iterator pos = const_cast<iterator>(first);
		if (first != last) {
			wstl::move(const_cast<iterator>(last), end_, pos);
			auto n = static_cast<size_type>(last - first);
			data_allocator::destroy(end_ - n, end_);
			end_ -= n;
		}
		return pos;
	}

	// resize, 修改容器大小
	template <class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size, const value_type &value) {
		if (new_size < size()) {
			erase(begin() + new_size, end());
		} else {
			insert(end(), new_size - size(), value);
		}
	}

	// swap, 交换两个 vector 容器
	template <class T, class Alloc>
	void vector<T, Alloc>::swap(vector &rhs) noexcept {
		if (this != &rhs) {
			wstl::swap(begin_, rhs.begin_);
			wstl::swap(end_, rhs.end_);
			wstl::swap(cap_, rhs.cap_);
		}
	}

	//******************************************************************** */
	// helper function

	// try_init, 初始化, 无异常抛出
	template <class T, class Alloc>
	void vector<T, Alloc>::try_init() noexcept {
		try {
			begin_ = data_allocator::allocate(16);
			end_ = begin_;
			cap_ = begin_ + 16;
		} catch (...) {
			begin_ = nullptr;
			end_ = nullptr;
			cap_ = nullptr;
		}
	}

	// init_space, 初始化空间
	template <class T, class Alloc>
	void vector<T, Alloc>::init_space(size_type size, size_type cap) {
		try {
			begin_ = data_allocator::allocate(cap);
			end_ = begin_ + size;
			cap_ = begin_ + cap;
		} catch (...) {
			begin_ = nullptr;
			end_ = nullptr;
			cap_ = nullptr;
			throw;
		}
	}

	// fill_init, 填充初始化
	template <class T, class Alloc>
	void vector<T, Alloc>::fill_init(size_type n, const value_type &value) {
		const auto new_size = wstl::max(static_cast<size_type>(16), n);
		init_space(n, new_size);
		wstl::uninitialized_fill_n(begin_, n, value);
	}

	// range_init, 区间初始化
	template <class T, class Alloc>
	template <class InputIterator>
	void vector<T, Alloc>::range_init(InputIterator first, InputIterator last) {
		const auto len = static_cast<size_type>(wstl::distance(first, last));
		const auto new_size = wstl::max(static_cast<size_type>(16), len);
		init_space(len, new_size);
		wstl::uninitialized_copy(first, last, begin_);
	}

	// destroy_and_recover, 销毋并回收空间
	template <class T, class Alloc>
	void vector<T, Alloc>::destroy_and_recover(iterator first, iterator last, size_type n) {
		data_allocator::destroy(first, last);
		data_allocator::deallocate(first, n);
	}

	// get_new_cap, 计算新的容量
	template <class T, class Alloc>
	typename vector<T, Alloc>::size_type vector<T, Alloc>::get_new_cap(size_type add_size) {
		const auto old_size = size();
		THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, "vector<T> : size too big in vector<T>::get_new_cap");
		if (old_size > max_size() - old_size / 2) {
			return old_size + add_size > max_size() - 16 ? old_size + add_size : old_size + add_size + 16;
		}
		const auto new_size = old_size == 0 ? wstl::max(static_cast<size_type>(16), add_size) : wstl::max(old_size + old_size / 2, old_size + add_size);
		return new_size;
	}

	// fill_assign, 填充赋值
	template <class T, class Alloc>
	void vector<T, Alloc>::fill_assign(size_type n, const value_type &value) {
		if (n > capacity()) {
			vector tmp(n, value);
			swap(tmp);
		} else if (n > size()) {
			wstl::fill(begin(), end(), value);
			end_ = wstl::uninitialized_fill_n(end_, n - size(), value);
		} else {
			erase(wstl::fill_n(begin(), n, value), end_);
		}
	}

	// copy_assign, 拷贝赋值
	template <class T, class Alloc>
	template <class InputIterator>
	void vector<T, Alloc>::copy_assign(InputIterator first, InputIterator last, input_iterator_tag) {
		auto cur = begin_;
		for (; first != last && cur != end_; ++first, ++cur) {
			*cur = *first;
		}
		if (first == last) {
			erase(cur, end_);
		} else {
			insert(end_, first, last);
		}
	}

	template <class T, class Alloc>
	template <class ForwardIterator>
	void vector<T, Alloc>::copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
		const auto len = wstl::distance(first, last);
		if (len > capacity()) {
			vector tmp(first, last);
			swap(tmp);
		} else if (static_cast<size_type>(len) <= size()) {
			auto new_end = wstl::copy(first, last, begin());
			data_allocator::destroy(new_end, end_);
			end_ = new_end;
		} else {
			auto mid = first;
			wstl::advance(mid, size());
			wstl::copy(first, mid, begin());
			auto new_end = wstl::uninitialized_copy(mid, last, end_);
			end_ = new_end;
		}
	}

	// reallocate_emplace, 重新分配空间并在 position 处构造元素
	template <class T, class Alloc>
	template <class... Args>
	void vector<T, Alloc>::reallocate_emplace(iterator position, Args &&...args) {
		const auto new_size = get_new_cap(1);
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try {
			new_end = wstl::uninitialized_move(begin_, position, new_begin);
			data_allocator::construct(wstl::address_of(*new_end), wstl::forward<Args>(args)...);
			++new_end;
			new_end = wstl::uninitialized_move(position, end_, new_end);
		} catch (...) {
			data_allocator::destroy(new_begin, new_end);
			data_allocator::deallocate(new_begin, new_size);
			throw;
		}
		destroy_and_recover(begin_, end_, cap_ - begin_);
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}

	// reallocate_insert, 重新分配空间并在 position 处插入元素
	template <class T, class Alloc>
	void vector<T, Alloc>::reallocate_insert(iterator position, const value_type &value) {
		const auto new_size = get_new_cap(1);
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try {
			new_end = wstl::uninitialized_move(begin_, position, new_begin);
			data_allocator::construct(wstl::address_of(*new_end), value);
			++new_end;
			new_end = wstl::uninitialized_move(position, end_, new_end);
		} catch (...) {
			data_allocator::destroy(new_begin, new_end);
			data_allocator::deallocate(new_begin, new_size);
			throw;
		}
		destroy_and_recover(begin_, end_, cap_ - begin_);
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}

	// reallocate_insert, 重新分配空间并在 position 处插入元素
	template <class T, class Alloc>
	void vector<T, Alloc>::reallocate_insert(iterator position, value_type &&value) {
		const auto new_size = get_new_cap(1);
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try {
			new_end = wstl::uninitialized_move(begin_, position, new_begin);
			data_allocator::construct(wstl::address_of(*new_end), wstl::move(value));
			++new_end;
			new_end = wstl::uninitialized_move(position, end_, new_end);
		} catch (...) {
			data_allocator::destroy(new_begin, new_end);
			data_allocator::deallocate(new_begin, new_size);
			throw;
		}
		destroy_and_recover(begin_, end_, cap_ - begin_);
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}

	// fill_insert, 填充插入
	template <class T, class Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::fill_insert(iterator position, size_type n, const value_type &value) {
		if (n == 0) {
			return const_cast<iterator>(position);
		}
		const auto position_idx = position - begin_;
		const value_type value_copy = value;
		if (static_cast<size_type>(cap_ - end_) >= n) {
			const size_type after_elems = end_ - position;
			auto old_end = end_;
			if (after_elems > n) {
				wstl::uninitialized_copy(end_ - n, end_, end_);
				end_ += n;
				wstl::move_backward(position, old_end - n, old_end);
				wstl::uninitialized_fill_n(position, n, value_copy);
			} else {
				end_ = wstl::uninitialized_fill_n(end_, n - after_elems, value_copy);
				end_ = wstl::uninitialized_move(position, old_end, end_);
				wstl::uninitialized_fill_n(position, after_elems, value_copy);
			}
		} else {
			const auto new_size = get_new_cap(n);
			auto new_begin = data_allocator::allocate(new_size);
			auto new_end = new_begin;
			auto cur = begin_;
			try {
				new_end = wstl::uninitialized_move(begin_, position, new_begin);
				new_end = wstl::uninitialized_fill_n(new_end, n, value);
				new_end = wstl::uninitialized_move(position, end_, new_end);
			} catch (...) {
				destroy_and_recover(new_begin, new_end, new_size);
				throw;
			}
			destroy_and_recover(begin_, end_, cap_ - begin_);
			begin_ = new_begin;
			end_ = new_end;
			cap_ = begin_ + new_size;
		}
		return begin_ + position_idx;
	}

	// copy_insert, 拷贝插入
	template <class T, class Alloc>
	template <class InputIterator>
	void vector<T, Alloc>::copy_insert(iterator position, InputIterator first, InputIterator last) {
		if (first == last) {
			return;
		}
		const auto n = wstl::distance(first, last);
		if (static_cast<size_type>(cap_ - end_) >= n) {
			const auto after_elems = end_ - position;
			auto old_end = end_;
			if (after_elems > n) {
				end_ = wstl::uninitialized_copy(end_ - n, end_, end_);
				wstl::move_backward(position, old_end - n, old_end);
				wstl::uninitialized_copy(first, last, position);
			} else {
				auto mid = first;
				wstl::advance(mid, after_elems);
				end_ = wstl::uninitialized_copy(mid, last, end_);
				end_ = wstl::uninitialized_move(position, old_end, end_);
				wstl::uninitialized_copy(first, mid, position);
			}
		} else {
			const auto new_size = get_new_cap(n);
			auto new_begin = data_allocator::allocate(new_size);
			auto new_end = new_begin;
			try {
				new_end = wstl::uninitialized_move(begin_, position, new_begin);
				new_end = wstl::uninitialized_copy(first, last, new_end);
				new_end = wstl::uninitialized_move(position, end_, new_end);
			} catch (...) {
				destroy_and_recover(new_begin, new_end, new_size);
				throw;
			}
			destroy_and_recover(begin_, end_, cap_ - begin_);
			begin_ = new_begin;
			end_ = new_end;
			cap_ = begin_ + new_size;
		}
	}

	// reinsert, 重新插入元素
	template <class T, class Alloc>
	void vector<T, Alloc>::reinsert(size_type size) {
		auto new_begin = data_allocator::allocate(size);
		try {
			wstl::uninitialized_move(begin_, end_, new_begin);
		} catch (...) {
			data_allocator::deallocate(new_begin, size);
			throw;
		}
		destroy_and_recover(begin_, end_, cap_ - begin_);
		begin_ = new_begin;
		end_ = begin_ + size;
		cap_ = end_;
	}

	/******************************************************************************************************/
	// 重载比较操作符

	template <class T, class Alloc>
	bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return lhs.size() == rhs.size() && wstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T, class Alloc>
	bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Alloc>
	bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return wstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Alloc>
	bool operator<=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return !(rhs < lhs);
	}

	template <class T, class Alloc>
	bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return rhs < lhs;
	}

	template <class T, class Alloc>
	bool operator>=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
		return !(lhs < rhs);
	}

	// 重载 swap
	template <class T, class Alloc>
	void swap(vector<T, Alloc> &lhs, vector<T, Alloc> &rhs) noexcept {
		lhs.swap(rhs);
	}

} // namespace wstl

#endif // WSTL_VECTOR_H