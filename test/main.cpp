#include "../include/squaker.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        squ::InitConsole();
        squ::PrintLOGO();
        // squ::RunTests();
        // squ::RunEvalTests();
        // squ::InteractiveExecution();
        squ::RunScriptTests();
        return 0;
    }
    std::string script_path = argv[1];
    try {
        squ::Script script;
        script.execute(squ::ReadFile(script_path));
        std::cout << "Script executed successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
}