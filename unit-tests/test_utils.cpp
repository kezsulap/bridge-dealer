#include "test_utils.hpp"
#include <cstdlib>
bool any_failed = false;
void test_any_fails() {
	if (any_failed) exit(1);
}
static int _ = (atexit(test_any_fails), 0);
