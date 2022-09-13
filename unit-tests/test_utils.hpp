#ifndef TEST_UTILS_H
#define TEST_UTILS_H1
#define ASSERT_EQUAL(a, b) assert_equal_impl(a, b, #a, #b)
#include <iostream>
#include <string>
extern bool any_failed;
template <class c> void assert_equal_impl(c a, c b, std::string a_name, std::string b_name) {
	if (a != b) {
		std::cout << "ERROR: " << a_name << " = " << a << " not equal to " << b_name << " = " << b << std::endl;
		any_failed = true;
	}
}
#endif
