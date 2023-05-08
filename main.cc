#include <iostream>
#include <string>
#include "common.hh"
#include "solver.hh"


char const * const cnf {R"(
n 40
m 4

01001010010100101001010010100101001010-1
11-1111-1111-1111-1111-1111-1111-1111-11
100--100--100--100--100--100--100--100--
----1----1----1----1----1----1----1----1
)"};

int main (int argc, char ** argv) {
    Solver::init(std::string {cnf}, Solver::Settings {});
        
    return {0};
}
