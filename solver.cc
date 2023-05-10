#include "solver.hh"
#include "stt.hh"

Solver::Settings Solver::theSettings {};
ClauseDB Solver::theClauseDB {};
Solver::Settings const& Solver::settings {theSettings};
ClauseDB const& Solver::cdb {theClauseDB};

bool Solver::init(std::string const & cnf, Settings const & settings) {
    theSettings = settings;

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

    char const * raw = cnf.c_str();
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


    theClauseDB = ClauseDB {varCnt, clauseCnt};

    u32 i = 0;
    while (*raw != '\0' && i < clauseCnt) {
        for (u32 j = 0; j < varCnt; j += 1) {
            theClauseDB.set(i, j, ternary(raw));
        } skipWS(raw);
        i += 1;
    }

    if (i != clauseCnt) {
        theClauseDB = {};
        return false;
    }

    return true;
}

void Solver::reset() {
    theClauseDB = {};
    theSettings = {};
}

Solver::Result Solver::solve() {
    STTStack stack;

    stack.vec.reserve(Solver::cdb.varCnt);
    stack.push(STTNode {});

    while (!stack.isEmpty()) {
        STTNode& current = stack.top();

        if (current.isMarked) {
            if (auto nextValue = current.nextBVValue(); nextValue != TerVec::Value::Undef) {
                stack.push(STTNode::nextAfter(current, nextValue));
            } else {
                stack.pop();
            }
        }
        else if (current.hasConflict() || !current.unitPropagate()) {
            stack.pop();
        }
        else if (current.isSAT()) {
            return {Result::Stats {}, Result::Sat, {.sat {current.model}}};
        }
        else {
            current.chooseBranchVar();
            current.isMarked = true;
        }
    }

    return {Result::Stats {}, Result::Unsat, {.unsat {"unsat"}}};
}
