#include <string>
#include <vector>
#include <optional>
#include "../types.hpp"
#include "../expression.hpp"
#include "test_utils.hpp"

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
	for (auto [text_expression, count_matching, matching_boards, non_matching_boards] : testcases) {
		std::cerr << "RUNNING " << text_expression << "\n";
		compiled_expression expression = compile_expression(parse_expression(text_expression));
		std::cerr << "COMPILED AS: \n" << expression << "\n";
		for (auto &matching : matching_boards) assert(expression.eval(board(matching)));
		for (auto &non_matching : non_matching_boards) assert(!expression.eval(board(non_matching)));
		if (count_matching.has_value()) { //TODO: some test on whether the sampling looks uniform
			ASSERT_EQUAL(*count_matching, expression.run_dp().first);
		}
	}
}
