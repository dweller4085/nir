#pragma once
#include "common.hh"
#include "mat.hh"
#include <vector>

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    CDBView(); // full vis
};

struct STTNode {
    CDBView view;
    TerVec model;
    STTNode(); // empty model + full vis
};

struct Solver {
    struct Settings {
        u64 timeout_ms;
    };
    struct Result {
        struct Stats {
            uint64_t time_ms;
        } stats;

        enum {
            Sat,
            Unsat,
            Aborted,
        } type;

        TerVec sat;
        char const * unsat;
        char const * aborted;
    };

    using ClauseDB = TerMat;
    using Clause = TerVec;

    Solver() = default;
    Solver (char const * dimacs, Settings const & settings) noexcept(false);
    Result solve() noexcept;


    Settings settings;
    ClauseDB cdb;
    std::vector<STTNode> searchState;
};
