#include "stt.hh"

STTNode STTNode::nextAfter(STTNode const& current) {
    auto next = STTNode {current};
    next.applyAssignment(current.branchVar.index, current.branchVar.value);
    next.isMarked = false;
    return next;
}

bool STTNode::setNextValue() {
    /*
        can probably support arbitrary variable value choice order, not just
        undef - false - true.
        use this->isMarked?
    */
    auto nextValue = Undef;
    
    if (branchVar.index >= 0) {
        switch (branchVar.value) {
            case Undef: nextValue = False; break;
            case False: nextValue = True; break;
            case True: nextValue = Undef; break;
        }
    }

    branchVar.value = nextValue;

    return nextValue != Undef;
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

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        auto vec = TerVecSlice {Scratch::salloc(0), Solver::cdb.clause(i)};
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

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        auto vec = TerVecSlice {Scratch::salloc(0), Solver::cdb.clause(i)};
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
    /* just finding some vector with min rang and picking the col in it with max rang */

    {
        struct {
            u32 rang;
            u32 index = 0;
        } minClause {UINT32_MAX}, maxCol {0};

        /* find a clause with min rang in cdb + cdbview */
        for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
            if (!view.clauseVis.at(i)) continue;

            auto vec = TerVecSlice {Scratch::salloc(0), Solver::cdb.clause(i)};
            vec.applyVis(view.varVis);

            if (auto rang = vec.rang(); rang < minClause.rang) {
                minClause.rang = rang;
                minClause.index = i;
            }
        }

        /* go over def values / columns of this clause, find the column with max rang */
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (!view.varVis.at(j) || Solver::cdb.at(minClause.index, j) == Undef) continue;

            auto vec = TerVecSlice {Scratch::salloc(0), Solver::cdb.column(j)};
            vec.applyVis(view.clauseVis);

            if (auto rang = vec.rang(); rang > maxCol.rang) {
                maxCol.rang = rang;
                maxCol.index = j;
            }
        }

        /* we're done. this is the branch var index. */
        branchVar.index = maxCol.index;

        /* for now this is how it is, because of how setNextValue works. */
        branchVar.value = Undef;
    }

    /*
    branchVar.index = model.findUndef();
    branchVar.value = Undef;
    */
}
