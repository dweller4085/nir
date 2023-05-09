#include "stt.hh"

STTNode STTNode::nextAfter(STTNode const & current) {
    auto next = STTNode {current};
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
    
    //
    
    /*
    // catch the conflicts here ...
    last changed var - branchVar - check for orthogonal to it clauses
    s32 unitClause;
    while ((unitClause = view.findUnit()) >= 0) {
        u32 var = unitClause.find(TerVec::Value::Def);
        model.set(var, view.at(unitClause, var));
        // ... and here
        // just check if there are i: view[i][j] = !unitClause[var]
        // if there are none - no conflict
        view.varVis.set(var, 0);
        view.clauseVis.set(unitClause, 0);
    }
    */
    return true;
}

bool STTNode::isSAT() const {
    return {};
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

}