#pragma once
#include "common.hh"
#include "mat.hh"
#include <vector>
#include <string>

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    
    CDBView(); // full vis
};

struct STTNode {
    CDBView view;
    TerVec model;
    u32 chosenVar;
    
    STTNode() = default;
    void unitPropagate();
    bool isSAT();
    bool isConflicting();
    bool chooseNextVar(); // if went over all the vars - return false
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

    static u32 varCnt;
    static u32 clauseCnt;

    Solver() = default;
    Solver (std::string const & dimacs, Settings const & settings) noexcept(false);
    Result solve() noexcept;


    Settings settings;
    ClauseDB cdb;
    std::vector<STTNode> STTStack;
};
