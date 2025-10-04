#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "FileReader.h"

int main(int argc, char** argv) {
    try {
        std::string source = readSource(argc, argv);
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        for (const auto& t : tokens)
            std::cout << left << setw(25) << t.value 
            << left << setw(25) << tokenTypeName(t.type) << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
