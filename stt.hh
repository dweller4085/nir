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
    struct {
        s32 index {-1};
        TerVec::Value value {TerVec::Value::Undef};
    } branchVar;
    bool isMarked {false};
    /*--------------------*/
    struct Unit { s32 clause; u32 var; };
    static STTNode nextAfter(STTNode const& current);
    bool unitPropagate();
    bool isSAT() const;
    bool setNextValue();
    void chooseBranchVar();
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
    inline usize depth() { return vec.size() - 1; }
};
