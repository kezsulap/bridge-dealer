#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <array>
#include <cstdint>
const int DECK_SIZE = 52, PLAYERS = 4, SUITS = 4, RANKS = 13;
const int NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3;
const int SPADES = 0, HEARTS = 1, DIAMONDS = 2, CLUBS = 3;
const int ACE = 12, KING = 11, QUEEN = 10, JACK = 9, TEN = 8;
using value = int;
using partition_t = std::array<uint8_t, DECK_SIZE>;//TODO: Rename to something like deck partition
using board_t = std::vector <std::array <uint8_t, 4>>; //Is it partitioned board? Rename to something more clear
#endif
