#include <iostream>
#include <vector>

#include "iterator.h"

int main() {

	// 使用指针作为迭代器
	int arr[] = {1, 2, 3, 4, 5};
	int *ptr = arr;

	std::cout << "\nPointer as Iterator:" << std::endl;
	std::cout << "Category: " << typeid(typename wstl::iterator_traits<decltype(ptr)>::iterator_category).name() << std::endl;
	std::cout << "Value Type: " << typeid(typename wstl::iterator_traits<decltype(ptr)>::value_type).name() << std::endl;


	return 0;
}
