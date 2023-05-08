#include "solver.hh"

bool Solver::init(std::string const & dimacs, Settings const & settings) {
    Solver::settings = settings;

    auto static constexpr isWS = [](char const * s) -> bool {
        return {*s == ' ' || *s == '\n'};
    };
    auto static constexpr isNewLine = [](char const * s) -> bool {
        return *s == '\n';
    };
    auto static constexpr skipWS = [](char const *& s) {
        while (isWS(s)) s++;
    };
    auto static constexpr isDigit = [](char const * d) -> bool {
        return {*d >= 0x30 && *d < 0x3A};
    };
    auto static constexpr digit = [](char const *& d) -> int {
        return {*(d++) - 0x30};
    };
    auto static constexpr ternary = [](char const *& d) -> TerVec::Value {
        TerVec::Value val {};
        switch (*(d++)) {
            case '0': val = TerVec::Value::False; break;
            case '1': val = TerVec::Value::True; break;
            case '-': val = TerVec::Value::Undef; break;
            default: break;
        }

        return val;
    };
    auto static constexpr integer = [](char const *& d) -> int {
        int s = 0;
        while (isDigit(d)) {
            s *= 10;
            s += digit(d);
        } return s;
    };

    char const * raw = dimacs.c_str();
    u32 varCnt;
    u32 clauseCnt;

    skipWS(raw);
    
    if (*raw == 'n') {
        varCnt = integer(raw += 2);
    } else {
        return false;
    } skipWS(raw);

    if (*raw == 'm') {
        clauseCnt = integer(raw += 2);
    } else {
        return false;
    } skipWS(raw);


    Solver::cdb = ClauseDB {varCnt, clauseCnt};

    u32 i = 0;
    while (*raw != '\0' && i < clauseCnt) {
        for (u32 j = 0; j < varCnt; j += 1) {
            Solver::cdb.set(i, j, ternary(raw));
        } skipWS(raw);
        i += 1;
    }

    if (i != clauseCnt) {
        Solver::cdb = {};
        return false;
    }

    return true;
}

void Solver::reset() {
    Solver::cdb = {};
    Solver::settings = {};
}

Solver::Result Solver::solve() {
    STTStack stack;
    stack.vec.reserve(128);
    stack.push(STTNode {});

    while (!stack.isEmpty()) {
        STTNode& current = stack.top();

        if (current.isMarked) {
            if (current.tryNextVal()) {
                stack.push(current);
            } else {
                // both values have been tried, no sat in this branch
                stack.pop();
            }
        }
        else if (!current.unitPropagate()) {
            // ran into a conflict in UP
            stack.pop();
        }
        else if (current.isSAT()) {
            return Result {Result::Stats {}, Result::Sat, {.sat {current.model}}};
        }
        else {
            current.chooseBranchVar();
            current.isMarked = true;
        }
    }

    // the whole ST was traversed (save the UP jumps) - unsat
    return Result {Result::Stats {}, Result::Unsat, {.unsat {"unsat"}}};
}

bool STTNode::tryNextVal() {
    bool exhaustedVals = false;

    if (branchVar < 0) {
        exhaustedVals = true;
    } else {
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







ClauseDB Solver::cdb {};
Solver::Settings Solver::settings {};