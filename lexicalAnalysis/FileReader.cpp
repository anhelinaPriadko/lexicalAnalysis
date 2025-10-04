#include "FileReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

string readSource(int argc, char** argv) {
    std::ostringstream ss;
    if (argc > 1) {
        ifstream in(argv[1]);
        if (!in.is_open())
            throw runtime_error("Cannot open file: " + string(argv[1]));
        ss << in.rdbuf();
    }
    else {
        ss << cin.rdbuf();
    }
    return ss.str();
}