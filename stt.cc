#include "stt.hh"

TerVec::Value STTNode::nextBVValue() const {
    using TerVec::Value::Undef, TerVec::Value::False, TerVec::Value::True;
    
    TerVec::Value nextValue {Undef};
    
    if (branchVar >= 0) {
        switch (model.at(branchVar)) {
            case Undef: nextValue = True; break;
            case True: nextValue = False; break;
            case False: nextValue = Undef; break;
        }
    }

    return nextValue;
}

bool STTNode::unitPropagate() {
    while (true) {
        auto [clause, var] = findUnit();
        if (clause < 0) break;

        applyAssignment(var, Solver::cdb.at(clause, var));
        if (hasConflict()) return false;
    }
    
    return true;
}

bool STTNode::isSAT() const {
    // probably when view.clauseVis is all zeroes
    // because then it would probably mean that all clauses are T === SAT
    return view.clauseVis.isAllZeros();
}

bool STTNode::hasConflict() const {
    // doesn't really need to know the last applied assignment, does it?
    // has to go through each visible clause in view to see if there are any empty ones
    // ... again a simple implementation: just searching for the empty clause
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        bool isEmpty = true;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (view.varVis.at(j) && Solver::cdb.at(i, j) != TerVec::Value::Undef) {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty) return true;
    }

    return false;
}

STTNode::Unit STTNode::findUnit() const {
    // .. simplest bare minimum implementation
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        u32 k = 0;
        s32 s = 0;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (view.varVis.at(j) && Solver::cdb.at(i, j) != TerVec::Value::Undef) {
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

void STTNode::applyAssignment(u32 var, TerVec::Value value) {
    model.set(var, value);
    view.varVis.clear(var);

    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (view.clauseVis.at(i) && Solver::cdb.at(i, var) == value) {
            view.clauseVis.clear(i);
        }
    }
}
