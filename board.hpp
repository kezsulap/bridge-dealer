#ifndef BOARD_H
#define BOARD_H
#include "types.hpp"
#include <ostream>
const int8_t UNASSIGNED_CARD = -1;
struct board {
	int8_t who[DECK_SIZE]; //TODO: is there any gain from optimizing this to using 2 bits per card (?)
	void output(std::ostream &o) const; 
};
#endif
