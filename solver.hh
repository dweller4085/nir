#pragma once
#include "vec.hh"
#include <vector>
#include <string>

struct ClauseDB;
struct STTStack;

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
};

struct ClauseDB {
    u64 * clauses;
    u32 clauseCnt;
    u32 varCnt;
    u32 wordsPerVar;
    /*------------*/
    ClauseDB() = default;
    ClauseDB& operator = (ClauseDB&&) noexcept;
    ClauseDB(u32 varCnt, u32 clauseCnt) noexcept;
    ~ClauseDB();
    
    void set(u32 clause, u32 var, TerVec::Value val) {
        u64 const j = (clause % 32) * 2;
        u64 * const word = clauses + wordsPerVar * var + clause / 32;
        *word &= ~(u64 {0b11} << j);
        *word |= (u64) val << j;
    }
    TerVec::Value at(u32 clause, u32 var) const {
        // todo
    }
    TerVecSlice col(u32 var) {
        // todo
    }
};

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    /*-------------*/
    CDBView(): varVis {Solver::cdb.varCnt, 1}, clauseVis {Solver::cdb.clauseCnt, 1} {}
    CDBView(CDBView const &) = default;
    CDBView(CDBView&&) = default;
    ~CDBView() = default;
};

struct STTNode {
    CDBView view;
    TerVec model;
    s32 branchVar {-1};
    bool isMarked {false};
    /*--------------------*/
    STTNode(): view {}, model {Solver::cdb.varCnt, TerVec::Value::Undef} {}
    STTNode(STTNode const&) = default;
    STTNode(STTNode &&) = default;
    bool unitPropagate(); // DPLL UP - false on conflict
    bool isSAT() const; // is current config of view + model make a SAT
    bool tryNextVal(); // if it didn't work for both values of the chosen var - return false
    void chooseBranchVar() { branchVar = model.findUndef(); }
};

struct STTStack {
    std::vector<STTNode> vec;
    /*-----------------------*/
    STTStack() = default;
    void push(STTNode const & node) { vec.emplace_back(node); }
    void pop() { vec.pop_back(); }
    bool isEmpty() { return vec.empty(); }
    STTNode& top() { return vec.back(); }
};
