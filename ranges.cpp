#include "ranges.hpp"
#include <limits>
#include <type_traits>
#include <algorithm>
value safe_mul(value a, value b) {
	static_assert(std::is_same<value, int>::value, "Update function to work with current value type");
	value r;
	if (__builtin_mul_overflow(a, b, &r)) {
		if ((a ^ b) >> 31) return std::numeric_limits<value>::max();
		return std::numeric_limits<value>::min();
	}
	return r;
}
value safe_add(value a, value b) {
	static_assert(std::is_same<value, int>::value, "Update function to work with current value type");
	value r;
	if (__builtin_add_overflow(a, b, &r)) {
		if (a > 0) return std::numeric_limits<value>::max();
		return std::numeric_limits<value>::min();
	}
	return r;
}
value safe_sub(value a, value b) {
	static_assert(std::is_same<value, int>::value, "Update function to work with current value type");
	value r;
	if (__builtin_sub_overflow(a, b, &r)) {
		if (a > 0) return std::numeric_limits<value>::max();
		return std::numeric_limits<value>::min();
	}
	return r;
}
range singleton(value x) {
	return {x, x};
}
range set_union(range a, range b) {
	return {std::min(a.first, b.first), std::max(a.second, b.second)};
}
range operator+(range a, range b) {
	return {a.first + b.first, a.second + b.second};
}
range operator-(range a, range b) {
	return {a.first - b.second, a.second - b.first};
}
range operator*(range a, range b) {
	value x[] = {a.first * b.first, a.second * b.first, a.first * b.second, a.second * b.second};
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator/(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero(); //TODO: return some sort of empty interval
	if (b.first < 0 && b.second > 0) {
		int t =  std::max(abs(a.first), abs(a.second));
		return {-t, t};
	}
	if (b.first == 0) b.first = 1;
	if (b.second == 0) b.second = 1;
	value x[] = {a.first / b.first, a.second / b.first, a.first / b.second, a.second / b.second};
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator%(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero();
	return {b.first <= 0 ? 0 : b.first, b.second >= 0 ? b.second - 1 : 0};
	//TODO Improve to include a
}
range take_max(range a, range b) {
	return {std::max(a.first, b.first), std::max(a.second, b.second)};
}
range take_min(range a, range b) {
	return {std::min(a.first, b.first), std::min(a.second, b.second)};
}
bool can_zero(range a) {
	return a.first <= 0 && a.second >= 0;
}
bool can_nonzero(range a) {
	return a != range{0, 0};
}
range is_zero(range a) {
	return {!can_nonzero(a), can_zero(a)};
}
range is_nonzero(range a) {
	return {!can_zero(a), can_nonzero(a)};
}
range is_positive(range a) {
	if (a.first > 0) return {1, 1};
	if (a.second <= 0) return {0, 0};
	return {0, 1};
}
range is_nonnegative(range a) {
	if (a.first >= 0) return {1, 1};
	if (a.second < 0) return {0, 0};
	return {0, 1};
}
range ternary(range a, range b, range c) {
	range first = is_nonzero(a);
	if (first == range{1, 1}) return b;
	if (first == range{0, 0}) return c;
	return set_union(b, c);
}
range logical_and(range a, range b) {
	return {!(can_zero(a) || can_zero(b)), can_nonzero(a) && can_nonzero(b)};
}
range logical_or(range a, range b) {
	return {!(can_zero(a) && can_zero(b)), can_nonzero(a) || can_nonzero(b)};
}
