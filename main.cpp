// #include "board.hpp"
#include "expression.hpp"
// #include "output_operators.hpp"
#include "parser.hpp"
// #include "ranges.hpp"
// #include "types.hpp"
#include <random>
#include <sstream>

int main(int argc, char **argv) {
	// board b("AKQ.JT9.876.5432 5432.AKQ.JT9.876 876.5432.AKQ.JT9 JT9.876.5432.AKQ");
	// b.output(std::cout);
	if (argc != 4 || (std::string(argv[3]) != "dp" && std::string(argv[3]) != "random" && std::string(argv[3]) != "just_p")) {
		std::cerr << "Usage: " << argv[0] << " count expression [dp/random/just_p]\n";
		return 1;
	}
	std::vector<board> boards;
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
			if (!any) break;
			std::cerr << "\n";
		}
		for (int _ = 0; _ < 245; ++_)
			std::cerr << "-";
		std::cerr << "\n";
	};
	std::cerr << "processing: " << argv[2] << "\n";
	int a = atoi(argv[1]);
	try {
		std::mt19937 rng(913612133);
		compiled_expression expression = compile_expression(parse_expression(argv[2]));
		std::cout << expression << "\n";
		int tried = 0;
		if (std::string(argv[3]) == "dp") {
			expression.run_dp(a);
		} else {
			for (int _ = 0; _ < 245; ++_)
				std::cerr << "-";
			std::cerr << "\n";
			int found = 0;
			while (found < a) {
				tried++;
				board b(rng);
				if (expression.eval(b)) {
					found++;
					if (std::string(argv[3]) == "random") {
						boards.push_back(b);
						if (boards.size() == 6u) {
							dump();
							boards.clear();
						}
					}
				}
			}
			if (!boards.empty()) dump();
			std::cerr << "(found = " << found << ") / (tried = " << tried << ") = " << (long double) (found) / tried << "\n";
		}
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
