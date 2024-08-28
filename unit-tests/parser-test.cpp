#include "../ranges.hpp"
#include "test_utils.hpp"
#include <vector>
#include <string>
#include <iostream>
std::vector <std::string> tokenize(const std::string &s);
template <class c> std::ostream& operator<<(std::ostream &o, const std::vector <c> &x) { //TODO: move to some header file
	o << "{";
	bool first = true;
	for (auto &a : x) {
		if (!first) o << ", ";
		o << a;
		first = false;
	}
	return o;
}
int main() {
	std::cout << "TOKENIZER:" << std::endl;
	ASSERT_EQUAL(tokenize("hcp(NS, spades) != 21"), (std::vector<std::string>{"hcp", "(", "NS", ",",  "spades", ")", "!=", "21"}));
	ASSERT_EQUAL(tokenize("hearts(south)>=5&&!hearts(north)>=2"), (std::vector<std::string>{"hearts", "(", "south", ")", ">=", "5", "&&", "!", "hearts", "(", "north", ")", ">=", "2"}));
	ASSERT_EQUAL(tokenize("1 != 2                "), (std::vector<std::string>{"1", "!=", "2"}));
	ASSERT_EQUAL(tokenize("1>=!2>=!3==!4!=!5&&!6||!7"), (std::vector<std::string>{"1", ">=", "!", "2", ">=", "!", "3", "==", "!", "4", "!=", "!", "5", "&&", "!", "6", "||", "!", "7"}));
	//TODO: more tests
}
