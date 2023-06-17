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
    return view.clauseVis.isAllZeros();
}

bool STTNode::hasConflict() const {
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        bool isEmpty = true;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (view.varVis.at(j) && Solver::cdb.at(i, j) != Undef) {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty) {
            Solver::stats.conflictCnt += 1;
            return true;
        }
    }

    return false;
}

STTNode::Unit STTNode::findUnit() const {
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        u32 k = 0;
        s32 s = 0;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (view.varVis.at(j) && Solver::cdb.at(i, j) != Undef) {
                if ((s += 1) > 1) break;
                k = j;
            }
        }

        if (s == 1) {
            return Unit {(s32)i, k};
        }
    }

    // sentinel 'no unit found'
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
    /*
    branchVar.index = model.findUndef();
    branchVar.value = Undef;
    */

    /* 1. find the set of vectors with min rang in the current cdb + cdbview.*/
    /* 2. ... */

    /* 24:4 2330 :( ugly hack for now... */
    /* just finding some vector with min rang and picking the col in it with max rang */

    if constexpr (false) {
        struct {
            u32 rang;
            u32 index = 0;
        } minClause {UINT32_MAX}, maxCol {0};

        /* find a clause with min rang in cdb + cdbview */
        for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
            if (!view.clauseVis.at(i)) continue;

            u32 rang = 0;
            for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
                if (view.varVis.at(j) && Solver::cdb.at(i, j) != Undef) rang += 1;
            }

            if (rang < minClause.rang) {
                minClause.rang = rang;
                minClause.index = i;
            }
        }

        /* go over def values / columns of this clause, find the column with max rang */
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (!view.varVis.at(j) || Solver::cdb.at(minClause.index, j) == Undef) continue;

            u32 rang = 0;
            for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
                if (view.clauseVis.at(i) && Solver::cdb.at(i, j) != Undef) rang += 1;
            }

            if (rang > maxCol.rang) {
                maxCol.rang = rang;
                maxCol.index = j;
            }
        }

        /* we're done. this is the branch var index. */
        branchVar.index = maxCol.index;

        /* for now this is how it is, because of how setNextValue works. */
        branchVar.value = Undef;
    }

    else {
        branchVar.index = model.findUndef();
        branchVar.value = Undef;
    }
}
