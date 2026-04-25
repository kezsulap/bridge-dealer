#include "shapes_set.hpp"
void shapes_set::set_valid(int s, int h, int d, int c) {
	assert(!this->finalized);
	assert(s >= 0 && s <= MAX_LENGTH);
	assert(h >= 0 && h <= MAX_LENGTH);
	assert(d >= 0 && d <= MAX_LENGTH);
	assert(c >= 0 && c <= MAX_LENGTH);
	assert(s + h + d + c == HAND_SIZE);
	this->valid[s][h][d][c] = true;
}
void shapes_set::set_invalid(int s, int h, int d, int c) {
	assert(!this->finalized);
	assert(s >= 0 && s <= MAX_LENGTH);
	assert(h >= 0 && h <= MAX_LENGTH);
	assert(d >= 0 && d <= MAX_LENGTH);
	assert(c >= 0 && c <= MAX_LENGTH);
	assert(s + h + d + c == HAND_SIZE);
	this->valid[s][h][d][c] = false;
}
void shapes_set::finalize() {
	assert(!this->finalized);
	auto get = [&](int s, int h, int d, int c, bool count_valid) -> int {
		if (s + h + d + c != HAND_SIZE) return 0;
		return this->valid[s][h][d][c] == count_valid;
	};
	for (int s = 0; s <= MAX_LENGTH; ++s) {
		for (int h = 0; h <= MAX_LENGTH; ++h) {
			for (int d = 0; d <= MAX_LENGTH; ++d) {
				for (int c = 0; c <= MAX_LENGTH; ++c) {
					this->valid_below[s + 1][h + 1][d + 1][c + 1] = get(s, h, d, c, true);
					this->invalid_below[s + 1][h + 1][d + 1][c + 1] = get(s, h, d, c, false);
				}
			}
		}
	}
	for (int s = 1; s <= MAX_LENGTH + 1; ++s) {
		for (int h = 0; h <= MAX_LENGTH + 1; ++h) {
			for (int d = 0; d <= MAX_LENGTH + 1; ++d) {
				for (int c = 0; c <= MAX_LENGTH + 1; ++c) {
					this->valid_below[s][h][d][c] += this->valid_below[s - 1][h][d][c];
					this->invalid_below[s][h][d][c] += this->invalid_below[s - 1][h][d][c];
				}
			}
		}
	}
	for (int s = 0; s <= MAX_LENGTH + 1; ++s) {
		for (int h = 1; h <= MAX_LENGTH + 1; ++h) {
			for (int d = 0; d <= MAX_LENGTH + 1; ++d) {
				for (int c = 0; c <= MAX_LENGTH + 1; ++c) {
					this->valid_below[s][h][d][c] += this->valid_below[s][h - 1][d][c];
					this->invalid_below[s][h][d][c] += this->invalid_below[s][h - 1][d][c];
				}
			}
		}
	}
	for (int s = 0; s <= MAX_LENGTH + 1; ++s) {
		for (int h = 0; h <= MAX_LENGTH + 1; ++h) {
			for (int d = 1; d <= MAX_LENGTH + 1; ++d) {
				for (int c = 0; c <= MAX_LENGTH + 1; ++c) {
					this->valid_below[s][h][d][c] += this->valid_below[s][h][d - 1][c];
					this->invalid_below[s][h][d][c] += this->invalid_below[s][h][d - 1][c];
				}
			}
		}
	}
	for (int s = 0; s <= MAX_LENGTH + 1; ++s) {
		for (int h = 0; h <= MAX_LENGTH + 1; ++h) {
			for (int d = 0; d <= MAX_LENGTH + 1; ++d) {
				for (int c = 1; c <= MAX_LENGTH + 1; ++c) {
					this->valid_below[s][h][d][c] += this->valid_below[s][h][d][c - 1];
					this->invalid_below[s][h][d][c] += this->invalid_below[s][h][d][c - 1];
				}
			}
		}
	}
	this->finalized = true;
}
range shapes_set::evaluate(range s, range h, range d, range c) const {
	assert(this->finalized);
	assert(s.min >= 0 && s.max <= MAX_LENGTH);
	assert(h.min >= 0 && h.max <= MAX_LENGTH);
	assert(d.min >= 0 && d.max <= MAX_LENGTH);
	assert(c.min >= 0 && c.max <= MAX_LENGTH);
	int valid_in = 0, invalid_in = 0;
	for (int s_index = 0; s_index < 2; ++s_index) {
		for (int h_index = 0; h_index < 2; ++h_index) {
			for (int d_index = 0; d_index < 2; ++d_index) {
				for (int c_index = 0; c_index < 2; ++c_index) {
					int sign = (s_index + h_index + d_index + c_index) % 2 ? -1 : 1;
					valid_in += valid_below[s_index ? s.max + 1 : s.min][h_index ? h.max + 1 : h.min][d_index ? d.max + 1 : d.min][c_index ? c.max + 1 : c.min] * sign;
					invalid_in += invalid_below[s_index ? s.max + 1 : s.min][h_index ? h.max + 1 : h.min][d_index ? d.max + 1 : d.min][c_index ? c.max + 1 : c.min] * sign;
				}
			}
		}
	}				
	assert(valid_in >= 0);
	assert(invalid_in >= 0);
	assert(valid_in + invalid_in > 0);
	return {!invalid_in, !!valid_in};
}
