#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#define ASSERT_EQUAL(a, b) assert_equal_impl(a, b, #a, #b)
#define ASSERT_THROWS(e, f) assert_throws_impl<e>([]{f;}, #f, #e)
#include <iostream>
#include <string>
inline bool any_failed = false;
static const std::string GREEN = "\u001b[32m", RED = "\u001b[31m", CLEAR_COLOURS = "\u001b[0m";
template <class c> void assert_equal_impl(c a, c b, std::string a_name, std::string b_name) {
	if (a != b) {
		std::cout << RED << "ERROR: " << a_name << " = " << a << " not equal to " << b_name << " = " << b << CLEAR_COLOURS << std::endl;
		any_failed = true;
	}
	else {
		std::cout << GREEN << "PASSED: " << a_name << " = " << b_name << CLEAR_COLOURS << std::endl;
	}
}
template <class Exception, class F> void assert_throws_impl(F f, std::string command, std::string exception_name) {
	try {
		f();
	}
	catch(Exception) {
		std::cout << GREEN << "PASSED: " << command << " raised " << exception_name << CLEAR_COLOURS << std::endl;
		return;
	}
	std::cout << RED << "ERROR: " << command << " didn't raise anything" << CLEAR_COLOURS << std::endl;
	any_failed = true;
}
#endif
