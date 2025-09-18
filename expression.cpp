#include "expression.hpp"
#include <cstring>
#include <sstream>
card_player_matrix operator+(card_player_matrix a, const card_player_matrix &b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] += b.coef[i][j];
	a.offset += b.offset;
	return a;
}
card_player_matrix operator-(card_player_matrix a, const card_player_matrix &b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] -= b.coef[i][j];
	a.offset -= b.offset;
	return a;
}
card_player_matrix operator*(card_player_matrix a, value b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] *= b;
	a.offset *= b;
	return a;
}
card_player_matrix::card_player_matrix() {
	memset(this, 0, sizeof(*this));
}
bool card_player_matrix::is_constant() {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			if (coef[i][j])
				return false;
	return true;
}
std::ostream &operator<<(std::ostream &o, const card_player_matrix &v) {
	bool anything = false;
	for (int who = 0; who < PLAYERS; ++who) {
		std::stringstream curr_player_desc;
		bool any = false;
		for (int card = 0; card < DECK_SIZE; ++card) {
			value coef = v.coef[card][who];
			if (coef) {
				if (coef > 1) {
					if (any) curr_player_desc << "+";
					curr_player_desc << coef;
				}
				else if (coef == 1) {
					if (any) curr_player_desc << "+";
				}
				else if (coef == -1) {
					curr_player_desc << "-";
				}
				else { //coef < -1
					curr_player_desc << coef;
				}
				curr_player_desc << SUITS_SYMBOLS[get_suit(card)] << RANK_SYMBOLS[get_rank(card)]; //TODO: extract to some separate function
				any = true;
			}
		}
		if (any) {
			if (anything) o << "+";
			o << PLAYERS_STR[who] << "*(" << curr_player_desc.str() << ")";
			anything = true;
		}
	}
	if (v.offset) {
		if (v.offset > 0 && anything) o << "+";
		o << v.offset;
		anything = true;
	}
	if (!anything) o << "0";
	return o;
}
std::ostream &operator<<(std::ostream &o, const compiled_expression &expression) {
	for (size_t i = 0; i < expression.input_variables.size(); ++i) {
		o << "x_" << i << " := " << expression.input_variables[i] << "\n";
	}
	for (size_t i = 0; i < expression.parts.size(); ++i) {
		o << "x_" << i + expression.input_variables.size() << " := ";
		switch(expression.parts[i].type) {
			case ADD: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " + x_" << expression.parts[i].arguments[1];
			} break;
			case SUBTRACT: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " - x_" << expression.parts[i].arguments[1];
			} break;
			case MULTIPLY: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " * x_" << expression.parts[i].arguments[1];
			} break;
			case DIVIDE: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " / x_" << expression.parts[i].arguments[1];
			} break;
			case MODULO: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " % x_" << expression.parts[i].arguments[1];
			} break;
			case TAKE_KTH: {
				assert(!expression.parts[i].arguments.empty());
				if (expression.parts[i].arguments[0] == 0) o << "min(";
				else if (expression.parts[i].arguments[0] == expression.parts[i].arguments.size() - 2) o << "max(";
				else o << "kth_smallest(" << expression.parts[i].arguments[0] + 1 << ", ";
				for (size_t j = 1; i < expression.parts[j].arguments.size(); ++j) {
					if (j) o << ", ";
					o << "x_" << expression.parts[i].arguments[j];
				}
				o << ")";
			} break;
			case LOGICAL_AND: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " && x_" << expression.parts[i].arguments[1];
			} break;
			case LOGICAL_OR: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " || x_" << expression.parts[i].arguments[1];
			} break;
			case TERNARY: {
				assert(expression.parts[i].arguments.size() == 3u);
				o << "x_" << expression.parts[i].arguments[0] << " ? x_" << expression.parts[i].arguments[1] << " : x_" << expression.parts[i].arguments[2];
			} break;
			case LOGICAL_NOT: {
				assert(expression.parts[i].arguments.size() == 1u);
				o << "!x_" << expression.parts[i].arguments[0];
			} break;
			default: assert(false);
		};
		o << "\n";
	}
	return o;
}
card_player_matrix full_product(const player_weights& players, const suit_weights& suits, const rank_weights& ranks) {
	card_player_matrix ret;
	for (size_t player = 0; player < PLAYERS; ++player) {
		for (size_t suit = 0; suit < SUITS; ++suit) {
			for (size_t rank = 0; rank < RANKS; ++rank) {
				ret.coef[make_card(rank, suit)][player] = players[player] * suits[suit] * ranks[rank];
			}
		}
	}
	return ret;
}
processed_input_variable process_input_variable(const card_player_matrix& matrix, const std::bitset<DECK_SIZE> &subset) {
	std::optional<range> values[HAND_SIZE + 1][HAND_SIZE + 1][HAND_SIZE + 1][HAND_SIZE + 1];
	auto touch = [&](range this_value, int north, int east, int south, int west) {
		auto &current_value = values[north][east][south][west];
		if (current_value.has_value()) *current_value = set_union(*current_value, this_value);
		else current_value = this_value;
	};
	touch(singleton(matrix.offset),  0, 0, 0, 0);
	size_t size_so_far = 0;
	for (size_t card_id = 0; card_id < DECK_SIZE; ++card_id) {
		if (!subset[card_id]) continue;
		for (size_t north_length = 0; north_length <= HAND_SIZE && north_length <= size_so_far; ++north_length) {
			for (size_t east_length = 0; east_length <= HAND_SIZE && east_length + north_length <= size_so_far; ++east_length) {
				for (size_t south_length = 0; south_length <= HAND_SIZE && south_length + east_length + north_length <= size_so_far; ++south_length) {
					size_t west_length = size_so_far - south_length - east_length - north_length;
					if (west_length > HAND_SIZE) continue;
					if (north_length < HAND_SIZE) {
						touch(*values[north_length][east_length][south_length][west_length] + singleton(matrix.coef[card_id][NORTH]), north_length + 1, east_length, south_length, west_length);
					}
					if (east_length < HAND_SIZE) {
						touch(*values[north_length][east_length][south_length][west_length] + singleton(matrix.coef[card_id][EAST]), north_length, east_length + 1, south_length, west_length);
					}
					if (south_length < HAND_SIZE) {
						touch(*values[north_length][east_length][south_length][west_length] + singleton(matrix.coef[card_id][SOUTH]), north_length, east_length, south_length + 1, west_length);
					}
					if (west_length < HAND_SIZE) {
						touch(*values[north_length][east_length][south_length][west_length] + singleton(matrix.coef[card_id][WEST]), north_length, east_length, south_length, west_length + 1);
					}
				}
			}
		}
		size_so_far++;
	}
	//TODO: Keep the code above as is, rename processed_input_variable into some name indicating it's just draft version and compress only the final result into something more memory efficient
	processed_input_variable ret;
	for (size_t north = 0; north <= HAND_SIZE; ++north) {
		for (size_t east = 0; east <= HAND_SIZE; ++east) {
			for (size_t south = 0; south <= HAND_SIZE; ++south) {
				for (size_t west = 0; west <= HAND_SIZE; ++west) {
					ret.content[north][east][south][west] = *values[north][east][south][west];
				}
			}
		}
	}
	return ret;
}
compiled_expression compile_expression(const parsed_expression &expression) {
	//TODO:
	//Identify any common parts and extract them into a shared card_player_matrix
	//
}
