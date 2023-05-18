#include <iostream>
#include <string>
#include <random>
#include "solver.hh"

std::string generateRandomCnfFixedRang(u32 varCnt, u32 clauseCnt, u32 rang, u32 seed) {
    auto cnf = std::string {} + "n " + std::to_string(varCnt) + "\nm " + std::to_string(clauseCnt) + "\n";
    auto rng = std::mt19937 {seed};
    std::vector<u32> ia {};
    ia.resize(rang);

    static constexpr char val[3] {'0', '1', '-'};

    for (u32 i = 0; i < clauseCnt; i += 1) {
        auto clause = std::string {};
        clause.insert(0, varCnt, '-');

        for (u32 k = 0; k < rang; k += 1) {
            ia[k] = rng() % varCnt;

            for (u32 u = 0; u < k; u += 1) {
                if (ia[k] == ia[u]) {
                    k -= 1;
                    break;
                }
            }
        }

        for (u32 k = 0; k < rang; k += 1) {
            clause[ia[k]] = val[rng() % u32 { 2 }];
        }

        cnf += clause + '\n';
    }

    return cnf;
}



namespace tests {
    void runCnfTests() {
        u32 const nVars = 33;
        u32 const nClauses = 65;
        u32 const rang = 4;

        for (u32 i = 0; i == 0; i += 1) {
            auto cnf = generateRandomCnfFixedRang(nVars, nClauses, rang, i);
            std::cout << std::format("fixedRang ({}, {}, {}, {}):\n{}", nVars, nClauses, i, rang, cnf);
            Solver::init(cnf);
            std::cout << "result:\n" + (std::string) Solver::solve() << "\n\n\n";
        }
    }

    void testGuts() {

    }


}


int main (int argc, char ** argv) {
    tests::runCnfTests();
}
