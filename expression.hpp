#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "types.hpp"
#include <vector>
#include "parser.hpp"
#include "ranges.hpp"
#include "board.hpp"
#include <bitset>
#include <array>
struct card_player_matrix { 
	value coef[DECK_SIZE][PLAYERS];
	value offset; //TODO: rename to bias (?) (or refactor to entirely remove)
	card_player_matrix();
	value eval(const board &b) const {
		value result = offset;
		for (size_t i = 0; i < DECK_SIZE; ++i) result += coef[i][b.who[i]];
		return result;
	};
	bool is_constant(); //TODO: is there any point in having this function (?)
	auto operator<=>(const card_player_matrix&) const = default;
};
card_player_matrix operator+(card_player_matrix a, const card_player_matrix &b);
card_player_matrix operator-(card_player_matrix a, const card_player_matrix &b);
card_player_matrix operator*(card_player_matrix a, value b);
card_player_matrix operator*(value a, card_player_matrix b);

//TODO: improve type safety of all this e.g. distinguish between player_weigts and suit_weights
template <size_t length> std::array<value, length> operator+(std::array<value, length> a, const std::array<value, length> &b) {
	for (size_t i = 0; i < length; ++i) a[i] += b[i];
	return a;
}

template <size_t length> std::array<value, length> operator-(std::array<value, length> a, const std::array<value, length> &b) {
	for (size_t i = 0; i < length; ++i) a[i] -= b[i];
	return a;
}

template <size_t length> std::array<value, length> operator*(value a, std::array<value, length> b) {
	for (value &v : b) v *= a;
	return b;
}
template <size_t length> std::array<value, length> operator*(std::array<value, length> a, value b) {
	for (value &v : a) v *= b;
	return a;
}


using player_weights = std::array<value, PLAYERS>;
using suit_weights = std::array<value, SUITS>;
using rank_weights = std::array<value, RANKS>;
template <size_t N> std::array<value, N> make_singleton_weight(size_t which) {
	std::array<value, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = (i == which ? 1 : 0);
	return ret;
};
constexpr suit_weights ALL_SUITS = {1, 1, 1, 1};
constexpr suit_weights SPADES_WEIGHTS = {1, 0, 0, 0};
constexpr suit_weights HEARTS_WEIGHTS = {0, 1, 0, 0};
constexpr suit_weights DIAMONDS_WEIGHTS = {0, 0, 1, 0};
constexpr suit_weights CLUBS_WEIGHTS = {0, 0, 0, 1};
constexpr suit_weights ZERO_SUIT_WEIGHTS = {0, 0, 0, 0};
constexpr player_weights NORTH_WEIGHTS = {1, 0, 0, 0};
constexpr player_weights EAST_WEIGHTS = {0, 1, 0, 0};
constexpr player_weights SOUTH_WEIGHTS = {0, 0, 1, 0};
constexpr player_weights WEST_WEIGHTS = {0, 0, 0, 1};
constexpr player_weights NS_WEIGHTS = {1, 0, 1, 0};
constexpr player_weights EW_WEIGHTS = {0, 1, 0, 1};
constexpr player_weights ZERO_PLAYER_WEIGHTS = {0, 0, 0, 0};
constexpr rank_weights ACES_WEIGHTS = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
constexpr rank_weights KINGS_WEIGHTS = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
constexpr rank_weights QUEENS_WEIGHTS = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
constexpr rank_weights JACKS_WEIGHTS = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
constexpr rank_weights ALL_RANKS = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
constexpr rank_weights ZERO_RANK_WEIGHTS = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const rank_weights HCP_WEIGHTS = 4 * ACES_WEIGHTS + 3 * KINGS_WEIGHTS + 2 * QUEENS_WEIGHTS + 1 * JACKS_WEIGHTS;
const rank_weights CONTROLS_WEIGHTS = 2 * ACES_WEIGHTS + 1 * KINGS_WEIGHTS;


card_player_matrix full_product(const player_weights&, const suit_weights&, const rank_weights&);


//add LOGICAL_XOR, ALL COMPARISON OPERATORS (or not all, there's no point in having both <= and >=, just swap the operands), add them to all spots where needed (like parser, evaluator etc.)
enum operations {ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, TAKE_KTH, LOGICAL_AND, LOGICAL_OR, TERNARY, LOGICAL_NOT, LOGICAL_XOR, LEQ, LESS_THAN, EQUAL_TO, NEQ}; //TODO: make this into enum class
//TODO: support having a constant as one of the arguments (at least when it makes sense)
//TODO: is cast to bool needed as a separate type of operation?
struct expression_part { //TODO: rename to anything reasonable xd
	size_t type;
	std::vector<size_t> arguments; //ints (?) idk xd
	value eval(const std::vector<value> &previous_variables) const;
	range eval(const std::vector<range> &previous_variables) const;
};
//TODO: how much gain is there from optimizing all this to one dimensional vector storing data for all variables and some constexpr stuff to get all the indices
//And having O(HAND_SIZE^3) rather than O(HAND_SIZE^4) memory used, also, divided by some 3! or anything alike
struct processed_input_variable {
	range content[HAND_SIZE + 1][HAND_SIZE + 1][HAND_SIZE + 1][HAND_SIZE + 1]; //TODO: make private and overload [] operator taking an array
};

std::bitset<DECK_SIZE> full_deck(); //TODO: make this into just constexpr rather than function call

processed_input_variable process_input_variable(const card_player_matrix&, const std::bitset<DECK_SIZE> &subset);
struct processed_deck_subset { //TODO: do I want it as a separate class, rather then just vector?
	std::vector<processed_input_variable> content; //TODO: make private and overload [] operator
};

using partial_evaluation = std::vector<std::optional<value> > ;
using dp_state = std::pair<partial_evaluation, std::array<int, PLAYERS> >;
struct compiled_expression {
	std::vector<value> used_constants; //TODO: remove, replace probably with variants of operations having hardcoded parameters
	std::vector<card_player_matrix> input_variables;
	std::vector<expression_part> parts;
	value eval(const board &) const;
	// range eval_partial(const board &) const; //TODO: is there any point in having this function (?) I guess it's meant to contain some cards with UNASSIGNED status
	processed_deck_subset process_subset(const std::bitset<DECK_SIZE> &deck_subset) const; //TODO: this (maybe) belongs in a different file
	// std::vector<std::optional<value>> partial_evaluate(const board &) const; //Returns either: nullopt if expression is no longer relevant or some value which makes everything equivalent
	dp_state make_initial_state() const;
	std::pair<board_count, std::vector<board>> run_dp(size_t count = 0) const;
	dp_state append_card(const dp_state &, size_t, size_t, const std::vector<processed_input_variable>&) const;
	std::vector<std::vector<size_t> > partition_deck_into_equivalence_classes() const;
};
struct partial_expression_part {
	enum class argument_type {constant, input_variable, other_expression}; //TODO: this doesn't belong here, put it elsewhere
	size_t type;
	std::vector<std::pair<argument_type, size_t> > arguments;
};


struct expression_compiler {
	std::vector<value> used_constants; //TODO: remove, replace probably with variants of operations having hardcoded parameters
	std::vector<card_player_matrix> input_variables;
	std::vector<partial_expression_part> subexpressions;
	static player_weights parse_players(const parsed_expression &subexpression); 
	static suit_weights parse_suits(const parsed_expression &subexpression); 
	static rank_weights parse_rank(const parsed_expression &subexpression);
	std::pair<partial_expression_part::argument_type, size_t> run_recursive(const parsed_expression &/*subexpression*/);
	compiled_expression finalize(std::pair<partial_expression_part::argument_type, size_t>);
	compiled_expression compile(const parsed_expression &expression);
	size_t add_used_constant(value x);
	size_t add_input_variable(card_player_matrix);
};
std::ostream &operator<<(std::ostream &o, const card_player_matrix &);
std::ostream &operator<<(std::ostream &o, const compiled_expression &);
compiled_expression compile_expression(const parsed_expression &expression);
#endif
