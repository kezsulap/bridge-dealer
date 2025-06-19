#ifndef OUTPUT_OPERATORS_H
#define OUTPUT_OPERATORS_H
#include <utility>
#include <string>
#include <ostream>
#include <type_traits>
template <class c, class d> std::ostream &operator<<(std::ostream &o, const std::pair <c, d> &x) {
	return o << "(" << x.first << ", " << x.second << ")";
}
template <class c> auto operator<<(std::ostream &o, const c&v)
		-> typename std::enable_if<!std::is_same<c, std::string>::value, decltype(v.end(), o)>::type {
	o << "{";
	int q = 0;
	for (auto &x : v) o << ", " + 2 * !q++ << x;
	return o << "}";
}
#endif
