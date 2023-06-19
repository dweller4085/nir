#pragma once
#include <vector>
#include <string>
#include "solver.hh"
#include "memory.hh"

struct CDBView {
    BinVecSlice varVis;
    BinVecSlice clauseVis;
};

struct STTNode {
    struct Unit { s32 clause; u32 var; };

    static FrameAllocator allocator;
    static STTNode nextAfter(STTNode const& current);

    STTNode();
    STTNode(STTNode&&) noexcept;
    STTNode(STTNode const&) noexcept;
    STTNode& operator = (STTNode const&) = default;
    ~STTNode();

    bool unitPropagate();
    bool isSAT() const;
    bool setNextValue();
    void chooseBranchVar();
    bool hasConflict() const;
    void applyAssignment(u32 var, Ternary value);
    Unit findUnit() const;

    bool isMoved {false}; // god damn it
    bool isMarked {false};
    TerVecSlice model;
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
