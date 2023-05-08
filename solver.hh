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
        // TODO
    }
    TerVecSlice col(u32 var) {
        // TODO
    }
};

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    ClauseDB const& cdb;
    /*-------------*/
    CDBView(ClauseDB const& cdb): varVis {cdb.varCnt, 1}, clauseVis {cdb.clauseCnt, 1}, cdb {cdb} {}
    CDBView(CDBView const &) = default;
    CDBView(CDBView&&) = default;
    ~CDBView() = default;
    void apply(u32 var); // F|A application from the book? Instead of manually setting Vis
};

struct STTNode {
    CDBView view;
    TerVec model;
    s32 branchVar {-1};
    bool isMarked {false};
    /*--------------------*/
    static STTNode nextAfter(STTNode const & current);
    STTNode(): view {Solver::cdb}, model {view.cdb.varCnt, TerVec::Value::Undef} {}
    STTNode(STTNode const&) = default;
    STTNode(STTNode &&) = default;
    bool unitPropagate(); // DPLL UP - false on conflict
    bool isSAT() const; // is current config of view + model make a SAT
    bool tryNextValue(); // if it didn't work for both values of the chosen var - return false
    void chooseBranchVar() { branchVar = model.findUndef(); }
};

struct STTStack {
    std::vector<STTNode> vec;
    /*-----------------------*/
    STTStack() = default;
    void push(STTNode&& node) { vec.push_back(std::move(node)); }
    void pop() { vec.pop_back(); }
    bool isEmpty() { return vec.empty(); }
    STTNode& top() { return vec.back(); }
};
