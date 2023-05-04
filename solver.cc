#include "solver.hh"

Solver::Solver(std::string const & dimacs, Settings const & settings):
    settings {settings},
    STTStack {STTNode {}} // the root node of ST
{
    // parse the dimacs string into the cdb...
    // saner format of my own for now

/*
    n 5
    m 4

    011--
    --101
    01001
    110--
*/
}

Solver::Result Solver::solve() {
    while (!STTStack.empty()) {
        STTNode& current = STTStack.back();

        if (current.isMarked) {
            if (current.tryNextVal()) {
                STTStack.push_back(STTNode::nextAfter(current));
            } else {
                // both values have been tried, no sat in this branch
                STTStack.pop_back();
            }
        }
        else if (!current.unitPropagate()) {
            // ran into conflict in UP
            STTStack.pop_back();
        }
        else if (current.isSAT()) {
            return Result {Result::Stats {}, Result::Sat, {.sat {current.model}}};
        }
        else {
            current.chooseBranchVar();
            current.isMarked = true;
        }
    }

    return Result {Result::Stats {}, Result::Unsat, {.unsat {"unsat"}}};
}

CDBView::CDBView():
    varVis {Solver::varCnt, 1},
    clauseVis {Solver::clauseCnt, 1}
{}

bool STTNode::tryNextVal() {
    bool exhaustedVals = false;

    if (branchVar < 0) {
        exhaustedVals = true;
    } else {
        switch (model[branchVar]) {
            using TerVec::Value::Undef, TerVec::Value::False, TerVec::Value::True;

            case Undef: {
                model.set(branchVar, False);
            } break;

            case False: {
                model.set(branchVar, True);
            } break;

            case True: {
                exhaustedVals = true;
            } break;
        }
    }

    return !exhaustedVals;
}

void STTNode::chooseBranchVar() {
    branchVar = model.find(TerVec::Value::Undef);
}

STTNode STTNode::nextAfter(STTNode const & current) {
    STTNode next {current};
    // not sure?
    // next.view.varVis.set(current.branchVar, 0);
    return next;
}

bool STTNode::unitPropagate() {
/*
    s32 unitClause;
    while (unitClause = view.findUnit() && !isConflicting()) {
        u32 var = unitClause.find(TerVec::Value::Def);
        model.set(var, view.at(unitClause, var));
        view.varVis.set(var, 0);
        view.clauseVis.set(unitClause, 0);
    }
    
*/
}
