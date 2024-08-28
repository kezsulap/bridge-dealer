#ifndef RANGES_H
#define RANGES_H
#include "types.hpp"
#include <iostream>
struct division_by_zero {};
using range = std::pair <value, value>;
range singleton(value x);
range set_union(range a, range b);
range operator+(range a, range b);
range operator-(range a, range b);
range operator*(range a, range b);
range operator/(range a, range b);
range operator%(range a, range b);
range take_max(range a, range b);
range take_min(range a, range b);
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
#endif
