#pragma once
#include <vector>
#include <string>
#include "solver.hh"

struct CDBView {
    BinVec varVis {Solver::cdb.varCnt, 1};
    BinVec clauseVis {Solver::cdb.clauseCnt, 1};
};

struct STTNode {
    CDBView view {};
    TerVec model {Solver::cdb.varCnt, TerVec::Value::Undef};
    s32 branchVar {-1};
    bool isMarked {false};
    /*--------------------*/
    struct Unit { s32 clause; u32 var; };
    bool unitPropagate();
    bool isSAT() const;
    TerVec::Value nextBVValue() const;
    void chooseBranchVar() { branchVar = model.findUndef(); }
    bool hasConflict() const;
    void applyAssignment(u32 var, TerVec::Value value);
    Unit findUnit() const;
};

struct STTStack {
    std::vector<STTNode> vec;
    /*-----------------------*/
    inline void push(STTNode&& node) { vec.push_back(std::move(node)); }
    inline void pop() { vec.pop_back(); }
    inline bool isEmpty() const { return vec.empty(); }
    inline STTNode& top() { return vec.back(); }
};
