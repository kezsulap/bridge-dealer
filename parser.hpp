#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <string>
#include <ostream>
std::vector <std::string> tokenize(const std::string &s);
struct parsed_expression {
	std::string value;
	std::vector<parsed_expression> sub_expressions;
	bool operator==(const parsed_expression &oth) const;
	bool operator!=(const parsed_expression &oth) const;
};
std::ostream& operator<<(std::ostream &o, const parsed_expression &x);
parsed_expression parse_tokenized_expression(const std::vector <std::string> &tokens);
parsed_expression parse_expression(const std::string &);
struct parse_error {
	std::string content;
};
#endif
