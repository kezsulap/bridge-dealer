#include "../ranges.hpp"
#include "../parser.hpp"
#include "test_utils.hpp"
#include <vector>
#include <string>
#include <iostream>
int main() {
	std::cout << "TOKENIZER:" << std::endl;
	ASSERT_EQUAL(tokenize("hcp(NS, spades) != 21"), (std::vector<std::string>{"hcp", "(", "NS", ",",  "spades", ")", "!=", "21"}));
	ASSERT_EQUAL(tokenize("hearts(south)>=5&&!hearts(north)>=2"), (std::vector<std::string>{"hearts", "(", "south", ")", ">=", "5", "&&", "!", "hearts", "(", "north", ")", ">=", "2"}));
	ASSERT_EQUAL(tokenize("1 != 2                "), (std::vector<std::string>{"1", "!=", "2"}));
	ASSERT_EQUAL(tokenize("1>=!2>=!3==!4!=!5&&!6||!7^^!8"), (std::vector<std::string>{"1", ">=", "!", "2", ">=", "!", "3", "==", "!", "4", "!=", "!", "5", "&&", "!", "6", "||", "!", "7", "^^", "!", "8"}));
	ASSERT_EQUAL(tokenize("      "), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize("\n"), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize("\r"), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize("\t"), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize("\v"), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize(" \n\r\t\v "), (std::vector<std::string>{}));
	ASSERT_EQUAL(tokenize("1\n\r\t\v+2"), (std::vector<std::string>{"1", "+", "2"}));
	ASSERT_EQUAL(tokenize("0xxx"), (std::vector<std::string>{"0xxx"}));
	ASSERT_EQUAL(tokenize("xx0x"), (std::vector<std::string>{"xx0x"}));
	ASSERT_EQUAL(tokenize("[53][41]"), (std::vector<std::string>{"[53][41]"}));
	ASSERT_EQUAL(tokenize("shape(north, [53][41])"), (std::vector<std::string>{"shape", "(", "north", ",", "[53][41]", ")"}));
	std::cout << "PARSER:" << std::endl;
	ASSERT_EQUAL(parse_expression("1"), (parsed_expression{"1", {}}));
	ASSERT_EQUAL(parse_expression("1+2"), (parsed_expression{
				"+", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1*2"), (parsed_expression{
				"*", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1-2"), (parsed_expression{
				"-", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1/2"), (parsed_expression{
				"/", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1%2"), (parsed_expression{
				"%", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1&&2"), (parsed_expression{
				"&&", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1||2"), (parsed_expression{
				"||", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1^^2"), (parsed_expression{
				"^^", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1==2"), (parsed_expression{
				"==", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1!=2"), (parsed_expression{
				"!=", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1<2"), (parsed_expression{
				"<", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1>2"), (parsed_expression{
				">", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1<=2"), (parsed_expression{
				"<=", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1>=2"), (parsed_expression{
				">=", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("(1)"), (parsed_expression{"1", {}}));
	ASSERT_EQUAL(parse_expression("((((((((((1))))))))))"), (parsed_expression{"1", {}}));
	ASSERT_EQUAL(parse_expression("1+2*3"), (parsed_expression{
				"+", {
					parsed_expression{"1", {}},
					parsed_expression{"*", {
						parsed_expression{"2", {}},
						parsed_expression{"3", {}}
					}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1*2+3"), (parsed_expression{
				"+", {
					parsed_expression{"*", {
						parsed_expression{"1", {}},
						parsed_expression{"2", {}}
					}},
					parsed_expression{"3", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1+(2*3)"), (parsed_expression{
				"+", {
					parsed_expression{"1", {}},
					parsed_expression{"*", {
						parsed_expression{"2", {}},
						parsed_expression{"3", {}}
					}}
				}
			}));
	ASSERT_EQUAL(parse_expression("(1*2)+3"), (parsed_expression{
				"+", {
					parsed_expression{"*", {
						parsed_expression{"1", {}},
						parsed_expression{"2", {}}
					}},
					parsed_expression{"3", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("(1+2)*3"), (parsed_expression{
				"*", {
					parsed_expression{"+", {
						parsed_expression{"1", {}},
						parsed_expression{"2", {}}
					}},
					parsed_expression{"3", {}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1*(2+3)"), (parsed_expression{
				"*", {
					parsed_expression{"1", {}},
					parsed_expression{"+", {
						parsed_expression{"2", {}},
						parsed_expression{"3", {}}
					}}
				}
			}));
	ASSERT_EQUAL(parse_expression("1?2:3"), (parsed_expression{
				"?:", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}},
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("1?2:3?4:5"), (parsed_expression{
				"?:", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}},
					parsed_expression{
						"?:", {
							parsed_expression{"3", {}},
							parsed_expression{"4", {}},
							parsed_expression{"5", {}},
						}
					}
				}
			}));
	ASSERT_EQUAL(parse_expression("1?2?3:4:5"), (parsed_expression{
				"?:", {
					parsed_expression{"1", {}},
					parsed_expression{
						"?:", {
							parsed_expression{"2", {}},
							parsed_expression{"3", {}},
							parsed_expression{"4", {}},
						}
					},
					parsed_expression{"5", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("max(1, 2, 3)"), (parsed_expression{
				"max", {
					parsed_expression{"1", {}},
					parsed_expression{"2", {}},
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("max(1)"), (parsed_expression{
				"max", {
					parsed_expression{"1", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("max(1, min(2, 3))"), (parsed_expression{
				"max", {
					parsed_expression{"1", {}},
					parsed_expression {
						"min", {
							parsed_expression{"2", {}},
							parsed_expression{"3", {}},
						}
					}
				}
			}));
	ASSERT_EQUAL(parse_expression("!3"), (parsed_expression{
				"!", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("!(3)"), (parsed_expression{
				"!", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("+3"), (parsed_expression{
				"+", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("+(3)"), (parsed_expression{
				"+", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("-3"), (parsed_expression{
				"-", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("!(3)"), (parsed_expression{
				"!", {
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("max(1, -2)"), (parsed_expression{
				"max", {
					parsed_expression{"1", {}},
					parsed_expression {
						"-", {
							parsed_expression{"2", {}},
						}
					}
				}
			}));
	ASSERT_EQUAL(parse_expression("max(-1, 2)"), (parsed_expression{
				"max", {
					parsed_expression {
						"-", {
							parsed_expression{"1", {}},
						}
					},
					parsed_expression{"2", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("1 >= -2"), (parsed_expression{
				">=", {
					parsed_expression{"1", {}},
					parsed_expression {
						"-", {
							parsed_expression{"2", {}},
						}
					},
				}
			}));
	ASSERT_EQUAL(parse_expression("1 * -2"), (parsed_expression{
				"*", {
					parsed_expression{"1", {}},
					parsed_expression {
						"-", {
							parsed_expression{"2", {}},
						}
					},
				}
			}));
	ASSERT_EQUAL(parse_expression("1 == 2 == 3"), (parsed_expression{
				"", {
					parsed_expression{"1", {}},
					parsed_expression{"==", {}},
					parsed_expression{"2", {}},
					parsed_expression{"==", {}},
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("1 <= 2 < 3"), (parsed_expression{
				"", {
					parsed_expression{"1", {}},
					parsed_expression{"<=", {}},
					parsed_expression{"2", {}},
					parsed_expression{"<", {}},
					parsed_expression{"3", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("!a < b"), (parsed_expression{
				"<", {
					parsed_expression{"!", {
						parsed_expression{"a", {}},
					}},
					parsed_expression{"b", {}},
				}
			}));
	ASSERT_EQUAL(parse_expression("!!a"), (parsed_expression{
				"!", {
					parsed_expression{"!", {
						parsed_expression{"a", {}},
					}},
				}
			}));
	ASSERT_EQUAL(parse_expression("--a"), (parsed_expression{
				"-", {
					parsed_expression{"-", {
						parsed_expression{"a", {}},
					}},
				}
			}));
}
