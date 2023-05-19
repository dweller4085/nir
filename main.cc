#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <sstream>
#include <filesystem>
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

void runCnfTests() {
    u32 const nVars = 100;
    u32 const nClauses = 750;
    u32 const rang = 4;

    for (u32 i = 0; i == 0; i += 1) {
        auto cnf = generateRandomCnfFixedRang(nVars, nClauses, rang, i);
        std::cout << std::format("fixedRang ({}, {}, {}, {}):\n{}", nVars, nClauses, i, rang, cnf);
        Solver::init(cnf);
        std::cout << "result:\n" + (std::string) Solver::solve() << "\n\n\n";
    }
}

int main (int argc, char ** argv) {
    /*for (int i = 1; i <= 14; i += 1) {
        std::cout << i << ". Solving...\n";
        auto in = std::ifstream {std::string{} + "C:\\Users\\i\\Documents\\nir_tests\\random\\dnfRnd_" + std::to_string(i) + ".pla"};
        auto out = std::ofstream {std::string {} + "C:\\Users\\i\\Documents\\nir_tests\\randomr\\test" + std::to_string(i) + ".txt"};
        
        // bleh
        std::stringstream ss {};
        ss << in.rdbuf();

        Solver::init(ss.str());
        out << (std::string) Solver::solve();
        out.close();
    }*/
    runCnfTests();
}
