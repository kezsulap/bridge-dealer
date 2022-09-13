#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "types.hpp"
#include <vector>
#include <memory>
struct variable {
	value coef[DECK_SIZE][PLAYERS];
	value offset;
	variable();
	bool is_constant();
};
variable operator+(variable a, const variable &b);
variable operator-(variable a, const variable &b);
variable operator*(variable a, value b);
variable operator*(value a, variable b);
enum operations {ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, TAKE_MIN, TAKE_MAX, LOGICAL_AND, LOGICAL_OR, TERNARY, };
struct compound_expression {
	
	std::vector <std::shared_ptr<expression> > subexpressions;
};
variable operator*(variable a, variable b);
#endif
