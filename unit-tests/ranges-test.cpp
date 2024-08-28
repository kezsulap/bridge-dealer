#include "../ranges.hpp"
#include "test_utils.hpp"
#include <climits>
#define ASSERT_RANGE_SUBSET(a, b, c) assert_range_subset_impl(a, b, c, #a, #b, #c)
#include "../ranges.hpp"
void assert_range_subset_impl(range a, range b, range c, std::string a_name, std::string b_name, std::string c_name) {
	if (a.first >= b.first && b.first >= c.first && a.second <= b.second && b.second <= c.second) {
		std::cout << GREEN << "PASSED: " << a_name << " <= " << b_name << " <= " << c_name << CLEAR_COLOURS << std::endl;
	}
	else {
		std::cout << RED << "ERROR: " << a_name << " = " << a << " <= " << b_name << " = " << b << " <= " << c_name << " = " << c << " not satisfied" << CLEAR_COLOURS << std::endl;
		any_failed = true;
	}
}
int main() {
	ASSERT_EQUAL(range(2, 2), singleton(2));

	std::cout << "ADDING RANGES:" << std::endl;
	ASSERT_EQUAL(range(2, 2) + range(3, 3), singleton(5));
	ASSERT_EQUAL(range(-1, 1) + range(-1, 1), range(-2, 2));
	ASSERT_EQUAL(range(-1, 1) + range(-1, 1), range(-2, 2));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX) + range(-1, 1), range(INT_MIN, INT_MAX));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX) + range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX));
	ASSERT_EQUAL(range(2'000'000'000, 2'000'000'000) + range(2'000'000'000, 2'000'000'000), range(INT_MAX, INT_MAX));
	ASSERT_EQUAL(range(2'000'000'000, 2'000'000'000) + range(100'000'000, 2'000'000'000), range(2'100'000'000, INT_MAX));
	ASSERT_EQUAL(range(-2'000'000'000, -2'000'000'000) + range(-2'000'000'000, -2'000'000'000), range(INT_MIN, INT_MIN));
	ASSERT_EQUAL(range(-2'000'000'000, -2'000'000'000) + range(-2'000'000'000, -100'000'000), range(INT_MIN, -2'100'000'000));
	ASSERT_EQUAL(range(-2'000'000'000, 2'000'000'000) + range(-2'000'000'000, 2'000'000'000), range(INT_MIN, INT_MAX));

	std::cout << "SUBTRACTING RANGES:" << std::endl;
	ASSERT_EQUAL(singleton(21) - singleton(37), singleton(-16));
	ASSERT_EQUAL(range(21, 37) - range(20, 23), range(-2, 17));
	ASSERT_EQUAL(singleton(0) - singleton(INT_MIN), singleton(INT_MAX));
	ASSERT_EQUAL(singleton(0) - singleton(INT_MAX), singleton(-INT_MAX));
	ASSERT_EQUAL(singleton(2'000'000'000) - singleton(-2'000'000'000), singleton(INT_MAX));
	ASSERT_EQUAL(range(-2'000'000'000, 2'000'000'000) - range(-2'000'000'000, 2'000'000'000), range(INT_MIN, INT_MAX));

	std::cout << "MULTIPLYING RANGES:" << std::endl;
	ASSERT_EQUAL(range(21, 37) * singleton(1), range(21, 37));
	ASSERT_EQUAL(range(21, 37) * singleton(-1), range(-37, -21));
	ASSERT_EQUAL(singleton(1) * range(21, 37), range(21, 37));
	ASSERT_EQUAL(singleton(-1) * range(21, 37), range(-37, -21));
	ASSERT_EQUAL(range(-1, 1) * range(-21, 37), range(-37, 37));
	ASSERT_EQUAL(range(-37, 21) * range(-1, 1), range(-37, 37));
	ASSERT_EQUAL(range(-21, 37) * range(-1, 1), range(-37, 37));
	ASSERT_EQUAL(singleton(0) * range(INT_MIN, INT_MAX), singleton(0));
	ASSERT_EQUAL(range(-1'000'000, 1'000'000) * range(-1'000'000, 1'000'000), range(INT_MIN, INT_MAX));
	ASSERT_EQUAL(range(1'000, 1'000'000) * range(2'137, 2'137'137), range(2'137'000, INT_MAX));
	ASSERT_EQUAL(range(-1'000'000, -1'000) * range(-2'137'137, -2'137), range(2'137'000, INT_MAX));
	ASSERT_EQUAL(range(1'000, 1'000'000) * range(-2'137'137, -2'137), range(INT_MIN, -2'137'000));
	ASSERT_EQUAL(range(-1'000'000, -1'000) * range(2'137, 2'137'137), range(INT_MIN, -2'137'000));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX) * range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX));

	std::cout << "DIVIDING RANGES:" << std::endl;
	ASSERT_EQUAL(range(-21, 37) / singleton(10), range(-2, 3));
	ASSERT_EQUAL(range(-12345, 123456) / range(-3, 3), range(-123456, 123456));
	ASSERT_EQUAL(range(-123456, 12345) / range(-3, 3), range(-123456, 123456));
	ASSERT_EQUAL(range(200, 500) / range(1, 10), range(20, 500));
	ASSERT_EQUAL(range(200, 500) / range(0, 10), range(20, 500));
	ASSERT_EQUAL(range(200, 500) / range(-10, -1), range(-500, -20));
	ASSERT_EQUAL(range(200, 500) / range(-10, 0), range(-500, -20));
	ASSERT_EQUAL(range(-100, 150) / range(2, 5), range(-50, 75));
	ASSERT_EQUAL(range(-100, 150) / range(-2, -5), range(-75, 50));
	ASSERT_EQUAL(singleton(0) / range(INT_MIN, INT_MAX), singleton(0));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX) / singleton(-1), range(-INT_MAX, INT_MAX));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX) / range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX));
	ASSERT_EQUAL(range(INT_MIN, 0) / range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX));
	ASSERT_EQUAL(range(INT_MIN, 0) / range(INT_MIN, 0), range(0, INT_MAX));
	ASSERT_THROWS(division_by_zero, range(21, 37) / range(0, 0));
	ASSERT_THROWS(division_by_zero, range(0, 0) / range(0, 0));
	ASSERT_THROWS(division_by_zero, range(-37, -21) / range(0, 0));
	ASSERT_THROWS(division_by_zero, range(INT_MIN, INT_MIN) / range(0, 0));

	std::cout << "MODULOING RANGES:" << std::endl;
	ASSERT_EQUAL(range(0, 6), range(0, 100) % range(7, 7));
	ASSERT_EQUAL(range(0, 6), range(0, 100) % range(0, 7));
	ASSERT_EQUAL(range(1, 7), range(21, 27) % range(10, 10));
	ASSERT_EQUAL(range(0, 36), range(0, 100) % range(-21, 37));
	ASSERT_EQUAL(range(0, 36), range(0, 100) % range(-37, 21));
	ASSERT_EQUAL(range(1, 4), range(13, 14) % range(5, 6));
	ASSERT_EQUAL(range(1, 4), range(13, 14) % range(-6, -5));
	ASSERT_RANGE_SUBSET(range(40, 43), range(4000, 4003) % range(44, 45), range(0, 44));
	ASSERT_THROWS(division_by_zero, range(21, 37) % range(0, 0));
	ASSERT_THROWS(division_by_zero, range(0, 0) % range(0, 0));
	ASSERT_THROWS(division_by_zero, range(-37, -21) % range(0, 0));
	ASSERT_THROWS(division_by_zero, range(INT_MIN, INT_MIN) / range(0, 0));

	std::cout << "TAKE_MAX:" << std::endl;
	ASSERT_EQUAL(range(0, 0), take_max(range(0, 0), range(0, 0)));
	ASSERT_EQUAL(range(21, 37), take_max(range(21, 37), range(-14, -10)));
	ASSERT_EQUAL(range(20, 40), take_max(range(10, 40), range(20, 30)));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX), take_max(range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX)));

	std::cout << "TAKE_MIN:" << std::endl;
	ASSERT_EQUAL(range(0, 0), take_min(range(0, 0), range(0, 0)));
	ASSERT_EQUAL(range(-14, -10), take_min(range(21, 37), range(-14, -10)));
	ASSERT_EQUAL(range(10, 30), take_min(range(10, 40), range(20, 30)));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX), take_min(range(INT_MIN, INT_MAX), range(INT_MIN, INT_MAX)));

	std::cout << "TERNARY" << std::endl;
	ASSERT_EQUAL(range(20, 21), ternary(range(0, 0), range(30, 31), range(20, 21)));
	ASSERT_EQUAL(range(25, 26), ternary(range(0, 0), range(20, 31), range(25, 26)));
	ASSERT_EQUAL(range(0, 100), ternary(range(0, 0), range(45, 46), range(0, 100)));
	ASSERT_EQUAL(range(INT_MAX, INT_MAX), ternary(range(0, 0), range(INT_MIN, INT_MIN), range(INT_MAX, INT_MAX)));
	ASSERT_EQUAL(range(INT_MIN, INT_MIN), ternary(range(0, 0), range(INT_MAX, INT_MAX), range(INT_MIN, INT_MIN)));

	ASSERT_EQUAL(range(20, 31), ternary(range(0, 1), range(30, 31), range(20, 21)));
	ASSERT_EQUAL(range(20, 31), ternary(range(0, 1), range(20, 31), range(25, 26)));
	ASSERT_EQUAL(range(0, 100), ternary(range(0, 1), range(45, 46), range(0, 100)));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX), ternary(range(0, 1), range(INT_MIN, INT_MIN), range(INT_MAX, INT_MAX)));
	ASSERT_EQUAL(range(INT_MIN, INT_MAX), ternary(range(0, 1), range(INT_MAX, INT_MAX), range(INT_MIN, INT_MIN)));

	ASSERT_EQUAL(range(30, 31), ternary(range(1, 2), range(30, 31), range(20, 21)));
	ASSERT_EQUAL(range(20, 31), ternary(range(3, 4), range(20, 31), range(25, 26)));
	ASSERT_EQUAL(range(45, 46), ternary(range(1, INT_MAX), range(45, 46), range(0, 100)));
	ASSERT_EQUAL(range(INT_MIN, INT_MIN), ternary(range(INT_MIN, -1), range(INT_MIN, INT_MIN), range(INT_MAX, INT_MAX)));
	ASSERT_EQUAL(range(INT_MAX, INT_MAX), ternary(range(-1, -1), range(INT_MAX, INT_MAX), range(INT_MIN, INT_MIN)));

	ASSERT_EQUAL(range(0, 0), ternary(range(INT_MIN, -1), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), ternary(range(-1, 0), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), ternary(range(0, 1), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), ternary(range(-1, 1), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), ternary(range(INT_MIN, INT_MAX), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(1, 1), ternary(range(0, 0), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 0), ternary(range(INT_MIN, INT_MIN), range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 0), ternary(range(INT_MAX, INT_MAX), range(0, 0), range(1, 1)));

	std::cout << "LOGICAL AND:" << std::endl;
	ASSERT_EQUAL(range(0, 0), logical_and(range(0, 0), range(0, 0)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(0, 0), range(0, 1)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(0, 1), range(0, 0)));
	ASSERT_EQUAL(range(0, 1), logical_and(range(0, 1), range(0, 1)));
	ASSERT_EQUAL(range(0, 1), logical_and(range(0, 1), range(1, 1)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(1, 1), range(0, 0)));
	ASSERT_EQUAL(range(0, 1), logical_and(range(1, 1), range(0, 1)));
	ASSERT_EQUAL(range(1, 1), logical_and(range(1, 1), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), logical_and(range(1, 1), range(INT_MIN, INT_MAX)));
	ASSERT_EQUAL(range(0, 1), logical_and(range(INT_MIN, INT_MAX), range(1, 1)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(0, 0), range(INT_MIN, INT_MAX)));
	ASSERT_EQUAL(range(0, 0), logical_and(range(INT_MIN, INT_MAX), range(0, 0)));


	std::cout << "LOGICAL OR:" << std::endl;
	ASSERT_EQUAL(range(0, 0), logical_or(range(0, 0), range(0, 0)));
	ASSERT_EQUAL(range(0, 1), logical_or(range(0, 0), range(0, 1)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(0, 0), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), logical_or(range(0, 1), range(0, 0)));
	ASSERT_EQUAL(range(0, 1), logical_or(range(0, 1), range(0, 1)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(0, 1), range(1, 1)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(1, 1), range(0, 0)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(1, 1), range(0, 1)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(1, 1), range(1, 1)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(1, 1), range(INT_MIN, INT_MAX)));
	ASSERT_EQUAL(range(1, 1), logical_or(range(INT_MIN, INT_MAX), range(1, 1)));
	ASSERT_EQUAL(range(0, 1), logical_or(range(0, 0), range(INT_MIN, INT_MAX)));
	ASSERT_EQUAL(range(0, 1), logical_or(range(INT_MIN, INT_MAX), range(0, 0)));

	std::cout << "LOGICAL NOT:" << std::endl;
	ASSERT_EQUAL(range(1, 1), logical_not(range(0, 0)));
	ASSERT_EQUAL(range(0, 1), logical_not(range(0, 1)));
	ASSERT_EQUAL(range(0, 0), logical_not(range(1, 1)));
	ASSERT_EQUAL(range(0, 0), logical_not(range(-1, -1)));
	ASSERT_EQUAL(range(0, 1), logical_not(range(-1, 1)));
	ASSERT_EQUAL(range(0, 1), logical_not(range(INT_MIN, INT_MAX)));
	ASSERT_EQUAL(range(0, 0), logical_not(range(INT_MIN, -1)));
	ASSERT_EQUAL(range(0, 0), logical_not(range(1, INT_MAX)));
}
