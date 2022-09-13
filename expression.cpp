#include "expression.hpp"
#include <cstring>
variable operator+(variable a, const variable &b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] += b.coef[i][j];
	a.offset += b.offset;
	return a;
}
variable operator-(variable a, const variable &b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] -= b.coef[i][j];
	a.offset -= b.offset;
	return a;
}
variable operator*(variable a, value b) {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			a.coef[i][j] *= b;
	a.offset *= b;
	return a;
}
variable::variable() {
	memset(this, 0, sizeof(*this));
}
bool variable::is_constant() {
	for (int i = 0; i < DECK_SIZE; ++i)
		for (int j = 0; j < PLAYERS; ++j)
			if (coef[i][j])
				return false;
	return true;
}
