// test_main.cpp
#include "../include/squaker.h"
#include <iostream>

int main() {
    try {
        squ::Script script;
        std::cout << script.execute("1+1").string() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "RunScriptTests threw an exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "RunScriptTests threw an unknown exception." << std::endl;
        return 1;
    }
    std::cout << "RunScriptTests completed without throwing." << std::endl;
    return 0;
}