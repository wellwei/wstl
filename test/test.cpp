#include <iostream>

#include "vector.h"

int main() {
	wstl::vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);

	for (auto it = vec.begin(); it != vec.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	vec.pop_back();
	vec.pop_back();

	for (auto it = vec.begin(); it != vec.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	wstl::vector<int> vec2 = vec;
	vec2.push_back(5);
	vec2.push_back(6);

	for (auto it = vec2.begin(); it != vec2.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	wstl::vector<int> vec3(std::move(vec2));

	for (auto it = vec3.begin(); it != vec3.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	wstl::vector<int> vec4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

	for (auto it = vec4.begin(); it != vec4.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	wstl::vector<int> vec5(vec4.begin(), vec4.end());

	for (auto i : vec5) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	std::cout << "vec5 size: " << vec5.size() << std::endl;
	std::cout << "vec5 capacity: " << vec5.capacity() << std::endl;

	vec5.reserve(100);
	std::cout << "vec5 size: " << vec5.size() << std::endl;
	std::cout << "vec5 capacity: " << vec5.capacity() << std::endl;

	vec5.insert(vec5.begin() + 5, 100);
	vec5.insert(vec5.begin() + 5, 2, 200);
	vec5.insert(vec5.begin() + 5, vec4.begin(), vec4.begin() + 5);

	for (auto i : vec5) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	vec5.erase(vec5.begin() + 5);
	vec5.erase(vec5.begin() + 5, vec5.begin() + 10);

	for (auto i : vec5) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
}