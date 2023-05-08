#pragma once
#include "common.hh"
#include "vec.hh"
#include <vector>
#include <string>

struct ClauseDB {
    u64 * clauses;
    u32 clauseCnt;
    u32 varCnt;

    ClauseDB() = default;
    ClauseDB(ClauseDB const &) = default;
    ClauseDB& operator = (ClauseDB&&) noexcept;
    ClauseDB(u32 varCnt, u32 clauseCnt) noexcept;
    ~ClauseDB();

    inline TerVecSlice operator [] (u32 i) noexcept {
        return {
            clauses + (varCnt / 32 + 1) * i,
            (varCnt / 32 + 1),
            varCnt
        };
    }
};


struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    CDBView();
    CDBView(CDBView const &) = default;
    ~CDBView() = default;
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

using STTStack = std::vector<STTNode>;

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

    static bool init(std::string const & dimacs, Settings const & settings);
    static void reset();
    static Result solve();

    static ClauseDB cdb;
    static Settings settings;
    static STTStack stack;
};
