project(bridge_generator)
set(CMAKE_CXX_STANDARD 17)
add_executable(ranges_test ranges.cpp unit-tests/ranges-test.cpp unit-tests/test_utils.cpp)
add_executable(parser_test parser.cpp unit-tests/parser-test.cpp unit-tests/test_utils.cpp)
