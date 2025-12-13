#include "expression.hpp"
#include "output_operators.hpp"
#include "types.hpp"
#include <cstring>
#include <limits>
#include <optional>
#include <sstream>
#include <algorithm>
#include <map>
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
	for (size_t i = 0; i < expression.used_constants.size(); ++i) {
		o << "x_" << i << " := " << expression.used_constants[i] << "\n";
	}
	for (size_t i = 0; i < expression.input_variables.size(); ++i) {
		o << "x_" << i + expression.used_constants.size() << " := " << expression.input_variables[i] << "\n";
	}
	for (size_t i = 0; i < expression.parts.size(); ++i) {
		o << "x_" << i + expression.used_constants.size() + expression.input_variables.size() << " := ";
		switch(expression.parts[i].type) {
			case ADD: { //TODO: don't copypaste for those which just have argument_1 operator argument_2
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
				for (size_t j = 1; j < expression.parts[i].arguments.size(); ++j) {
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
			case LESS_THAN: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " < x_" << expression.parts[i].arguments[1];
			} break;
			case LEQ: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " <= x_" << expression.parts[i].arguments[1];
			} break;
			case EQUAL_TO: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " == x_" << expression.parts[i].arguments[1];
			} break;
			case NEQ: {
				assert(expression.parts[i].arguments.size() == 2u);
				o << "x_" << expression.parts[i].arguments[0] << " != x_" << expression.parts[i].arguments[1];
			} break;
			default: {
								 std::cerr << "expression.parts[i].type = " << expression.parts[i].type << std::endl;
								 assert(false);
							 }
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
					ret.content[north][east][south][west] = values[north][east][south][west].has_value() ? *values[north][east][south][west] : range{0, 0}; //TODO: this is ugly, but probably will be deleted anyway when optimized to store this stuff in a different way
				}
			}
		}
	}
	return ret;
}
std::bitset<DECK_SIZE> full_deck() {return std::bitset<DECK_SIZE>().set();}

value compiled_expression::eval(const board &b) const {
	std::vector <value> values = used_constants;
	for (const card_player_matrix &input_variable : input_variables) {
		values.push_back(input_variable.eval(b));
	}
	for (const expression_part &part : parts) {
		values.push_back(part.eval(values));
	}
	return values.back();
}


value expression_part::eval(const std::vector<value> &previous_variables) const {
	switch (type) {
		case ADD: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] + previous_variables[arguments[1]];
		} break;
		case SUBTRACT: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] - previous_variables[arguments[1]];
		} break;
		case MULTIPLY: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] * previous_variables[arguments[1]];
		} break;
		case DIVIDE: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] / previous_variables[arguments[1]]; //TODO: what to do if there's a division by 0, or MIN_INT / -1? Raise some exception I guess
		} break;
		case MODULO: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] % previous_variables[arguments[1]]; //TODO: what to do if there's a division by 0, or MIN_INT % -1? Raise some exception I guess
		} break;
		case TAKE_KTH: {
			assert(!arguments.empty());
			std::vector<value> argument_previous_variables;
			for (size_t i = 1; i < arguments.size(); ++i) argument_previous_variables.push_back(previous_variables[arguments[i]]);
			std::nth_element(argument_previous_variables.begin(), argument_previous_variables.begin() + arguments[0], argument_previous_variables.end());
			return argument_previous_variables[arguments[0]];
		} break;
		case LOGICAL_AND: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] && previous_variables[arguments[1]];
		} break;
		case LOGICAL_OR: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] || previous_variables[arguments[1]];
		} break;
		case TERNARY: {
			assert(arguments.size() == 3u);
			return previous_variables[arguments[0]] ? previous_variables[arguments[1]] : previous_variables[arguments[2]];
		} break;
		case LOGICAL_NOT: {
			assert(arguments.size() == 1u);
			return !previous_variables[arguments[0]];
		} break;
		case LEQ: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] <= previous_variables[arguments[1]];
		} break;
		case LESS_THAN: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] < previous_variables[arguments[1]];
		} break;
		case EQUAL_TO: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] == previous_variables[arguments[1]];
		} break;
		case NEQ: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] != previous_variables[arguments[1]];
		} break;
		default: assert(false);
	}
}
range expression_part::eval(const std::vector<range> &previous_variables) const {
	switch (type) {
		case ADD: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] + previous_variables[arguments[1]];
		} break;
		case SUBTRACT: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] - previous_variables[arguments[1]];
		} break;
		case MULTIPLY: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] * previous_variables[arguments[1]];
		} break;
		case DIVIDE: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] / previous_variables[arguments[1]]; //TODO: what to do if there's a division by 0, or MIN_INT / -1? Raise some exception I guess
		} break;
		case MODULO: {
			assert(arguments.size() == 2u);
			return previous_variables[arguments[0]] % previous_variables[arguments[1]]; //TODO: what to do if there's a division by 0, or MIN_INT % -1? Raise some exception I guess
		} break;
		case TAKE_KTH: {
										 assert(false); //TODO
		} break;
		case LOGICAL_AND: {
			assert(arguments.size() == 2u);
			return logical_and(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		case LOGICAL_OR: {
			assert(arguments.size() == 2u);
			return logical_or(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		case TERNARY: {
			assert(arguments.size() == 3u);
			return ternary(previous_variables[arguments[0]], previous_variables[arguments[1]], previous_variables[arguments[2]]);
		} break;
		case LOGICAL_NOT: {
			assert(arguments.size() == 1u);
			return logical_not(previous_variables[arguments[0]]);
		} break;
		case LEQ: {
			assert(arguments.size() == 2u);
			return less_equal(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		case LESS_THAN: {
			assert(arguments.size() == 2u);
			return less_than(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		case EQUAL_TO: {
			assert(arguments.size() == 2u);
			return equal(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		case NEQ: {
			assert(arguments.size() == 2u);
			return not_equal(previous_variables[arguments[0]], previous_variables[arguments[1]]);
		} break;
		default: assert(false);
	}
}

std::string to_lowercase(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(),
	               [](unsigned char c){ return std::tolower(c); });
	return str;

}

value parse_number(const parsed_expression &expression) {
	//TODO: decide what to do with leading zeros
	//TODO: make it raise on overflow
	if (expression.is_token()) {
		try {
			static_assert(std::is_same<value, int>::value, "Use std::stol/std::stoll for long/long long value type");
			std::size_t pos;
			value v = std::stoi(expression.value, &pos);
			assert(pos == expression.value.size());
			return v;
		}
		catch (const std::invalid_argument &) {
			throw parse_error{"Expected integer, got " + expression.value};
		}
		catch (const std::out_of_range &) {
			throw parse_error{"Integer out of range: " + expression.value};
		}
	}
	else {
		throw parse_error{"NOT IMPLEMENTED YET"};
		//RUN THROUGH ALL THOSE FUCKING OPERATORS 😨
	}
}

std::vector <std::pair <std::string, int> > parse_string_with_weights(const parsed_expression &expression, bool allow_negation) {
	if (expression.is_token()) {
		return {{expression.value, 1}};
	}
	else if (expression.value == "!") {
		if (allow_negation) {
			assert(expression.sub_expressions.size() == 1);
			if (!expression.sub_expressions[0].is_token()) {
				throw parse_error{"! expected to get a token, got compound expression"};
			}
			return {{"!" + expression.sub_expressions[0].value, 1}};
		}
		else {
			throw parse_error{"Negation not allowed in this context"}; //TODO: better error message
		}
	}
	else if (expression.value == "+") {
		assert(expression.sub_expressions.size() == 2u);
		std::vector <std::pair <std::string, int> > LHS = parse_string_with_weights(expression.sub_expressions[0], allow_negation);
		std::vector <std::pair <std::string, int> > RHS = parse_string_with_weights(expression.sub_expressions[1], allow_negation);
		LHS.insert(LHS.end(), RHS.begin(), RHS.end());
		return LHS;
	}
	else if (expression.value == "-") {
		assert(expression.sub_expressions.size() == 2u);
		std::vector <std::pair <std::string, int> > LHS = parse_string_with_weights(expression.sub_expressions[0], allow_negation);
		std::vector <std::pair <std::string, int> > RHS = parse_string_with_weights(expression.sub_expressions[1], allow_negation);
		for (auto &[token, coefficient] : RHS) LHS.emplace_back(token, -coefficient);
		return LHS;
	}
	else if (expression.value == "*") {
		assert(expression.sub_expressions.size() == 2u);
		std::optional<value> LHS, RHS;
		std::string LHS_error, RHS_error;
		try {
			LHS = parse_number(expression.sub_expressions[0]);
		}
		catch (parse_error e) {
			LHS_error = e.content;
		}
		try {
			RHS = parse_number(expression.sub_expressions[1]);
		}
		catch (parse_error e) {
			RHS_error = e.content;
		}
		if (LHS.has_value() && RHS.has_value()) {
			throw parse_error{"Expected number * string or string * number, got number * number"};
		}
		if (!LHS.has_value() && !RHS.has_value()) {
			throw parse_error{"Expected number * string or string * number, got \"" + LHS_error + "\" * \"" + RHS_error + "\""};
		}
		if (LHS.has_value()) {
			std::vector <std::pair <std::string, int> > RHS_weights = parse_string_with_weights(expression.sub_expressions[1], allow_negation);
			for (auto &[token, coefficient] : RHS_weights) {
				coefficient *= *LHS; //TODO: raise if overflow
			}
			return RHS_weights;
		}
		else {
			std::vector <std::pair <std::string, int> > LHS_weights = parse_string_with_weights(expression.sub_expressions[0], allow_negation);
			for (auto &[token, coefficient] : LHS_weights) {
				coefficient *= *RHS; //TODO: raise if overflow
			}
			return LHS_weights;
		}
	}
	else {
		throw parse_error{"Operator " + expression.value + " not allowed in this context"};
	}
}



player_weights expression_compiler::parse_players(const parsed_expression &subexpression) {
	static const std::map <std::string, player_weights> known_players = {
		{"north", NORTH_WEIGHTS}, {"n", NORTH_WEIGHTS},
		{"south", SOUTH_WEIGHTS}, {"s", SOUTH_WEIGHTS},
		{"west", WEST_WEIGHTS}, {"w", WEST_WEIGHTS},
		{"east", EAST_WEIGHTS}, {"e", EAST_WEIGHTS},
		{"ns", NS_WEIGHTS}, {"sn", NS_WEIGHTS},
		{"ew", EW_WEIGHTS}, {"we", EW_WEIGHTS},
	};
	auto extract_weights = [](const std::string &x) {
		std::string x_lower = to_lowercase(x);
		auto it = known_players.find(x_lower);
		if (it == known_players.end()) {
			throw parse_error{"Unknown player(s) " + x};
		}
		return it->second;
	};
	player_weights ret = ZERO_PLAYER_WEIGHTS;
	std::vector <std::pair <std::string, value> > coefficients = parse_string_with_weights(subexpression, false);
	for (auto [token, coefficient] : coefficients) {
		ret = ret + extract_weights(token) * coefficient; //TODO: raise if overflow
																											//TODO: this can be made into (or decide not to do so, instead function with _safe in it's name
	}
	return ret;
}

std::optional<suit_weights> try_parse_suit_weights(const std::string &x) {
	static const std::map <std::string, suit_weights> known_suits = {
		{"spade", SPADES_WEIGHTS},
		{"heart", HEARTS_WEIGHTS},
		{"diamond", DIAMONDS_WEIGHTS},
		{"club", CLUBS_WEIGHTS},
		{"major", SPADES_WEIGHTS + HEARTS_WEIGHTS},
		{"minor", DIAMONDS_WEIGHTS + CLUBS_WEIGHTS},
		{"red", HEARTS_WEIGHTS + DIAMONDS_WEIGHTS},
		{"black", SPADES_WEIGHTS + CLUBS_WEIGHTS},
		{"round", HEARTS_WEIGHTS + CLUBS_WEIGHTS},
		{"pointed", SPADES_WEIGHTS + DIAMONDS_WEIGHTS},
	};
	std::string x_lower = to_lowercase(x);
	{
		auto it = known_suits.find(x_lower);
		if (it != known_suits.end()) return it->second;
	}
	if (x_lower.back() == 's') {
		auto it = known_suits.find(x_lower.substr(0, x_lower.size() - 1));
		if (it != known_suits.end()) return it->second;
	}
	return std::nullopt;
}

std::map<std::string, rank_weights> generate_known_ranks() {
	std::map<std::string, rank_weights> ret = {
		{"jack", JACKS_WEIGHTS},
		{"j", JACKS_WEIGHTS},
		{"queen", QUEENS_WEIGHTS},
		{"q", QUEENS_WEIGHTS},
		{"king", KINGS_WEIGHTS},
		{"k", KINGS_WEIGHTS},
		{"ace", ACES_WEIGHTS},
		{"a", ACES_WEIGHTS}, //TODO: something more robust (including tens_plus or anything.....)
	};
	for (size_t rank = 0; rank < RANKS; ++rank) {
		ret[std::string(1, tolower(RANK_SYMBOLS[rank]))] = make_singleton_weight<RANKS>(rank);
	}
	return ret;
}

rank_weights expression_compiler::parse_rank(const parsed_expression &subexpression) {
	static const std::map<std::string, rank_weights> known_ranks = generate_known_ranks();;
	auto extract_weights = [](const std::string &x) {
		std::string x_lower = to_lowercase(x);
		auto it = known_ranks.find(x_lower);
		if (it == known_ranks.end()) {
			throw parse_error{"Unknown rank(s) " + x};
		}
		return it->second;
	};
	assert(subexpression.is_token());
	return extract_weights(subexpression.value);
}

suit_weights expression_compiler::parse_suits(const parsed_expression &subexpression) {
	static const std::map <std::string, suit_weights> known_suits = { //TODO: avoid copy-pasting, use above function instead
		{"spade", SPADES_WEIGHTS},
		{"heart", HEARTS_WEIGHTS},
		{"diamond", DIAMONDS_WEIGHTS},
		{"club", CLUBS_WEIGHTS},
		{"major", SPADES_WEIGHTS + HEARTS_WEIGHTS},
		{"minor", DIAMONDS_WEIGHTS + CLUBS_WEIGHTS},
		{"red", HEARTS_WEIGHTS + DIAMONDS_WEIGHTS},
		{"black", SPADES_WEIGHTS + CLUBS_WEIGHTS},
		{"round", HEARTS_WEIGHTS + CLUBS_WEIGHTS},
		{"pointed", SPADES_WEIGHTS + DIAMONDS_WEIGHTS},
	};
	auto extract_weights = [](const std::string &x) {
		std::string x_lower = to_lowercase(x);
		bool complement = false;
		if (!x_lower.empty() && x_lower[0] == '!') {
			x_lower = x_lower.substr(1);
			complement = true;
		}
		{
			auto it = known_suits.find(x_lower);
			if (it != known_suits.end()) return complement ? ALL_SUITS - it->second : it->second;
		}
		if (x_lower.back() == 's') {
			auto it = known_suits.find(x_lower.substr(0, x_lower.size() - 1));
			if (it != known_suits.end()) return complement ? ALL_SUITS - it->second : it->second;
		}
		throw parse_error{"Unknown suit " + x};
	};
	suit_weights ret = ZERO_SUIT_WEIGHTS;
	std::vector <std::pair <std::string, value> > coefficients = parse_string_with_weights(subexpression, true);
	for (auto [token, coefficient] : coefficients) {
		ret = ret + extract_weights(token) * coefficient; //TODO: raise if overflow
																											//TODO: this can be made into (or decide not to do so, instead function with _safe in it's name
	}
	return ret;
}
std::pair<partial_expression_part::argument_type, size_t> expression_compiler::run_recursive(const parsed_expression &subexpression) {
	if (subexpression.is_token()) {
		value x = parse_number(subexpression);
		return {partial_expression_part::argument_type::constant, this->add_used_constant(x)};
	}
	else if (subexpression.is_function()) {
		{
			std::optional<suit_weights> suits = try_parse_suit_weights(subexpression.value);
			if (suits.has_value()) {
				assert(subexpression.sub_expressions.size() == 1u); //TODO: throw parse_error instead
				player_weights players = parse_players(subexpression.sub_expressions[0]);
				card_player_matrix this_weights = full_product(players, *suits, ALL_RANKS);
				return {partial_expression_part::argument_type::input_variable, this->add_input_variable(this_weights)};
			}
		}
		if (subexpression.value == "hcp") { //TODO: case insensitive
			assert(subexpression.sub_expressions.size() == 1u || subexpression.sub_expressions.size() == 2u); //TODO: support hcp(player, suits)
			suit_weights suits = subexpression.sub_expressions.size() == 2u ? parse_suits(subexpression.sub_expressions[1]) : ALL_SUITS;
			player_weights players = parse_players(subexpression.sub_expressions[0]);
			card_player_matrix this_weights = full_product(players, suits, HCP_WEIGHTS); //TODO: this block is repetitive, compress it somehow (function/macro/whatever is better)
			return {partial_expression_part::argument_type::input_variable, this->add_input_variable(this_weights)};
		}
		if (subexpression.value == "has") { //TODO: case insensitive
			assert(subexpression.sub_expressions.size() == 3u); 
			player_weights players = parse_players(subexpression.sub_expressions[0]);
			suit_weights suits = parse_suits(subexpression.sub_expressions[1]); //Possibly allow both suit rank and rank suit (or even all 3! orders, because why not)
			rank_weights ranks = parse_rank(subexpression.sub_expressions[2]);
			card_player_matrix this_weights = full_product(players, suits, ranks); //TODO: this block is repetitive, compress it somehow (function/macro/whatever is better)
			size_t this_index = input_variables.size();
			input_variables.push_back(this_weights);
			return {partial_expression_part::argument_type::input_variable, this_index};
		}
	}
	else if (subexpression.is_operator()) {
		size_t operation_type;
		bool flip = false;
		//TODO: unary minus and plus
		if (subexpression.value == "+") operation_type = ADD;
		else if (subexpression.value == "-") operation_type = SUBTRACT;
		else if (subexpression.value == "*") operation_type = MULTIPLY;
		else if (subexpression.value == "/") operation_type = DIVIDE;
		else if (subexpression.value == "&&") operation_type = LOGICAL_AND;
		else if (subexpression.value == "||") operation_type = LOGICAL_OR;
		else if (subexpression.value == "^^") operation_type = LOGICAL_XOR;
		else if (subexpression.value == "!") operation_type = LOGICAL_NOT;
		else if (subexpression.value == "?:") operation_type = TERNARY;
		else if (subexpression.value == "<=") operation_type = LEQ;
		else if (subexpression.value == ">=") {operation_type = LEQ; flip = true;}
		else if (subexpression.value == "<") operation_type = LESS_THAN;
		else if (subexpression.value == ">") {operation_type = LESS_THAN; flip = true;}
		else if (subexpression.value == "==") operation_type = EQUAL_TO;
		else if (subexpression.value == "==") operation_type = NEQ;
		else assert(false); //Are there any other operators left (?)
		std::vector<std::pair <partial_expression_part::argument_type, size_t> > compiled_subexpressions;
		for (auto &sub : subexpression.sub_expressions) {
			compiled_subexpressions.emplace_back(run_recursive(sub));
		}
		if (flip) {
			assert(compiled_subexpressions.size() == 2u);
			swap(compiled_subexpressions[0], compiled_subexpressions[1]);
		}
		size_t this_index = subexpressions.size();
		subexpressions.push_back({operation_type, compiled_subexpressions});
		return {partial_expression_part::argument_type::other_expression, this_index};
	}
	else if (subexpression.is_chained_comparison()) {
		assert(false); //Not implemented yet
	}
	else {
		assert(false); //UNKNOWN subexpression type
	}
	throw "NOT IMPLEMENTED YET";
}
compiled_expression expression_compiler::finalize(std::pair<partial_expression_part::argument_type, size_t> final_id) {
	auto convert_to_index = [&](std::pair<partial_expression_part::argument_type, size_t> x) -> size_t {
		switch(x.first) {
			case partial_expression_part::argument_type::constant: return x.second;
			case partial_expression_part::argument_type::input_variable: return used_constants.size() + x.second;
			case partial_expression_part::argument_type::other_expression: return used_constants.size() + input_variables.size() + x.second;
			default: assert(false);
		}
	};
	compiled_expression result;
	result.used_constants = used_constants; //std::move() ?
	result.input_variables = input_variables;
	for (auto &subexpression : subexpressions) {
		expression_part processed_expression_part;
		processed_expression_part.type = subexpression.type; //TODO: treat constant parameters differently (e.g kth_element)
		for (auto &x : subexpression.arguments) processed_expression_part.arguments.push_back(convert_to_index(x));
		result.parts.push_back(processed_expression_part);
	}
	return result;
}
compiled_expression expression_compiler::compile(const parsed_expression &expression) {
	return finalize(run_recursive(expression));
}

compiled_expression compile_expression(const parsed_expression &expression) {
	expression_compiler parser;
	return parser.compile(expression);
}
//TODO: test DP with some expressions without any compound part, just one input variable/one constant
dp_state compiled_expression::make_initial_state() const {
	std::vector<range> values;
	for (value x : used_constants) values.push_back(singleton(x));
	for (auto input_variable : input_variables) {
		values.push_back(process_input_variable(input_variable, full_deck()).content[HAND_SIZE][HAND_SIZE][HAND_SIZE][HAND_SIZE]);
	}
	for (auto &expression_part : this->parts) {
		values.push_back(expression_part.eval(values));
	}
	// std::cerr << values << "\n";
	std::vector <bool> relevant(values.size()); //TODO: refactor this to not contain constants
	relevant.back() = true;
	size_t offset = used_constants.size() + input_variables.size(); //Make one function to get the offset, rather than compute it every time explicitely
	for (int i = (int)parts.size() - 1; i >= 0; --i) {
		if (relevant[offset + i] && !is_singleton(values[offset + i])) {
			for (size_t x : parts[i].arguments) { //TODO: this would break for kth and other stuff which doesn't just have references to previous elements, but also constants
				relevant[x] = true;
			}
		}
	}
	// std::cerr << "relevant = " << relevant << "\n";
	partial_evaluation ret(values.size());
	for (size_t i = 0; i < used_constants.size(); ++i) ret[i] = std::nullopt;
	for (size_t i = 0; i < input_variables.size(); ++i) ret[i + used_constants.size()] = relevant[i + used_constants.size()] ? std::optional<value>(input_variables[i].offset) : std::nullopt;
	for (size_t i = used_constants.size() + input_variables.size(); i < values.size(); ++i) {
		if (relevant[i] && is_singleton(values[i])) ret[i] = values[i].first; //TODO: wrap this get_first into any member function (or just extract_only_value which raises if not singleton (?))
		else ret[i] = std::nullopt;
	}
	return {ret, std::array<value, PLAYERS>()};
}



size_t expression_compiler::add_used_constant(value x) {
	for (size_t i = 0; i < used_constants.size(); ++i) {
		if (used_constants[i] == x) {
			return i;
		}
	}
	size_t new_index = used_constants.size();
	used_constants.push_back(x);
	return new_index;
}
size_t expression_compiler::add_input_variable(card_player_matrix x) {
	for (size_t i = 0; i < input_variables.size(); ++i) {
		if (input_variables[i] == x) {
			return i;
		}
	}
	size_t new_index = input_variables.size();
	input_variables.push_back(x);
	return new_index;
}


dp_state compiled_expression::append_card(const dp_state &state, size_t card, size_t player, const std::vector<processed_input_variable>&preprocessed_variables) const {
	// std::cerr << "Try append to " << state << ", give " << card_to_str(card) << " to " << PLAYERS_STR[player] << "\n";
	auto &[current_eval, current_count] = state;
	std::array<int, PLAYERS> new_count = current_count;
	new_count[player]++;
	std::vector<range> values;
	partial_evaluation ret(used_constants.size() + input_variables.size() + parts.size());
	for (size_t i = 0; i < used_constants.size(); ++i) {
		values.push_back(singleton(used_constants[i]));
	}
	for (size_t i = 0; i < input_variables.size(); ++i) {
		if (current_eval[i + used_constants.size()].has_value()) {
			// std::cerr << " variable " << i << " still has value" << std::endl;
			ret[i + used_constants.size()] = (*current_eval[i + used_constants.size()] + input_variables[i].coef[card][player]);
			static_assert(PLAYERS == 4);
			values.push_back(singleton(*ret[i + used_constants.size()]) + preprocessed_variables[i].content[HAND_SIZE - new_count[0]][HAND_SIZE - new_count[1]][HAND_SIZE - new_count[2]][HAND_SIZE - new_count[3]]); //TODO: refactor with ranges overload for int + range
		}
		else {
			// std::cerr << " variable " << i << " is null" << std::endl;
			values.push_back(singleton(0)); //Irrelevant placeholder, it won't be read anyway
		}
	}
	size_t offset = used_constants.size() + input_variables.size(); //Make one function to get the offset, rather than compute it every time explicitely
	for (size_t i = 0; i < parts.size(); ++i) {
		if (current_eval[i + offset].has_value()) values.push_back(singleton(*current_eval[i + offset]));
		else values.push_back(parts[i].eval(values));
	}
	std::vector <bool> relevant(values.size()); //TODO: refactor this to not contain constants
	relevant.back() = true;
	for (int i = (int)parts.size() - 1; i >= 0; --i) {
		if (relevant[offset + i] && !is_singleton(values[offset + i])) {
			for (size_t x : parts[i].arguments) { //TODO: this would break for kth and other stuff which doesn't just have references to previous elements, but also constants
				relevant[x] = true;
			}
		}
	}
	// std::cerr << "values = " << values << "\n";
	// std::cerr << "relevant = " << relevant << "\n";
	for (size_t i = 0; i < used_constants.size(); ++i) ret[i] = std::nullopt;
	for (size_t i = 0; i < input_variables.size(); ++i) {
		if (!relevant[i + used_constants.size()]) ret[i + used_constants.size()] = std::nullopt;
	}
	for (size_t i = used_constants.size() + input_variables.size(); i < values.size(); ++i) {
		if (relevant[i] && is_singleton(values[i])) ret[i] = values[i].first; //TODO: wrap this get_first into any member function (or just extract_only_value which raises if not singleton (?))
		else ret[i] = std::nullopt;
	}
	// std::cerr << "Try append to " << state << ", give " << card_to_str(card) << " to " << PLAYERS_STR[player] << ", got " << ret << ", " << new_count << "\n";
	bool any_non_null = false;
	for (auto &x : ret) if (x.has_value()) any_non_null = true;
	assert(any_non_null);
	return {ret, new_count};
}

bool can_append_card(const dp_state &dp, size_t player) {
	return dp.second[player] < HAND_SIZE;
}

struct dp_value {
	board_count count;
	std::vector <std::pair<const dp_value *, size_t> > previous;
	void append(const dp_value &x, size_t player) {
		previous.emplace_back(&x, player);
		count += x.count;
	}
};

std::ostream &operator<<(std::ostream &o, const dp_value &x) {
	return o << x.count;
}

#include <random>

using i128 = __int128_t;
using u128 = __uint128_t;

static u128 rand_u128(std::mt19937_64 &rng) {
    u128 hi = rng();
    u128 lo = rng();
    return (hi << 64) | lo;
}

// Uniform __int128 in [a, b)
i128 uniform_i128(i128 a, i128 b, std::mt19937_64 &rng) {
    u128 range = (u128)(b - a);

    // Compute largest multiple of range that fits in 128 bits
    u128 limit = (~u128{0} / range) * range;  // equivalent to floor(2^128 / range) * range

    while (true) {
        u128 r = rand_u128(rng);
        if (r < limit) {
            return (i128)(r % range) + a;
        }
    }
}


std::pair<board_count, std::vector<board> > compiled_expression::run_dp(const size_t count) const {
	std::vector<std::map <dp_state, dp_value> > dp(DECK_SIZE + 1); //TODO: after a run is done I only need values, memory used to store keys is wasted, store values elsewhere (just in a vector (?))
	dp[0][make_initial_state()] = {1, {}};
	std::bitset<DECK_SIZE> still_undealt = full_deck();
	std::vector <size_t> cards;
	for (size_t i = 0; i < DECK_SIZE; ++i) {
		size_t limit = std::numeric_limits<size_t>::max();
		size_t best_card = std::numeric_limits<size_t>::max();
		auto append_advance_dp = [&](size_t card) -> void {
			auto &current_dp = dp[i];
			std::map<dp_state, dp_value> new_dp;
			std::bitset<DECK_SIZE> new_undealt = still_undealt;
			new_undealt[card] = 0;
			std::vector<processed_input_variable> processed_variables;
			for (auto &input_variable : input_variables) processed_variables.push_back(process_input_variable(input_variable, new_undealt));
			for (auto &[state, count] : current_dp) {
				for (size_t player = 0; player < PLAYERS; ++player) {
					if (can_append_card(state, player)) {
						new_dp[append_card(state, card, player, processed_variables)].append(count, player); //TODO: filter out boards with final output already decided to be something
						if (new_dp.size() >= limit) return;
					}
				}
			}
			assert(new_dp.size() <= limit);
			dp[i + 1] = std::move(new_dp);
			limit = dp[i + 1].size();
			best_card = card;
		};
		for (size_t card = 0; card < DECK_SIZE; ++card)
			if (still_undealt[card])
				append_advance_dp(card);
		cards.push_back(best_card);
		still_undealt[best_card] = false;
		std::cerr << "Chose to add " << card_to_str(best_card) << " leading to dp.size() = " << dp[i + 1].size() << "\n";
	}
	// std::cerr << "dp = " << dp << "\n";
	// std::cerr << "total_size = " << total_size << "\n";
	std::mt19937_64 rng(0);
	std::vector<board> boards;
	size_t BLOCK_SIZE = 6;
	auto dump = [&]() { //TODO: make this into a "normal" function
		std::vector<std::string> content;
		for (auto &b : boards) {
			std::stringstream s;
			b.output(s);
			content.push_back(s.str());
		}
		std::vector<size_t> indices(content.size());
		while (true) {
			bool any = false;
			for (size_t i = 0; i < content.size(); ++i) {
				if (indices[i] >= content[i].size()) continue;
				while (indices[i] < content[i].size() && content[i][indices[i]] != '\n') {
					std::cerr << content[i][indices[i]];
					indices[i]++;
					any = true;
				}
				std::cerr << "| ";
				indices[i]++;
			}
			std::cerr << "\n";
			if (!any) break;
		}
		for (int _ = 0; _ < 245; ++_)
			std::cerr << "-";
		std::cerr << "\n";
	};
	board_count matching_cou = 0;
	const dp_value * initial_pos = nullptr;
	for (auto &[state, value] : dp.back()) {
		if (*state.first.back() == 1) {
			initial_pos = &value;
			matching_cou = value.count;
		}
	}
	std::vector<board> found_boards; //TODO: rename to avoid this issue with boards and found_boards;
	for (size_t _ = 0; _ < count; ++_) {
		const dp_value *pos = initial_pos;
		partial_board b;
		if (pos == nullptr) {
			std::cerr << "Conditions are contradictive\n";
			return {0, {}};
		}
		for (int i = DECK_SIZE - 1; i >= 0; --i) {
			size_t card = cards[i];
			i128 current_count = pos->count;
			i128 x = uniform_i128(0, current_count, rng);
			size_t who = -1;
			for (auto &[previous_node, player] : pos->previous) {
				if (x < previous_node->count) {
					pos = previous_node;
					who = player;
					break;
				}
				else {
					x -= previous_node->count;
				}
			}
			b.who[card] = who;
		}
		boards.push_back(b.finalize());
		found_boards.push_back(b.finalize());
		assert(eval(b.finalize()) == 1);
		if (boards.size() == BLOCK_SIZE) {
			dump();
			boards.clear();
		}
	}
	if (!boards.empty()) dump();
	std::cerr << "(matching = " << matching_cou << ") / (all_bords_count = 53644737765488792839237440000) = " << matching_cou / (long double) 53644737765488792839237440000.0L;
	return {matching_cou, found_boards};
}
