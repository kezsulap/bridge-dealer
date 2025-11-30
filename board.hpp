#ifndef BOARD_H
#define BOARD_H
#include "types.hpp"
#include <ostream>
#include <string>
#include <random>
const int8_t UNASSIGNED_CARD = -1;
struct partial_board;

struct board {
	//this one is not supposed to have UNASSIGNED_CARDs
	int8_t who[DECK_SIZE]; //TODO: is there any gain from optimizing this to using 2 bits per card (?)
	void output(std::ostream &o) const;
	partial_board to_partial() const;
	board(const std::string &notation);
	board(std::mt19937 &rng);
};


struct partial_board {
	//this one is not supposed to have UNASSIGNED_CARDs
	int8_t who[DECK_SIZE];
	void output(std::ostream &o) const;
	board finalize() const;
};
#endif
