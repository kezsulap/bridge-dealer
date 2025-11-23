#include "expression.hpp"
#include "types.hpp"
#include <cstring>
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

value compiled_expression::eval(const board &b) const {
	std::vector <value> values;
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
		default: assert(false);
	}
	return 0;
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
suit_weights expression_compiler::parse_suits(const parsed_expression &subexpression) {
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
std::pair<partial_expression_part::argument_type, size_t> expression_compiler::run_recursive(const parsed_expression &/*subexpression*/) {
	throw "NOT IMPLEMENTED YET";
}
compiled_expression expression_compiler::finalize() {
	throw "NOT IMPLEMENTED YET";
// compiled_expression result;
// result.input_variables = input_variables;
// for (auto &subexpression : subexpressions) {
	// expression_part processed_expression_part;
	// processed_expression_part.type = subexpression.type;
	// result.parts.push_back(processed_expression_part);
// }
}
compiled_expression expression_compiler::compile(const parsed_expression &expression) {
	run_recursive(expression);
	return finalize();
}

compiled_expression compile_expression(const parsed_expression &expression) {
	//TODO:
	//Identify any common parts and extract them into a shared card_player_matrix

	expression_compiler parser;
	parser.run_recursive(expression);
	return parser.finalize();
}
