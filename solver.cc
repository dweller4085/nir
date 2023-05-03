#include "solver.hh"

Solver::Solver(std::string const & dimacs, Settings const & settings):
    settings {settings},
    cdb {},
    STTStack {STTNode {}} // the root node of ST
{
    // parse the dimacs string into the cdb...
    // saner format of my own for now

}

Solver::Result Solver::solve() {
    
    while (!STTStack.empty()) {
        STTNode& current = STTStack.back();
        current.unitPropagate();

        if (current.isConflicting()) {
            STTStack.pop_back();
        } 
        else if (current.isSAT()) {
            return Result {Result::Stats {}, Result::Sat, {.sat {}}};
        }
        else if (current.chooseNextVar()) {
            // correct the view here? with the chosen model + var
            STTStack.push_back(STTNode {});
        }
        else {
            STTStack.pop_back();
        }
    }

    return Result {Result::Stats {}, Result::Unsat, {.unsat {"unsat"}}};
}

CDBView::CDBView():
    varVis {Solver::varCnt, 1},
    clauseVis {Solver::clauseCnt, 1}
{}

bool STTNode::chooseNextVar() {
    bool exhaustedVars = false;
    switch (model[chosenVar]) {
        using TerVec::Value::Undef, TerVec::Value::False, TerVec::Value::True;
        case Undef: {
            model.set(chosenVar, False);
        } break;
        
        case False: {
            model.set(chosenVar, True);
        } break;
        
        case True: {
            // find next one to the right if there is
            // not sure what the logic here is
            s32 nextVar = model.findNext(chosenVar, Undef);
            if (nextVar < 0) {
                exhaustedVars = true;
            }
            else {
                chosenVar = nextVar;
            }
        } break;
    }

    return !exhaustedVars;
}
