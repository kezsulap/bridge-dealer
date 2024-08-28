#include "test_utils.hpp"
#include <cstdlib>
void test_any_fails() {
	if (any_failed) exit(1);
}
void assert_range_subset_impl(range a, range b, range c, std::string a_name, std::string b_name, std::string c_name) {
	if (a.first >= b.first && b.first >= c.first && a.second <= b.second && b.second <= c.second) {
		std::cout << GREEN << "PASSED: " << a_name << " <= " << b_name << " <= " << c_name << CLEAR_COLOURS << std::endl;
	}
	else {
		std::cout << RED << "ERROR: " << a_name << " = " << a << " <= " << b_name << " = " << b << " <= " << c_name << " = " << c << " not satisfied" << CLEAR_COLOURS << std::endl;
		any_failed = true;
	}
}
static int _ = (atexit(test_any_fails), 0);
