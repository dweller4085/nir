#include <iostream>
#include <string>
#include <random>
#include "solver.hh"

std::string generateRandomCnf(u32 varCnt, u32 clauseCnt) {
    auto cnf = std::string {} + "n " + std::to_string(varCnt) + "\nm " + std::to_string(clauseCnt) + "\n";
    auto rng = std::mt19937 {};
    static constexpr char val[3] { '0', '1', '-' };
    for (u32 i = 0; i < clauseCnt; i += 1) {
        for (u32 j = 0; j < varCnt; j += 1) {
            cnf += val[rng () % u32 { 3 }];
        } cnf += '\n';
    }

    return cnf;
}

int main (int argc, char ** argv) {

    auto cnf = generateRandomCnf(8, 5);
    Solver::init(cnf, Solver::Settings {});
    std::cout << cnf << "\n";
    std::cout << (std::string) Solver::solve() << "\n";

    //for (int i = 0; i < 10; i += 1) {
    //    Solver::init(generateRandomCnf(64, 64), Solver::Settings {});
    //    auto result = Solver::solve();
    //    std::cout << (std::string) result << "\n";
    //}
}
