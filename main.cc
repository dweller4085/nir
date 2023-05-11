#include <iostream>
#include <string>
#include <random>
#include "solver.hh"

std::string generateRandomCnf(u32 varCnt, u32 clauseCnt, u32 seed, f32 distr) {
    auto cnf = std::string {} + "n " + std::to_string(varCnt) + "\nm " + std::to_string(clauseCnt) + "\n";
    auto rng = std::mt19937 {seed};

    // f32 disrt: ratio of '-' in each clause
    static constexpr char val[3] { '0', '1', '-' };
    
    for (u32 i = 0; i < clauseCnt; i += 1) {
        for (u32 j = 0; j < varCnt; j += 1) {
            char c {'-'};
            if (rng() < (u32) UINT32_MAX * distr) {
                c = val[rng () % u32 { 3 }];
            }
            cnf += c;
        } cnf += '\n';
    }

    return cnf;
}

int main (int argc, char ** argv) {

    //auto cnf = generateRandomCnf(31, 5, 123, 0.333f);
    //Solver::init(cnf, Solver::Settings {});
    //std::cout << cnf << "\n";
    //std::cout << (std::string) Solver::solve() << "\n";

    for (u32 i = 0; i < 10; i += 1) {
        auto cnf = generateRandomCnf(31, 31, i, 0.15f);
        Solver::init(cnf, Solver::Settings {});
        std::cout << cnf << "\n";
        std::cout << (std::string) Solver::solve() << "\n";
    }
}
