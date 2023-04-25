#include "common.hh"
#include "solver.hh"

#include <iostream>

int main (int argc, char ** argv) {
    
    char * dimacs = nullptr;

    try {
        auto solver = Solver::from_dimacs(dimacs).with_settings(Solver::Settings {});
        auto result = solver.solve();

        switch (result.type) {
            case Solver::Result::Sat: {

            } break;
            case Solver::Result::Unsat: {

            } break;
            case Solver::Result::Aborted: {

            } break;
        }

    } catch (std::exception const & e) {
        std::cerr << e.what() << "\n";
        exit(-1);
    }
}
