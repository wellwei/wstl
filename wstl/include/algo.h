#ifndef WSTL_ALGO_H
#define WSTL_ALGO_H

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"

namespace wstl {
	// reverse, 反转区间 [first, last) 中的元素
	template <class BidirectionalIterator>
	void reverse(BidirectionalIterator first, BidirectionalIterator last) {
		while (first != last && first != --last) {
			wstl::iter_swap(first++, last);
		}
	}
}

#endif // WSTL_ALGO_H