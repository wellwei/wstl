#ifndef WSTL_MEMORY_H
#define WSTL_MEMORY_H

// 这个头文件负责更高级的动态内存管理， 包括基本函数，空间配置器，未初始化的储存空间管理，以及各种智能指针

#include <climits>
#include <cstddef>
#include <cstdlib>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace wstl {

	// 获取对象地址
	template <class T>
	constexpr T *address_of(T &value) noexcept {
		return &value;
	}

	// 获取 / 释放临时缓冲区
}

#endif // WSTL_MEMORY_H