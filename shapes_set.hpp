#ifndef SHAPES_SET_H
#define SHAPES_SET_H
#include "types.hpp"
#include "ranges.hpp"
#include <algorithm>
#include <array>
constexpr int MAX_LENGTH = std::min(HAND_SIZE, RANKS);
struct shapes_set {
	static_assert(SUITS == 4, "shapes not supported for number of suits other than 4");
	void set_valid(int s, int h, int d, int c);
	void set_invalid(int s, int h, int d, int c);
	void finalize();
	range evaluate(range s, range h, range d, range c) const;
	shapes_set() {}
private:
	std::array<std::array<std::array<std::array<bool, MAX_LENGTH + 1>, MAX_LENGTH + 1>, MAX_LENGTH + 1>, MAX_LENGTH + 1> valid{};
	bool finalized = false;
	std::array<std::array<std::array<std::array<int, MAX_LENGTH + 2>, MAX_LENGTH + 2>, MAX_LENGTH + 2>, MAX_LENGTH + 2> valid_below{}, invalid_below{};
};
#endif
