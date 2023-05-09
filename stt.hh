#pragma once
#include <vector>
#include "solver.hh"

struct CDBView {
    BinVec varVis {Solver::cdb.varCnt, 1};
    BinVec clauseVis {Solver::cdb.clauseCnt, 1};
    /*-------------*/
    struct Unit {
        s32 clause;
        u32 var;
    };
    void apply(u32 var, TerVec::Value value); // F <- F|A application
    Unit findUnit() const;
};

struct STTNode {
    CDBView view {};
    TerVec model {Solver::cdb.varCnt, TerVec::Value::Undef};
    s32 branchVar {-1};
    bool isMarked {false};
    /*--------------------*/
    static STTNode nextAfter(STTNode const & current);
    bool unitPropagate(); // DPLL UP - false on conflict
    bool isSAT() const; // is current config of view + model make a SAT
    bool tryNextValue(); // if it didn't work for both values of the chosen var - return false
    void chooseBranchVar() { branchVar = model.findUndef(); }
    bool hasConflict() const;
};

struct STTStack {
    std::vector<STTNode> vec;
    /*-----------------------*/
    inline void push(STTNode&& node) { vec.push_back(std::move(node)); }
    inline void pop() { vec.pop_back(); }
    inline bool isEmpty() const { return vec.empty(); }
    inline STTNode& top() { return vec.back(); }
};
