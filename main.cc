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
        auto clause = std::string {};
        u32 s {0};
        for (u32 j = 0; j < varCnt; j += 1) {
            char c {'-'};
            if (rng() < (u32) UINT32_MAX * distr) {
                c = val[rng () % u32 { 2 }];
                s += 1;
            }
            clause += c;
        }

        if (s == 0) {
            i -= 1;
            continue;
        } else {
            cnf += clause + '\n';
        }
    }

    return cnf;
}

int main (int argc, char ** argv) {
    u32 const nVars = 31;
    u32 const nClauses = 31;
    f32 const distr = .1f;
    for (u32 i = 3; i == 3; i += 1) {
        auto cnf = generateRandomCnf(nVars, nClauses, i, distr);
        Solver::init(cnf, Solver::Settings {});
        std::cout << "cnf (" << nVars << ", " << nClauses << ", " << i << ", " << distr << "):\n";
        std::cout << cnf << "\n";
        std::cout << "result:\n" + (std::string) Solver::solve() << "\n\n\n";
    }
}
