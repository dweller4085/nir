#include "stt.hh"

STTNode STTNode::nextAfter(STTNode const & current) {
    auto next = STTNode {current};
    // [NOTE A] here as well. maybe make the chooseNextVar actually return a var?
    next.view.apply(current.branchVar, current.model.at(current.branchVar));
    return next; // nrvo hopefully
}

bool STTNode::tryNextValue() {
    bool exhaustedVals = false;

    if (branchVar < 0) {
        exhaustedVals = true;
    }
    else {
        switch (model.at(branchVar)) {
            using TerVec::Value::Undef, TerVec::Value::False, TerVec::Value::True;

            case Undef: model.set(branchVar, False); break;
            case False: model.set(branchVar, True); break;
            case True: exhaustedVals = true; break;
        }
    }

    return !exhaustedVals;
}

bool STTNode::unitPropagate() {
    while (true) {
        auto [clause, var] = view.findUnit();
        if (clause < 0) break;

        // should probably do it in one STTNode::apply or something [NOTE A]
        model.set(var, Solver::cdb.at(clause, var));
        view.apply(var, Solver::cdb.at(clause, var));

        if (hasConflict()) return false;
    }
    
    return true;
}

bool STTNode::isSAT() const {
    // probably when view.clauseVis is all zeroes
    // because then it would probably mean that all clauses are T === SAT
    return view.clauseVis.isAllZeros();
}

void CDBView::apply(u32 var, TerVec::Value value) {
    // remove this var from consideration - it either evaluates to F in the clause
    // or makes the whole clause T
    varVis.clear(var);
    
    // remove all clauses from consideration that are parallel to (var = value)
    // i.e. the clauses that become T with this var assignment
    // ... simplest implementation for now to just get something working at all
    auto const col = Solver::cdb.column(var);
    for (u32 i = 0; i < col.len; i += 1) {
        if (col.at(i) == value) {
            clauseVis.clear(i);
        }
    }
}

bool STTNode::hasConflict() const {
    // doesn't really need to know the last applied assignment, does it?
    // has to go through each visible clause in view to see if there are any empty ones
    // ... again a simple implementation: just searching for the empty clause
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!view.clauseVis.at(i)) continue;

        // maybe make a method with view.at(i, j)? maybe
        bool isEmpty = true;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (!view.varVis.at(i)) continue;
            if (Solver::cdb.at(i, j) != TerVec::Value::Undef) {
                isEmpty = false;
            }
        }

        if (isEmpty) return true;
    }

    return false;
}

CDBView::Unit CDBView::findUnit() const {


    // .. simplest bare minimum implementation
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        if (!clauseVis.at(i)) continue;

        u32 k = 0;
        s32 s = 0;
        for (u32 j = 0; j < Solver::cdb.varCnt; j += 1) {
            if (!varVis.at(j)) continue;
            if (Solver::cdb.at(i, j) != TerVec::Value::Undef) {
                s += 1;
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
