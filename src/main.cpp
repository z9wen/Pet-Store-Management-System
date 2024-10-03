#include <iostream>

auto plus(int a, int b) -> int {
    int result = a + b;
    return result;
}

#if 0 //for test 1 on & 0 off
auto main() -> int {
    std::cout << "Welcome to Pet Store Management System" << std::endl;

    return 0;
}
#endif
