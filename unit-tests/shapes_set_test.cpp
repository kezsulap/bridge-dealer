#include "../shapes_set.hpp"
#include "../output_operators.hpp"
#include "test_utils.hpp"
#include <vector>
#include <tuple>

const int ANY = -1;

void run_testcase(std::vector<std::tuple<int, int, int, int> > shapes_list_included, std::vector<std::tuple<int, int, int, int> > shapes_list_excluded) {
	std::cerr << "RUNNING ON shapes_list_included = " << shapes_list_included << ", shapes_list_excluded = " << shapes_list_excluded << "\n";
	shapes_set set;
	bool valid[14][14][14][14] = {};
	for (auto [s_bound, h_bound, d_bound, c_bound] : shapes_list_included) {
		int count_done = 0;
		for (int s = (s_bound == ANY ? 0 : s_bound); s <= (s_bound == ANY ? 13 : s_bound); ++s) {
			for (int h = (h_bound == ANY ? 0 : h_bound); h <= (h_bound == ANY ? 13 : h_bound); ++h) {
				for (int d = (d_bound == ANY ? 0 : d_bound); d <= (d_bound == ANY ? 13 : d_bound); ++d) {
					for (int c = (c_bound == ANY ? 0 : c_bound); c <= (c_bound == ANY ? 13 : c_bound); ++c) {
						if (s + h + d + c == 13) {
							valid[s][h][d][c] = true;
							set.set_valid(s, h, d, c);
							count_done++;
						}
					}
				}
			}
		}
		assert(count_done > 0);
	}
	for (auto [s_bound, h_bound, d_bound, c_bound] : shapes_list_excluded) {
		int count_done = 0;
		for (int s = (s_bound == ANY ? 0 : s_bound); s <= (s_bound == ANY ? 13 : s_bound); ++s) {
			for (int h = (h_bound == ANY ? 0 : h_bound); h <= (h_bound == ANY ? 13 : h_bound); ++h) {
				for (int d = (d_bound == ANY ? 0 : d_bound); d <= (d_bound == ANY ? 13 : d_bound); ++d) {
					for (int c = (c_bound == ANY ? 0 : c_bound); c <= (c_bound == ANY ? 13 : c_bound); ++c) {
						if (s + h + d + c == 13) {
							valid[s][h][d][c] = false;
							set.set_invalid(s, h, d, c);
							count_done++;
						}
					}
				}
			}
		}
		assert(count_done > 0);
	}
	set.finalize();
	for (int s_low = 0; s_low <= 13; ++s_low) for (int s_high = s_low; s_high <= 13; ++s_high) {
		for (int h_low = 0; h_low <= 13; ++h_low) for (int h_high = h_low; h_high <= 13; ++h_high) {
			for (int d_low = 0; d_low <= 13; ++d_low) for (int d_high = d_low; d_high <= 13; ++d_high) {
				for (int c_low = 0; c_low <= 13; ++c_low) for (int c_high = c_low; c_high <= 13; ++c_high) {
					if (s_low + h_low + d_low + c_low <= 13 && s_high + h_high + d_high + c_high >= 13) {
						bool any = false, all = true;
						for (int s = s_low; s <= s_high; ++s) {
							for (int h = h_low; h <= h_high; ++h) {
								for (int d = d_low; d <= d_high; ++d) {
									for (int c = c_low; c <= c_high; ++c) {
										if (s + h + d + c == 13) {
											if (valid[s][h][d][c]) {
												any = true;
											}
											else {
												all = false;
											}
										}
									}
								}
							}
						}
						range class_result = set.evaluate(range(s_low, s_high), range(h_low, h_high), range(d_low, d_high), range(c_low, c_high));
						ASSERT_EQUAL_SILENT(any, class_result.max == 1);
						ASSERT_EQUAL_SILENT(all, class_result.min == 1);
					}
				}
			}
		}
	}
}

int main() {
	run_testcase({}, {});
	run_testcase({{ANY, ANY, 5, 3}}, {});
	run_testcase({{4, 3, 3, 3}, {3, 4, 3, 3}, {3, 3, 4, 3}, {3, 3, 3, 4}}, {});
	run_testcase({{ANY, ANY, ANY, ANY}}, {{ANY, 1, 2, ANY}});
	run_testcase({{ANY, ANY, ANY, ANY}}, {{ANY, 0, 0, ANY}, {ANY, 0, ANY, 0}, {0, ANY, ANY, 0}});
	run_testcase({{ANY, 0, 0, ANY}, {ANY, 0, ANY, 0}, {0, ANY, ANY, 0}}, {{0, 0, 0, 13}});
	run_testcase({{13, 0, 0, 0}, {0, 13, 0, 0}, {0, 0, 13, 0}}, {});
	run_testcase({{13, 0, 0, 0}, {0, 13, 0, 0}, {0, 0, 0, 13}, {2, 2, ANY, ANY}}, {{2, 2, 7, 2}});
}
