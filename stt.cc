#include "stt.hh"

STTNode STTNode::nextAfter(STTNode const& current) {
    auto next = STTNode {current};
    next.applyAssignment(current.branchVar.index, current.branchVar.value);
    next.isMarked = false;
    next.branchVar.value = Undef;
    return next;
}

bool STTNode::setNextValue() {
    if (branchVar.value == Undef) {
        branchVar.value = branchVar.first;
    } else if (branchVar.value == branchVar.first) {
        branchVar.value = (Ternary) (0b1 ^ ((u64) branchVar.first));
    } else {
        branchVar.value = Undef;
    }

    return branchVar.value != Undef;
}

bool STTNode::unitPropagate() {
    while (true) {
        auto [clause, var] = findUnit();
        if (clause < 0) break;

        applyAssignment(var, Solver::cdb.at(clause, var));

        if constexpr (Solver::settings.modelTrace) {
            Solver::stats.modelTrace.UP(model);
        }

        if (hasConflict()) return false;
    }
    
    return true;
}

bool STTNode::isSAT() const {
    return view.clauseVis.isAllZeroes();
}

bool STTNode::hasConflict() const {
    // try to find an empty clause

    auto _ = Scratch {};

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.clause(i)};
        vec.applyVis(view.varVis);

        if (vec.isEmpty()) {
            Solver::stats.conflictCnt += 1;
            return true;
        }
    }

    return false;
}

STTNode::Unit STTNode::findUnit() const {
    // try to find a unit clause

    auto _ = Scratch {};

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.clause(i)};
        vec.applyVis(view.varVis);

        if (auto var = vec.isUnit(); var >= 0) {
            return Unit {(s32) i, (u32) var};
        }
    }

    return Unit {-1, 0};
}

void STTNode::applyAssignment(u32 var, Ternary value) {
    model.set(var, value);
    view.varVis.clear(var);

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (view.clauseVis.at(i) && Solver::cdb.at(i, var) == value) {
            view.clauseVis.clear(i);
        }
    }    
}

void STTNode::chooseBranchVar() {
    auto _ = Scratch {};
    
    struct {
        usize * clauses;
        TerVecSlice mask;
        usize size = 0;
    } minRangClauseSet {
        _.alloc<usize>(Solver::cdb.clauseCnt),
        {Solver::cdb.clauseCnt, False, _.alloc(TerVecSlice::memoryFor(Solver::cdb.clauseCnt), alignof(u64))},
    };

    { /* build the min rang clauses subset (=: R) of cdb + view. */
        auto _ = Scratch {};

        u32 * clauseRangs = _.alloc<u32>(Solver::cdb.clauseCnt);
        for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) clauseRangs[i] = UINT32_MAX;

        u32 minRang = UINT32_MAX;

        /* fill the array of clause rangs. find the min rang. */
        for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
            if (!view.clauseVis.at(i)) continue;

            auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.clause(i)};
            vec.applyVis(view.varVis);

            auto rang = vec.rang();
            clauseRangs[i] = rang;

            if (rang < minRang) {
                minRang = rang;
            }
        }

        /* fill the min rang clause subset */
        for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
            if (view.clauseVis.at(i) && clauseRangs[i] == minRang) {
                minRangClauseSet.clauses[minRangClauseSet.size++] = i;
                minRangClauseSet.mask.set(i);
            }
        }
    }

    struct Columns {
        usize * columns;
        usize size = 0;
    }
    maxRangColumnSet {_.alloc<usize>(Solver::cdb.varCnt)},
    maxRangMonotoneCols {_.alloc<usize>(Solver::cdb.varCnt)};

    { /* now build the set of columns with max rang in R (=: Q), and the subset of monotone cols in Q (=: M). */
        auto _ = Scratch {};

        u32 * columnRangs = _.alloc<u32>(Solver::cdb.varCnt);
        for (u32 i = 0; i < Solver::cdb.varCnt; i += 1) columnRangs[i] = 0;

        u32 maxRang = 0;

        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (!view.varVis.at(j)) continue;

            auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.column(j)};
            vec.applyVis(minRangClauseSet.mask);

            auto rang = vec.rang();
            columnRangs[j] = rang;

            if (rang > maxRang) {
                maxRang = rang;
            }
        }

        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (view.varVis.at(j) && columnRangs[j] == maxRang) {
                auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.column(j)};
                vec.applyVis(minRangClauseSet.mask);

                maxRangColumnSet.columns[maxRangColumnSet.size++] = j;
                if (vec.isMonotone() != Undef) maxRangMonotoneCols.columns[maxRangMonotoneCols.size++] = j;
            }
        }
    }

    /* finally we've selected all the subsets R Q M. */
    
    /* 1. choose the branch var. */
    {
        auto _ = Scratch {};

        struct {
            u32 rang = 0;
            usize index = 0;
        } maxCol;

        Columns cols;
        if (maxRangMonotoneCols.size > 0) {
            cols = maxRangMonotoneCols;
        } else {
            cols = maxRangColumnSet;
        }

        /* choose the column with the max rang in cdb + view. */
        for (u32 k = 0; k < cols.size; k += 1) {
            auto j = cols.columns[k];

            auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.column(j)};
            vec.applyVis(view.clauseVis);

            if (auto rang = vec.rang(); rang > maxCol.rang) {
                maxCol.rang = rang;
                maxCol.index = j;
            }
        }

        branchVar.index = maxCol.index; 
    }

    /* 2. choose the first branch var value to be tried. */
    {
        auto _ = Scratch {};

        auto vec = TerVecSlice {_.alloc(0, 8), Solver::cdb.column(branchVar.index)};
        vec.applyVis(view.clauseVis);

        if (vec.countOnes() >= vec.rang() / 2) {
            branchVar.first = True;
        } else {
            branchVar.first = False;
        }
    }

    /* 3. we're done. */

    /*
    branchVar.index = model.findUndef();
    branchVar.first = False;
    */
}
