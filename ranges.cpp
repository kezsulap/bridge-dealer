#include "ranges.hpp"
#include <limits>
#include <type_traits>
#include <algorithm>
std::ostream &operator<<(std::ostream & o, range r) {
	return o << "[" << r.min << ", " << r.max << "]";
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
	return {std::min(a.min, b.min), std::max(a.max, b.max)};
}
range operator+(range a, range b) {
	return {safe_add(a.min, b.min), safe_add(a.max, b.max)};
}
range operator-(range a, range b) {
	return {safe_sub(a.min, b.max), safe_sub(a.max, b.min)};
}
range operator*(range a, range b) {
	value x[] = {safe_mul(a.min, b.min), safe_mul(a.max, b.min), safe_mul(a.min, b.max), safe_mul(a.max, b.max)}; //TODO: can this be made more efficient (?)
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator/(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero(); //TODO: return some sort of empty interval (?)
	if (b.min < 0 && b.max > 0) {
		if (a.min == std::numeric_limits<value>::min()) {
			return {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
		}
		value t = std::max(abs(a.min), abs(a.max));
		return {-t, t};
	}
	if (b.min == 0) b.min = 1;
	if (b.max == 0) b.max = -1;
	value x[] = {safe_div(a.min, b.min), safe_div(a.max, b.min), safe_div(a.min, b.max), safe_div(a.max, b.max)};
	return {*std::min_element(x, x + 4), *std::max_element(x, x + 4)};
}
range operator%(range a, range b) {
	if (b == range{0, 0})
		throw division_by_zero();
	range b2 = {b.min <= 0 && b.max >= 0 ? 1 : std::min(std::abs(b.min), std::abs(b.max)), std::max(std::abs(b.min), std::abs(b.max))}; //Watch out for abs(INT_MIN)
	if (a.min / b2.max == a.max / b2.min) {
		value c = a.min / b2.max;
		return {a.min - c * b2.max, a.max - c * b2.min};
	}
	return {0, b2.max - 1};
}
range take_max(range a, range b) {
	return {std::max(a.min, b.min), std::max(a.max, b.max)};
}
range take_min(range a, range b) {
	return {std::min(a.min, b.min), std::min(a.max, b.max)};
}
bool can_zero(range a) {
	return a.min <= 0 && a.max >= 0;
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
	return {a.max < b.min, a.min < b.max};
}
range less_equal(range a, range b) {
	return {a.max <= b.min, a.min <= b.max};
}
range greater_than(range a, range b) {
	return less_than(b, a);
}
range greater_equal(range a, range b) {
	return less_equal(b, a);
}
range equal(range a, range b) {
	return {a.min == a.max && a.min == b.min && a.min == b.max, a.min <= b.max && b.min <= a.max};
}
range not_equal(range a, range b) {
	return {a.min > b.max || b.min > a.max, a.min != a.max || a.min != b.min || a.min != b.max};	
}
range is_positive(range a) {
	if (a.min > 0) return {1, 1};
	if (a.max <= 0) return {0, 0};
	return {0, 1};
}
range is_nonnegative(range a) {
	if (a.min >= 0) return {1, 1};
	if (a.max < 0) return {0, 0};
	return {0, 1};
}
range ternary(range a, range b, range c) { //TODO: make it so a is required to be a boolean and use cast to bool (if needed) when compiling an expression
	range min = is_nonzero(a);
	if (min == range{1, 1}) return b;
	if (min == range{0, 0}) return c;
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
bool is_singleton(range x) {
	return x.min == x.max;
}
std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_min(range a, range b) {
	if (a.max <= b.min) {
		return {std::nullopt, std::numeric_limits<value>::max()};
	}
	if (b.max <= a.min) {
		return {std::numeric_limits<value>::max(), std::nullopt};
	}
	return {std::nullopt, std::nullopt};
}
std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_max(range a, range b) {
	if (a.max <= b.min) {
		return {std::numeric_limits<value>::min(), std::nullopt};
	}
	if (b.max <= a.min) {
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
