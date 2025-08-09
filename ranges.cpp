#include "ranges.hpp"
#include <limits>
#include <type_traits>
#include <algorithm>
std::ostream &operator<<(std::ostream & o, range r) {
	return o << "[" << r.first << ", " << r.second << "]";
}
value safe_mul(value a, value b) {
	static_assert(std::is_same<value, int>::value, "Update function to work with current value type");
	value r;
	if (__builtin_mul_overflow(a, b, &r)) {
		if ((a ^ b) >> 31) return std::numeric_limits<value>::min();
		return std::numeric_limits<value>::max();
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
		if (a >= 0) return std::numeric_limits<value>::max();
		return std::numeric_limits<value>::min();
	}
	return r;
}
value safe_div(value a, value b) {
	static_assert(std::is_same<value, int>::value, "Update function to work with current value type");
	if (a == std::numeric_limits<value>::min() && b == -1) return std::numeric_limits<value>::max();
	return a / b;
}
range singleton(value x) {
	return {x, x};
}
range set_union(range a, range b) {
	return {std::min(a.first, b.first), std::max(a.second, b.second)};
}
range operator+(range a, range b) {
	return {safe_add(a.first, b.first), safe_add(a.second, b.second)};
}
range operator-(range a, range b) {
	return {safe_sub(a.first, b.second), safe_sub(a.second, b.first)};
}
range operator*(range a, range b) {
	value x[] = {safe_mul(a.first, b.first), safe_mul(a.second, b.first), safe_mul(a.first, b.second), safe_mul(a.second, b.second)}; //TODO: can this be made more efficient (?)
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator/(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero(); //TODO: return some sort of empty interval (?)
	if (b.first < 0 && b.second > 0) {
		if (a.first == std::numeric_limits<value>::min()) {
			return {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
		}
		value t =  std::max(abs(a.first), abs(a.second));
		return {-t, t};
	}
	if (b.first == 0) b.first = 1;
	if (b.second == 0) b.second = -1;
	value x[] = {safe_div(a.first, b.first), safe_div(a.second, b.first), safe_div(a.first, b.second), safe_div(a.second, b.second)};
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator%(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero();
	range b2 = {b.first <= 0 && b.second >= 0 ? 1 : std::min(std::abs(b.first), std::abs(b.second)), std::max(std::abs(b.first), std::abs(b.second))}; //Watch out for abs(INT_MIN)
	if (a.first / b2.second == a.second / b2.first) {
		value c = a.first / b2.second;
		return {a.first - c * b2.second, a.second - c * b2.first};
	}
	return {0, b2.second - 1};
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
range is_nonzero(range a) { // Cast to bool (?)
	return {!can_zero(a), can_nonzero(a)};
}
range less_than(range a, range b) {
	return {a.second < b.first, a.first < b.second};
}
range less_equal(range a, range b) {
	return {a.second <= b.first, a.first <= b.second};
}
range greater_than(range a, range b) {
	return less_than(b, a);
}
range greater_equal(range a, range b) {
	return less_equal(b, a);
}
range equal(range a, range b) {
	return {a.first == a.second && a.first == b.first && a.first == b.second, a.first <= b.second && b.first <= a.second};
}
range not_equal(range a, range b) {
	return {a.first > b.second || b.first > a.second, a.first != a.second || a.first != b.first || a.first != b.second};	
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
range ternary(range a, range b, range c) { //TODO: make it so a is required to be a boolean and use cast to bool (if needed) when compiling an expression
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
range logical_not(range a) {
	return {!can_nonzero(a), can_zero(a)};
}
range take_kth(const std::vector<range> &ranges, size_t k) {
	assert(k < ranges.size());
	std::vector<value> begins(ranges.size()), ends(ranges.size());
	for (size_t i = 0; i < ranges.size(); ++i) {
		auto [b, e] = ranges[i];
		begins[i] = b;
		ends[i] = e;
	}
	std::nth_element(begins.begin(), begins.begin() + k, begins.end());
	std::nth_element(ends.begin(), ends.begin() + k, ends.end());
	return {begins[k], ends[k]};
}
std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_min(range a, range b) {
	if (a.second <= b.first) {
		return {std::nullopt, std::numeric_limits<value>::max()};
	}
	if (b.second <= a.first) {
		return {std::numeric_limits<value>::max(), std::nullopt};
	}
	return {std::nullopt, std::nullopt};
}
std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_max(range a, range b) {
	if (a.second <= b.first) {
		return {std::numeric_limits<value>::min(), std::nullopt};
	}
	if (b.second <= a.first) {
		return {std::nullopt, std::numeric_limits<value>::min()};
	}
	return {std::nullopt, std::nullopt};
}
std::tuple<std::optional<value>, std::optional<value>, std::optional<value> > replace_irrelevant_ternary(range a, range, range) {
	if (!can_zero(a)) {
		return {std::nullopt, std::nullopt, 0};
	}
	if (!can_nonzero(a)) {
		return {std::nullopt, 0, std::nullopt};
	}
	return {std::nullopt, std::nullopt, std::nullopt};
}
