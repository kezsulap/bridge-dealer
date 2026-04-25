#ifndef RANGES_H
#define RANGES_H
#include "types.hpp"
#include <iostream>
#include <vector>
#include <optional>
struct division_by_zero {};
struct range {
	value min, max;
	range(int _min, int _max): min(_min), max(_max) {
		assert(_min <= _max);
	}
	range(): min(0), max(0){}
	bool operator==(const range &oth) const = default;
};
range singleton(value x);
range set_union(range a, range b);
range operator+(range a, range b);
range operator-(range a, range b);
range operator*(range a, range b);
range operator/(range a, range b);
range operator%(range a, range b);
range take_max(range a, range b);
range take_min(range a, range b);
range less_than(range a, range b);
range less_equal(range a, range b);
range greater_than(range a, range b);
range greater_equal(range a, range b);
range equal(range a, range b);
range not_equal(range a, range b);
//0 based
range take_kth(const std::vector<range> &ranges, size_t k);
/* TODO: does anyone outside need these functions?
	bool can_zero(range a);
	bool can_nonzero(range a);
	range is_zero(range a);
	range is_nonzero(range a);
	range is_positive(range a);
	range is_nonnegative(range a);
*/
range ternary(range a, range b, range c);
range logical_and(range a, range b);
range logical_or(range a, range b);
range logical_not(range a);
std::ostream &operator<<(std::ostream &, range);

bool is_singleton(range x);

std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_min(range a, range b);
std::tuple<std::optional<value>, std::optional<value>> replace_irrelevant_max(range a, range b);
std::tuple<std::optional<value>, std::optional<value>, std::optional<value> > replace_irrelevant_ternary(range a, range b, range c);
#endif
