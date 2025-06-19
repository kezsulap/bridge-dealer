#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "types.hpp"
#include <vector>
#include <memory>
#include <variant>
using namespace std;
struct variable {
	value coef[DECK_SIZE][PLAYERS];
	value offset; //TODO: rename to bias (?)
	variable();
	bool is_constant();
};
variable operator+(variable a, const variable &b);
variable operator-(variable a, const variable &b);
variable operator*(variable a, value b);
variable operator*(value a, variable b);
enum operations {ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, TAKE_MIN, TAKE_MAX, LOGICAL_AND, LOGICAL_OR, TERNARY, LOGICAL_NOT};
struct compound_expression;
struct expression {
	std::variant<std::unique_ptr<variable>, std::unique_ptr<compound_expression> > content;
};
struct compound_expression {
	uint8_t type;
	std::vector <expression> subexpressions;
};
ostream &operator<<(ostream &o, const variable &v) {
	bool anything = false;
	for (int who = 0; who < PLAYERS; ++who) {
		stringstream curr_player_desc;
		bool any = false;
		for (int card = 0; card < DECK; ++card) {
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
ostream &operator<<(ostream &o, const compound_expression &c) {
	
}
ostream&operator<<(ostream &o, const expression &x) {
	if (x.content.index() == 0) return o << *get<0>(x.content);
	return o << *get<1>(x.content);
}
#endif
