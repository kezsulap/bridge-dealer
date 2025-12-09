#include "test_utils.hpp"
#include "../expression.hpp"
#include <array>
#include <vector>
#include <climits>

#define PROCESS_VARIABLE_TEST(...) _process_variable_test_impl(#__VA_ARGS__, __LINE__, __VA_ARGS__)

void _process_variable_test_impl(std::string description, int line, std::vector<std::array<value, PLAYERS>> content, value offset, std::vector<int> indices, std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t>, range>> expected_results) {
	card_player_matrix matrix;
	std::bitset<DECK_SIZE> deck_subset;
	for (int x : indices) deck_subset[x] = true;
	assert(indices.size() == content.size());
	for (size_t i = 0; i < indices.size(); ++i) {
		for (size_t j = 0; j < PLAYERS; ++j) {
			matrix.coef[indices[i]][j] = content[i][j];
		}
	}
	matrix.offset = offset;
	processed_input_variable output = process_input_variable(matrix, deck_subset);
	std::vector<std::tuple<std::tuple<size_t, size_t, size_t, size_t>, range, range>> failed_tests;
	for (auto &[input, expected_output] : expected_results) {
		auto [north, east, south, west] = input;
		assert(north + east + south + west == indices.size());
		auto actual_output = output.content[north][east][south][west];
		if (expected_output != actual_output) {
			failed_tests.emplace_back(input, expected_output, actual_output);
		}
	}
	if (!failed_tests.empty()) {
		std::cerr << RED << "TEST FAILURE:\n " << description << "\nAT LINE " << line << "\n";
		for (auto &[input, expected_output, actual_output] : failed_tests) {
			auto [north, east, south, west] = input;
			std::cerr << north << ", " << east << ", " << south << ", " << west << " expected to return " << expected_output << " actually returned " << actual_output << std::endl;
		}
		mark_test_failure();
		std::cerr << CLEAR_COLOURS;
	}
	else {
		std::cerr << GREEN << "TEST PASSED " << description << " AT LINE " << line << std::endl;
	}
}

struct testcase {
	std::string expression;
	std::optional<board_count> count_matching;
	std::vector<std::string> matching_boards;
	std::vector<std::string> non_matching_boards;
};

std::vector <testcase> testcases = {
	#include "test_cases.hpp"
};

int main() {
	PROCESS_VARIABLE_TEST({}, 0, {}, {{{0, 0, 0, 0}, {0, 0}}});
	PROCESS_VARIABLE_TEST({}, 5, {}, {{{0, 0, 0, 0}, {5, 5}}});
	PROCESS_VARIABLE_TEST({}, INT_MAX, {}, {{{0, 0, 0, 0}, {INT_MAX, INT_MAX}}});
	PROCESS_VARIABLE_TEST({}, INT_MIN, {}, {{{0, 0, 0, 0}, {INT_MIN, INT_MIN}}});
	PROCESS_VARIABLE_TEST({{4, 3, 2, 1}}, 0, {0}, {{{1, 0, 0, 0}, {4, 4}}, {{0, 1, 0, 0}, {3, 3}},{{0, 0, 1, 0}, {2, 2}},{{0, 0, 0, 1}, {1, 1}}});
	PROCESS_VARIABLE_TEST({{4, 3, 2, 1}}, 0, {21}, {{{1, 0, 0, 0}, {4, 4}}, {{0, 1, 0, 0}, {3, 3}},{{0, 0, 1, 0}, {2, 2}},{{0, 0, 0, 1}, {1, 1}}});
	PROCESS_VARIABLE_TEST({{4, 3, 2, 1}}, 0, {51}, {{{1, 0, 0, 0}, {4, 4}}, {{0, 1, 0, 0}, {3, 3}},{{0, 0, 1, 0}, {2, 2}},{{0, 0, 0, 1}, {1, 1}}});
	PROCESS_VARIABLE_TEST({{4, 3, 2, 1}, {4, 3, 2, 1}}, 0, {4, 16}, {{{2, 0, 0, 0}, {8, 8}}, {{1, 1, 0, 0}, {7, 7}}, {{1, 0, 1, 0}, {6, 6}}, {{0, 0, 0, 2}, {2, 2}}, {{1, 0, 0, 1}, {5, 5}}});
	PROCESS_VARIABLE_TEST({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}, 0, {4, 16, 18, 51}, {{{4, 0, 0, 0}, {1, 1}}, {{1, 3, 0, 0}, {0, 2}}, {{1, 1, 1, 1}, {0, 4}}, {{1, 1, 2, 0}, {0, 3}}});


	ASSERT_EQUAL(full_product(NORTH_WEIGHTS, ALL_SUITS, HCP_WEIGHTS).eval(board("AKQ.JT9.876.5432 5432.AKQ.JT9.876 876.5432.AKQ.JT9 JT9.876.5432.AKQ")), 10);
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("n")), player_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("N")), player_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("NorTh")), player_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("north")), player_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("s")), player_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("S")), player_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("south")), player_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("South")), player_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("NS")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("SN")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("nS")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("sN")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("NS")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("SN")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("nS")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("sN")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("e")), player_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("East")), player_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("E")), player_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("w")), player_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("W")), player_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("west")), player_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("EW")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("WE")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("eW")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("wE")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("Ew")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("We")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("ew")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("we")), player_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("north + south")), player_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("north + 2 * south")), player_weights({1, 0, 2, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("north - 2 * east")), player_weights({1, -2, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("north - west * 2")), player_weights({1, 0, 0, -2}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("2 * (east - south)")), player_weights({0, 2, -2, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("east - east")), player_weights({0, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_players(parse_expression("0 * (north - south)")), player_weights({0, 0, 0, 0}));

	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("spades")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("hearts")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("diamonds")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("clubs")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("spade")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("heart")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("diamond")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("club")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("SPADES")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("HEARTS")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("DIAMONDS")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("CLUBS")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("SPADE")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("HEART")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("DIAMOND")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("CLUB")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("spAdes")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("Hearts")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("dIaMoNdS")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("cLUBS")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("sPADE")), suit_weights({1, 0, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("Heart")), suit_weights({0, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("Diamond")), suit_weights({0, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("CLub")), suit_weights({0, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("!spades")), suit_weights({0, 1, 1, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("!heart")), suit_weights({1, 0, 1, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("!DIAMOND")), suit_weights({1, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("!club")), suit_weights({1, 1, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("major")), suit_weights({1, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("MAjors")), suit_weights({1, 1, 0, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("minor")), suit_weights({0, 0, 1, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("MINors")), suit_weights({0, 0, 1, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("black")), suit_weights({1, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("blaCKS")), suit_weights({1, 0, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("red")), suit_weights({0, 1, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("reds")), suit_weights({0, 1, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("round")), suit_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("rounds")), suit_weights({0, 1, 0, 1}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("pointed")), suit_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("pointeds")), suit_weights({1, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("pointeds + spades")), suit_weights({2, 0, 1, 0}));
	ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("pointeds - hearts")), suit_weights({1, -1, 1, 0}));

	
	for (auto [text_expression, count_matching, matching_boards, non_matching_boards] : testcases) {
		std::cerr << "RUNNING " << text_expression << "\n";
		compiled_expression expression = compile_expression(parse_expression(text_expression));
		std::cerr << "COMPILED AS: \n" << expression << "\n";
		for (auto &matching : matching_boards) {
			assert(expression.eval(board(matching)) == 1);
			std::cerr << GREEN << matching << " matches as expected\n" << CLEAR_COLOURS;
		}
		for (auto &non_matching : non_matching_boards) {
			assert(expression.eval(board(non_matching)) == 0);
			std::cerr << GREEN << non_matching << " doesn\'t match as expected\n" << CLEAR_COLOURS;
		}
	}
	// ASSERT_EQUAL(expression_compiler::parse_suits(parse_expression("-hearts")), suit_weights({1, -1, 1, 0})); //TODO: fix
	//TODO: more tests
	//
}
