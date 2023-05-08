#pragma once
#include <vector>
#include "solver.hh"

struct STTNode {
    CDBView view;
    TerVec model;
    s32 branchVar {-1};
    bool isMarked {false};
    /*--------------------*/
    static STTNode nextAfter(STTNode const & current);
    STTNode(): view {solver::cdb}, model {view.cdb.varCnt, TerVec::Value::Undef} {}
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
