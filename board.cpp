#include "board.hpp"
#include <vector>
#include <algorithm>
#include <sstream>
std::string generate_suit(const std::vector<int> &cards) {
	if (cards.empty()) {
		return "---" + std::string(RANKS - 3, ' ');
	}
	std::stringstream o;
	for (int x : cards) o << RANK_SYMBOLS[x];
	o << std::string(RANKS - cards.size(), ' ');
	return o.str();
}

void board::output(std::ostream &o) const {
	std::vector <int> cards[PLAYERS][SUITS], unassigned[SUITS];
	bool any_unassigned = false;
	for (int i = 0; i < DECK_SIZE; ++i) { //TODO: move to separate function
		int suit = get_suit(i), rank = get_rank(i);
		int who_has = who[i];
		if (who_has == UNASSIGNED_CARD) {
			unassigned[suit].push_back(rank);
			any_unassigned = true;
		}
		else cards[who_has][suit].push_back(rank);
	}
	for (int i = 0; i < PLAYERS; ++i)
		for (int j = 0; j < SUITS; ++j)
			std::reverse(cards[i][j].begin(), cards[i][j].end());
	for (int i = 0; i < SUITS; ++i)
		std::reverse(unassigned[i].begin(), unassigned[i].end());
	for (int i = 0; i < SUITS; ++i) {
		o << std::string(RANKS, ' ') << suit_color_codes[i] << generate_suit(cards[NORTH][i]) << reset_colors << std::string(RANKS, ' ') << "\n";
	}
	for (int i = 0; i < SUITS; ++i) {
		o << suit_color_codes[i] << generate_suit(cards[WEST][i]) << reset_colors << std::string(RANKS, ' ') << suit_color_codes[i] << generate_suit(cards[EAST][i]) << reset_colors << "\n";
	}
	for (int i = 0; i < SUITS; ++i) {
		o << std::string(RANKS, ' ') << suit_color_codes[i] << generate_suit(cards[SOUTH][i]) << reset_colors << std::string(RANKS, ' ') << "\n";
	}
	if (any_unassigned) {
		o << "Unassigned cards:\n";
		for (int i = 0; i < SUITS; ++i) {
			if (!unassigned[i].empty()) {
				o << suit_color_codes[i] << generate_suit(unassigned[i]) << reset_colors << "\n";
			}
		}
	}
}
