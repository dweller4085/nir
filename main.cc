#include <iostream>
#include <string>
#include "solver.hh"


char const * const cnf {R"(
n 3
m 4
01-
0-1
-11
-00
)"};

int main (int argc, char ** argv) {
    Solver::init(std::string {cnf}, Solver::Settings {});
    auto result = Solver::solve();

}
