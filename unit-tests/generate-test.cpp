#include <string>
#include <vector>
#include "../types.hpp"
#include "../output_operators.hpp"
#include "../expression.hpp"

void generate_testcase(std::string expression, int cou_matching, int cou_nonmatching, const std::vector<std::string> &other_matching, const std::vector<std::string> &other_nonmatching) {
	auto [matching_count, matching_sample] = compile_expression(parse_expression(expression)).run_dp(cou_matching);
	auto [nonmatching_count, nonmatching_sample] = compile_expression(parse_expression("!(" + expression + ")")).run_dp(cou_nonmatching);
	std::cerr << matching_count << " + " << nonmatching_count << " =?= " << TOTAL_COUNT << "\n";
	assert(matching_count + nonmatching_count == TOTAL_COUNT);
	std::cout << "{\n\t\"" << expression << "\",\n\tparse_int128(\"" << matching_count << "\"),\n\t{\n";
	for (auto &b : matching_sample) std::cout << "\t\t\"" << b.to_notation() << "\",\n";
	for (auto &b : other_matching) std::cout << "\t\t\"" << b << "\",\n";
	std::cout << "\t}, {\n";
	for (auto &b : nonmatching_sample) std::cout << "\t\t\"" << b.to_notation() << "\",\n";
	for (auto &b : other_matching) std::cout << "\t\t\"" << b << "\",\n";
	std::cout << "\t}\n},\n";
}

int main(int argc, char **argv) {
	assert(argc == 4);
	generate_testcase(argv[1], atoi(argv[2]), atoi(argv[3]), {}, {});
}
