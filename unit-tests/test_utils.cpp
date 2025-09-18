#include "test_utils.hpp"
#include <cstdlib>
void mark_test_failure() {
	any_failed = true;
}
void test_any_fails() {
	if (any_failed) exit(1);
}
static int _ = (atexit(test_any_fails), 0);
