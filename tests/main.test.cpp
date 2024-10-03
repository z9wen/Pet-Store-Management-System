#include "../lib/catch_amalgamated.hpp"
#include "../src/test.h"

#include <iostream>

TEST_CASE("main test") {
    int a = 10;
    int b = 20;
    int c = plus(a, b);
    CHECK(c == 30);

}