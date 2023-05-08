#include <iostream>
#include <string>
#include "solver.hh"


char const * const cnf {R"(
n 5
m 4
01001
11-11
100--
----1
)"};

int main (int argc, char ** argv) {
    if (!Solver::init(std::string {cnf}, Solver::Settings {})) {
        exit(-1);
    }

    auto result = Solver::solve();
    


    return {0};
}
