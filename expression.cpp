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
processed_input_variable process_input_variable(const card_player_matrix&, const std::bitset<DECK_SIZE> &subset) {
	processed_input_variable ret;
	static_assert(PLAYERS == 4);
	throw "not implemented yet";
}
compiled_expression compile_expression(const parsed_expression &expression) {
	//TODO:
	//Identify any common parts and extract them into a shared card_player_matrix
	//
}
