#include "stt.hh"

STTNode STTNode::nextAfter(STTNode const & current) {
    auto next = STTNode {current};
    next.view.apply(current.branchVar);
    return next;
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