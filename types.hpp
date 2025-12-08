#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <cassert>
const int PLAYERS = 4, SUITS = 4, RANKS = 13, DECK_SIZE = SUITS * RANKS;
static_assert(DECK_SIZE % PLAYERS == 0);
const int HAND_SIZE = DECK_SIZE / PLAYERS;;
const int NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3;
const std::string PLAYERS_STR[PLAYERS] = {"NORTH", "EAST", "SOUTH", "WEST"};
const char PLAYERS_SYMBOLS[PLAYERS] = {'N', 'E', 'S', 'W'};
const int SPADES = 0, HEARTS = 1, DIAMONDS = 2, CLUBS = 3;
const std::string SUITS_STR[SUITS] = {"SPADES", "HEARTS", "DIAMONDS", "CLUBS"};
const std::string SUITS_SYMBOLS[SUITS] = {"♠", "♥", "♦", "♣"};
const int ACE = 12, KING = 11, QUEEN = 10, JACK = 9, TEN = 8;
const char RANK_SYMBOLS[RANKS] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
using board_count = __int128;
const board_count TOTAL_COUNT = 635013559600 * (board_count) 8122425444 * (board_count) 10400600; //TODO: make this into anything "normal"
inline board_count parse_int128(const std::string x) {
	board_count ret = 0;
	for (char c : x) {
		assert(c >= '0' && c <= '9');
		ret = ret * 10 + (c - '0');
	}
	return ret;
}
inline int char_to_rank(char x) {
	for (size_t i = 0; i < RANKS; ++i) if (RANK_SYMBOLS[i] == x) return i;
	assert(false);
}
const std::string suit_color_codes[SUITS] = {"\033[38:2:0:130:255m", "\033[38:2:255:0:0m", "\033[38:2:255:165:0m", "\033[38:2:0:255:0m"};
const std::string reset_colors = "\033[0m";
inline int get_rank(int card) {
	assert(card >= 0 && card < DECK_SIZE);
	return card / SUITS;
}
inline int get_suit(int card) {
	assert(card >= 0 && card < DECK_SIZE);
	return card % SUITS;
}
inline int make_card(int rank, int suit) {
	assert(rank >= 0 && rank < RANKS);
	assert(suit >= 0 && suit < SUITS);
	return rank * SUITS + suit;
}
inline std::string card_to_str(int card) {
	return SUITS_SYMBOLS[get_suit(card)] + RANK_SYMBOLS[get_rank(card)];
}
using value = int;
#endif
