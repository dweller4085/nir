#pragma once
#include <vector>
#include <string>
#include "solver.hh"
#include "memory.hh"

struct CDBView {
    BinVec varVis {Solver::cdb.varCnt, true};
    BinVec clauseVis {Solver::cdb.clauseCnt, true};
};

struct STTNode {
    struct Unit { s32 clause; u32 var; };
    static STTNode nextAfter(STTNode const& current);

    bool unitPropagate();
    bool isSAT() const;
    bool setNextValue();
    void chooseBranchVar();
    bool hasConflict() const;
    void applyAssignment(u32 var, Ternary value);
    Unit findUnit() const;

    bool isMarked {false};
    TerVec model {Solver::cdb.varCnt, Undef};
    CDBView view;
    struct {
        s32 index {-1};
        Ternary value {Undef};
    } branchVar;
};

struct STTStack {
    void push(STTNode&& node) { vec.push_back(std::move(node)); }
    void pop() { vec.pop_back(); }
    bool isEmpty() const { return vec.empty(); }
    STTNode& top() { return vec.back(); }
    usize depth() const { return vec.size() - 1; }

    std::vector<STTNode> vec;
};
