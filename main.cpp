// #include "board.hpp"
#include "expression.hpp"
// #include "output_operators.hpp"
#include "parser.hpp"
// #include "ranges.hpp"
// #include "types.hpp"
// #include <random>

int main(int argc, char **argv) {
	// board b("AKQ.JT9.876.5432 5432.AKQ.JT9.876 876.5432.AKQ.JT9 JT9.876.5432.AKQ");
	// b.output(std::cout);
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " count expression\n";
		return 1;
	}
	int a = atoi(argv[1]);
	try {
		compiled_expression expression = compile_expression(parse_expression(argv[2]));
	// std::cout << expression << "\n";
		expression.run_dp(24);
	}
	catch (parse_error e) {
		std::cerr << e.content << "\n";
	}
	// int found = 0;
	// std::mt19937 rng(98441439);
	// int did = 0;
	// while (found < a) {
		// board b(rng);
		// if (expression.eval(b)) {
			// b.output(std::cout);
			// std::cout << "\n\n";
			// found++;
		// }
		// did++;
	// }
	// std::cout << found << "/" << did << std::endl;
}
