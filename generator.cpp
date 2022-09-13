#include <bits/stdc++.h> //TODO, replace
using namespace std; //TODO, remove
template <class Output, class Filter> void split(board_t board, size_t index, int current_size, int new_size, Output output, Filter filter) {
	auto [north, east, south, west] = board[index];
	assert(north + east + south + west == current_size);
	board.emplace_back();
	int leave_out = current_size - new_size;
	for (int new_north = max(0, north - leave_out); new_north <= min<int>(north, new_size); ++new_north) {
		for (int new_east = max(0, east - (leave_out - (north - new_north))); new_east <= min<int>(east, new_size - new_north); ++new_east) {
			for (int new_south = max(0, south - (leave_out - (north - new_north) - (east - new_east))); new_south <= min<int>(south, new_size - new_north - new_east); ++new_south) {
				int new_west = new_size - new_north - new_east - new_south;
				assert(new_north >= 0 && new_north <= north);
				assert(new_east >= 0 && new_east <= east);
				assert(new_south >= 0 && new_south <= south);
				assert(new_west >= 0 && new_west <= west);
				board[index] = {(uint8_t)(north - new_north), (uint8_t)(east - new_east), (uint8_t)(south - new_south), (uint8_t)(east - new_east)};
				board.back() = {(uint8_t)new_north, (uint8_t)new_east, (uint8_t)new_south, (uint8_t)new_east};
				if (filter(board)) *output = board;
			}
		}
	}
}
void run_partition(const vector <board_t> prev_partition, vector<board_t> &output, size_t index, int current_size, int new_size) {
	auto filter = [&](auto &x){return true;};
	for (auto b : prev_partition) split(b, index, current_size, new_size, back_inserter(output), filter);
}
int main() {
	vector<board_t> blank_board = {{{13, 13, 13, 13}}};
	vector <board_t> split_1;
	run_partition(blank_board, split_1, 0, 52, 26);
	debug imie(split_1) imie(split_1.size());
}
