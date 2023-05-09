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
    Solver::init(std::string {cnf}, Solver::Settings {});
    auto result = Solver::solve();

}
