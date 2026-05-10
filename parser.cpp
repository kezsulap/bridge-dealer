#include "parser.hpp"
#include "output_operators.hpp"
#include <cassert>
#include <cctype>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
/*
variable make_card(int suit, int rank, int player) {
	variable res;
	res[make_card_index(suit, rank)][player] = 1;
	return res;
}
int make_card_index(int suit, int rank) {
	assert(suit >= 0 && suit < SUITS && rank >= 0 && rank < RANKS);
	return suit * 13 + rank;
}
variable make_suit_length(int suit, int player) {
	variable res;
	for (int rank = 0; rank < RANKS; ++rank) res[make_card_index(suit, rank)][player] = 1;
	return res;
}
variable make_hcp(int player, int suit = -1) {
	variable res;
	for (int rank = JACK; rank <= ACE; ++rank)
		for (int s = (suit == -1 ? 0 : suit); s <= (suit == -1 ? SUITS - 1 : suit); ++s)
			res[make_card_index(rank, s)][player] = ();
	return res;
}
variable make_rank(int rank, int player) {
	
}
*/
std::string join(const std::string &separator, const std::vector <std::string> &rest) {
	std::stringstream out;
	for (size_t i = 0; i < rest.size(); ++i) {
		if (i) out << separator;
		out << rest[i];
	}
	return out.str();
}
/*
//Remember to support hcp(north + south), hcp(NS), hearts(NS), hcp(north, clubs + diamonds)
//TODO: What to do with shape(north, any 5332 + 55xx - any 0xxx) etc?
variable make_constant(value x) {
	variable res;
	res.offset = x;
	return res;
};
variable suit_length(const std::vector <string> &arguments, int suit) {
	
}
array<value, PLAYERS> parse_players(const string &arguments) {
	array<value, PLAYERS> res = {};
	if (arguments.empty()) {
		throw parse_error("Missing player id");
	}
	if (arguments.size() >= 2u) {
		throw parse_error("Missing player" + join(", "arguments));
	}
	return res;
}
variable make_suit_length(const std::vector <string> &arguments, int suit, const string &name) {
	if (arguments.size() != 1u) {
		throw parse_error {name + " requires one argument rather than \"" + join(", ", arguments) + "\""};
	}
	players = parse_players(arguments[0]);
	variable res;
	for (int i = 0; i < PLAYERS; ++i)
		res = res + players[i];
	return res;
}
*/

// ()	Function call
// + -	Unary plus and minus
// ! Logical NOT
// * / %	Multiplication, division, and remainder	Left-to-right
// + -	Addition and subtraction
// < <=	For relational operators < and ≤ respectively
// > >=	For relational operators > and ≥ respectively
// == !=	For relational = and ≠ respectively
// &&	Logical AND
// ||	Logical OR
// ?:	Ternary conditional[note 3]	Right-to-left

//Deliberately not including >>, <<, |, ^, &, ~ bit operations

std::ostream& operator<<(std::ostream &o, const parsed_expression &x) {
	return o << x.value << x.sub_expressions;
}

bool is_in(const std::string &x, const std::vector<std::string> content) {
	for (auto &c : content) if (c == x) return true;
	return false;
}

bool is_valid_identifier(const std::string &s) {
	for (char c : s) if (!isalnum(c) && c != '_') return false;
	return true;
}
bool is_operator(const std::string &x) {
	return is_in(x, {"*", "/", "%", "+", "-", "==", "<", ">", "<=", ">=", "!=", "&&", "||", "^^", "!"});
}

bool parsed_expression::operator==(const parsed_expression &oth) const {
	return value == oth.value && sub_expressions == oth.sub_expressions;
}
bool parsed_expression::is_token() const {
	return sub_expressions.empty();
}
bool parsed_expression::is_operator() const {
	return ::is_operator(value);
}
bool parsed_expression::is_function() const {
	return !is_chained_comparison() && !is_token() && !is_operator();
}
bool parsed_expression::is_chained_comparison() const {
	return value.empty(); //TODO: make this cleaner then this ugly hack
}
bool parsed_expression::operator!=(const parsed_expression &oth) const {
	return !(*this == oth);
}

//Shape syntax: any series of tokens (whether space separated or not, whatever xd) including digits, x, [, ], ., +, -, will be considered a shape
//any 5332 | 6+M4xx !2614

bool is_valid_character_within_shapelist(char x) {
	return std::isdigit(x) || x == 'm' || x == 'M' || x == '+' || x == '-' || x == '[' || x == ']';
}

parsed_expression parse_tokenized_expression(const std::vector <std::string> &tokens) {
	assert(!tokens.empty());
	using iterator = std::vector<std::string>::const_iterator;
	auto parse_shapelist_rec = [](iterator begin, iterator end, auto parse_shapelist) -> parsed_expression {
		assert(begin != end);
		iterator split_point = end;
		int parenthesis_depth = 0;
		for (auto it = begin; it != end; ++it) {
			if (*it == "(") parenthesis_depth++;
			else if (*it == ")") parenthesis_depth--;
			if (parenthesis_depth == 0 && (*it == "|" || *it == "!") && split_point == end) {
				split_point = it;
			}
		}
		assert(parenthesis_depth == 0); //Parenthesis depths should already be checked within rec_parse
		if (split_point != end) {
			return parsed_expression{*split_point, {parse_shapelist(begin, split_point, parse_shapelist), parse_shapelist(split_point + 1, end, parse_shapelist)}};
		}
		if (*begin == "any") {
			if (begin + 1 == end) throw parse_error{"missing shape after any"};
			return parsed_expression{"any", {parse_shapelist(begin + 1, end, parse_shapelist)}};
		}
		if (*begin == "(" && *(end - 1) == ")") return parse_shapelist(begin + 1, end - 1, parse_shapelist); //TODO: what will happen with (x)(y)?
		std::stringstream s;
		for (iterator it = begin; it != end; ++it) {
			for (char c : *it) if (!is_valid_character_within_shapelist(c)) throw parse_error{"Invalid character " + std::string(1, c) + " within shape list"};
			s << *it;
		}
		return parsed_expression{s.str(), {}};
	};
	auto parse_shapelist = [parse_shapelist_rec](iterator begin, iterator end) -> parsed_expression {
		return parse_shapelist_rec(begin, end, parse_shapelist_rec);
	};
	auto rec_parse_ = [&parse_shapelist](iterator begin, iterator end, auto rec_parse) -> parsed_expression {
		if (begin == end) throw parse_error{"Missing expression"};
		assert(begin < end);
#define rec_parse(...) rec_parse(__VA_ARGS__, rec_parse)
		constexpr int NONE_PRIORITY = -1;
		constexpr int MULTIPLICATION_DIVISION_MODULO_PRIORITY = 0;
		constexpr int ADDITION_SUBTRACTION_PRIORITY = 1;
		constexpr int COMPARISON_PRIORITY = 2;
		constexpr int LOGICAL_AND_PRIORITY = 3;
		constexpr int LOGICAL_OR_PRIORITY = 4;
		constexpr int LOGICAL_XOR_PRIORITY = 5;
		int parenthesis_depth = 0, ternary_operator_depth = 0;
		std::stack<std::string> parenthesis_and_ternary_stack;
		iterator ternary_first = end, ternary_second = end;
		std::vector<iterator> indices;
		int highest_priority = NONE_PRIORITY;
		for (auto it = begin; it != end; ++it) {
			if (*it == "(") {
				parenthesis_and_ternary_stack.push("(");
				parenthesis_depth++;
			}
			else if (*it == ")") {
				if (parenthesis_and_ternary_stack.empty()) throw parse_error{"Mismatched parenthesis"};
				if (parenthesis_and_ternary_stack.top() == "?") throw parse_error{"Expecting : before )"};
				parenthesis_and_ternary_stack.pop();
				parenthesis_depth--;
			}
			else if (*it == "?") {
				parenthesis_and_ternary_stack.push("?");
				ternary_operator_depth++;
			}
			else if (*it == ":") {
				if (parenthesis_and_ternary_stack.empty() || parenthesis_and_ternary_stack.top() == "(") throw parse_error{": without prior ?"};
				parenthesis_and_ternary_stack.pop();
				ternary_operator_depth--;
			}
			// std::cerr << *it << " at depth = " << parenthesis_depth << ", " << ternary_operator_depth << "\n";
			if (parenthesis_depth) continue;
			if (ternary_operator_depth == 1 && *it == "?" && ternary_first == end) ternary_first = it;
			if (ternary_operator_depth == 0 && *it == ":" && ternary_second == end) ternary_second = it;
			if (ternary_operator_depth) continue;
			int this_operator_priority = NONE_PRIORITY;
			bool this_multi_operator = false;
			if (is_in(*it, {"*", "/", "%"})) {
				this_operator_priority = MULTIPLICATION_DIVISION_MODULO_PRIORITY;
			} else if (is_in(*it, {"+", "-"})) {
				if (it != begin && !is_operator(*(it - 1))) {
					this_operator_priority = ADDITION_SUBTRACTION_PRIORITY;
				}
			} else if (is_in(*it, {"==", "<", ">", "<=", ">=", "!="})) {
				this_operator_priority = COMPARISON_PRIORITY;
				this_multi_operator = true;
			} else if (*it == "&&") {
				this_operator_priority = LOGICAL_AND_PRIORITY;
			} else if (*it == "||") {
				this_operator_priority = LOGICAL_OR_PRIORITY;
			} else if (*it == "^^") {
				this_operator_priority = LOGICAL_XOR_PRIORITY;
			}
			// std::cerr << "*it = " << *it << ", " << this_operator_priority << "\n";
			if (this_operator_priority != NONE_PRIORITY) {
				if (this_operator_priority > highest_priority) {
					highest_priority = this_operator_priority;
					indices = {it};
				}
				else if (this_operator_priority == highest_priority && this_multi_operator) {
					indices.push_back(it);
				}
			}
		}
		if (parenthesis_depth) throw parse_error{"Mismatched parenthesis"};
		if (ternary_operator_depth) throw parse_error{"Ternary operator ?: missing the : part"};
		// std::cerr << "ternary_first = " << (ternary_first == end ? "---" : *ternary_first) << ", ternary_second = " << (ternary_second == end ? "---" : *ternary_second) << std::endl; 
		if (ternary_first != end) {
			assert(ternary_second != end);
			return parsed_expression{"?:", {
					rec_parse(begin, ternary_first),
					rec_parse(ternary_first + 1, ternary_second),
					rec_parse(ternary_second + 1, end)
				}
			};
		}
		// std::cerr << "highest_priority = " << highest_priority << "\n";
		if (ternary_operator_depth != 0) throw parse_error{"Mismatched ? and : operators"};
		if (parenthesis_depth != 0) throw parse_error{"Mismatched parenthesis"};
		if (highest_priority != NONE_PRIORITY) {
			// std::cerr << "found operator " << *indices[0] << "\n";
			assert(!indices.empty());
			if (indices.size() == 1u)
				return {*indices[0], {rec_parse(begin, indices[0]), rec_parse(indices[0] + 1, end)}};
			std::vector<parsed_expression> sub_expressions;
			sub_expressions.push_back(rec_parse(begin, indices[0]));
			for (size_t i = 0; i < indices.size(); ++i) {
				sub_expressions.push_back(parsed_expression{*indices[i], {}});
				sub_expressions.push_back(rec_parse(indices[i] + 1, i + 1 == indices.size() ? end : indices[i + 1]));
			}
			return {"", sub_expressions};
		}
		if (end - begin == 1) {
			if (!is_valid_identifier(*begin)) throw parse_error{"Invalid identifier: " + *begin};
			return parsed_expression{*begin, {}};
		}
		if (*begin == "(" && *(end - 1) == ")") { //TODO: Some cleaner error message on (x)(y) or anything alike
			return rec_parse(begin + 1, end - 1);
		}
		if (is_valid_identifier(*begin) && *(begin + 1) == "(" && *(end - 1) == ")") {
			std::vector <iterator> commas;
			int depth = 0;
			for (auto it = begin; it != end; ++it) {
				if (*it == "(") depth++;
				if (*it == ")") depth--;
				if (*it == "," && depth == 1) {
					commas.push_back(it);
				}
			}
			assert(depth == 0); // TODO: does this indeed catch a bug and not invalid input?, If so add some comment or anything
			std::vector <parsed_expression> subexpressions;
			if (*begin == "shape") {
				if (commas.size() != 1) throw parse_error{"function shape expecting 2 parameters, got" + (commas.empty() ? std::to_string((begin + 1 == end) ? 1 : 0) : std::to_string(commas.size() + 1))};
				return parsed_expression{*begin, {rec_parse(begin + 2, commas[0]), parse_shapelist(commas[0] + 1, end - 1)}};
			}
			if (commas.empty()) subexpressions.push_back(rec_parse(begin + 2, end - 1)); //TODO: what will happen when dealing with foo()?
			else {
				subexpressions.push_back(rec_parse(begin + 2, commas[0]));
				for (size_t i = 0; i < commas.size() - 1; ++i)
					subexpressions.push_back(rec_parse(commas[i] + 1, commas[i + 1]));
				subexpressions.push_back(rec_parse(commas.back() + 1, end - 1));
			}
			return parsed_expression{*begin, subexpressions};
		}
		if (is_in(*begin, {"+", "-", "!"})) {
			return parsed_expression{*begin, {rec_parse(begin + 1, end)}};
		}
		throw parse_error{"Can't parse expression at " + join(" ", std::vector<std::string>(begin, end))};
#undef rec_parse
	};
	return rec_parse_(tokens.begin(), tokens.end(), rec_parse_);
}
parsed_expression parse_expression(const std::string &expression) {
	return parse_tokenized_expression(tokenize(expression));
}


const std::vector <std::string> two_character_operators = {"<=", ">=", "==", "!=", "&&", "||", "^^"};
bool is_two_character_operator(char a, char b) {
	for (auto c : two_character_operators) if (c[0] == a && c[1] == b) return true;
	return false;
}
bool is_multicharacter_token_part(char x) {
	return isalnum(x) || x == '_' || x == '[' || x == ']'; //Usage of [] in shapes like [53][41] or 3]5[32
}
std::vector <std::string> tokenize(const std::string &s) {
	std::vector <std::string> ans;
	for (size_t i = 0; i < s.size();) {
		if (isspace(s[i])) {
			i++;
		}
		else if (is_multicharacter_token_part(s[i])) {
			ans.emplace_back();
			while (i < s.size() && is_multicharacter_token_part(s[i])) {
				ans.back().push_back(s[i]);
				i++;
			}
		}
		else {
			ans.emplace_back(1, s[i]);
			i++;
			if (i < s.size() && is_two_character_operator(ans.back().back(), s[i])) {
				ans.back().push_back(s[i]);
				i++;
			}
		}
	}
	return ans;
}
