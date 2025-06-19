#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <cassert>
const int PLAYERS = 4, SUITS = 4, RANKS = 13, DECK_SIZE = SUITS * RANKS;
const int NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3;
const std::string PLAYERS_STR[PLAYERS] = {"NORTH", "EAST", "SOUTH", "WEST"};
const char PLAYERS_SYMBOLS[PLAYERS] = {'N', 'E', 'S', 'W'};
const int SPADES = 0, HEARTS = 1, DIAMONDS = 2, CLUBS = 3;
const std::string SUITS_STR[SUITS] = {"SPADES", "HEARTS", "DIAMONDS", "CLUBS"};
const std::string SUITS_SYMBOLS[SUITS] = {"♠", "♥", "♦", "♣"};
const int ACE = 12, KING = 11, QUEEN = 10, JACK = 9, TEN = 8;
const char RANK_SYMBOLS[RANKS] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
inline int get_suit(int card) {
	assert(card >= 0 && card < DECK_SIZE);
	return card / SUITS;
}
inline int get_rank(int card) {
	assert(card >= 0 && card < DECK_SIZE);
	return card % SUITS;
}
inline int make_card(int rank, int suit) {
	assert(rank >= 0 && rank < RANKS);
	assert(suit >= 0 && suit < SUITS);
	return rank * SUITS + suit;
}
using value = int;
using partition_t = std::array<uint8_t, DECK_SIZE>;//TODO: Rename to something like deck partition
using board_t = std::vector <std::array <uint8_t, 4>>; //Is it partitioned board? Rename to something more clear
#endif
