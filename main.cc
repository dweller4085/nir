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

std::string generateRandomCnfFixedRang(u32 varCnt, u32 clauseCnt, u32 seed, u32 rang) {
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


int main (int argc, char ** argv) {
    u32 const nVars = 16;
    u32 const nClauses = 31;
    u32 const rang = 2;
    f32 const distr = .1f;
    auto cnf = std::string {};
    enum CnfType {
        fixedRang,
        fixedDistr,
    } cnfType = fixedRang;

    for (u32 i = 0; i == 0; i += 1) {
        switch (cnfType) {
            case fixedRang: {
                cnf = generateRandomCnfFixedRang(nVars, nClauses, i, rang);
                std::cout << "fixedRang (" << nVars << ", " << nClauses << ", " << i << ", " << rang << "):\n" << cnf << "\n";
            } break;

            case fixedDistr: {
                cnf = generateRandomCnf(nVars, nClauses, i, distr);
                std::cout << "fixedDistr (" << nVars << ", " << nClauses << ", " << i << ", " << distr << "):\n" << cnf << "\n";
            } break;
        }

        Solver::init(cnf);
        std::cout << "result:\n" + (std::string) Solver::solve() << "\n\n\n";
    }
}
