#pragma once
#include "common.hh"
#include "mat.hh"
#include <vector>
#include <string>

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    CDBView();
};

struct STTNode {
    CDBView view;
    TerVec model;
    s32 branchVar;
    bool isMarked {false};
    
    static STTNode nextAfter(STTNode const &);
    bool unitPropagate(); // DPLL UP - false on conflict
    bool isSAT() const; // is current config of view + model make a SAT
    bool tryNextVal(); // if it didn't work for both values of the chosen var - return false
    void chooseBranchVar();
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
        
        struct {
            TerVec sat;
            char const * unsat;
            char const * aborted;
        } value;
    };

    using ClauseDB = TerMat;
    using Clause = TerVec;

    Solver() = default;
    Solver (std::string const & dimacs, Settings const & settings) noexcept(false);
    Result solve();

    static u32 varCnt; // immutable
    static u32 clauseCnt; // immutable

    Settings settings;
    static ClauseDB cdb; // immutable
    std::vector<STTNode> STTStack;
};
