#include "parser.hpp"
#include <bits/stdc++.h>
using namespace std;
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
string join(const string &separator, const vector <string> &rest) {
	stringstream out;
	for (size_t i = 0; i < rest.size(); ++i) {
		if (i) out << separator;
		out << rest[i];
	}
	return out.str();
}
//Remember to support hcp(north + south), hcp(NS), hearts(NS), hcp(north, clubs + diamonds)
//TODO: What to do with shape(north, any 5332 + 55xx - any 0xxx) etc?
variable make_constant(value x) {
	variable res;
	res.offset = x;
	return res;
};
variable suit_length(const vector <string> &arguments, int suit) {
	
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
variable make_suit_length(const vector <string> &arguments, int suit, const string &name) {
	if (arguments.size() != 1u) {
		throw parse_error {name + " requires one argument rather than \"" + join(", ", arguments) + "\""};
	}
	players = parse_players(arguments[0]);
	variable res;
	for (int i = 0; i < PLAYERS; ++i)
		res = res + players[i];
	return res;
}
struct parse_error {
	string content;
};
*/
const vector <string> two_character_operators = {"<=", ">=", "==", "!=", "&&", "||"};
bool is_two_character_operator(char a, char b) {
	for (auto c : two_character_operators) if (c[0] == a && c[1] == b) return true;
	return false;
}
bool is_alnum_or_underscore(char x) {
	return isalnum(x) || x == '_';
}
vector <string> tokenize(const string &s) {
	vector <string> ans;
	for (size_t i = 0; i < s.size();) {
		if (isspace(s[i])) {
			i++;
		}
		else if (is_alnum_or_underscore(s[i])) {
			ans.emplace_back();
			while (i < s.size() && is_alnum_or_underscore(s[i])) {
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
// int main() {
	// string x;
	// getline(cin, x);
	// vector <string> out = tokenize(x);
	// for (string c : out) cout << c << "\n";
// }
