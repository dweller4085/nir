#include <chrono>
#include <iostream>
#include "solver.hh"
#include "stt.hh"

Solver::Result::Stats Solver::stats {};
ClauseDB Solver::theClauseDB {};
ClauseDB const& Solver::cdb {theClauseDB};

bool Solver::init(std::string const & cnf) {
    stats = {};

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
    stats = {};
}

Solver::Result Solver::solve() {
    STTStack stack;

    stack.vec.reserve(Solver::cdb.varCnt);
    stack.push(STTNode {});

    auto start = std::chrono::steady_clock::now();

    if constexpr (Solver::settings.modelTrace) {
        Solver::stats.modelTrace.root(stack.top().model);
    }

    while (!stack.isEmpty()) {
        STTNode& current = stack.top();

        if (current.isMarked) {
            if (current.setNextValue()) {
                stack.push(STTNode::nextAfter(current));

                if constexpr (Solver::settings.modelTrace) {
                    Solver::stats.modelTrace.BV(stack.top().model);
                }
                Solver::stats.nodesVisitedCnt += 1;

            } else {
                stack.pop();

                if constexpr (Solver::settings.modelTrace) {
                    if (!stack.isEmpty()) {
                        Solver::stats.modelTrace.BT(stack.top().model);
                    }
                }
            }
        }
        else if (current.hasConflict() || !current.unitPropagate()) {
            stack.pop();

            if constexpr (Solver::settings.modelTrace) {
                if (!stack.isEmpty()) {
                    Solver::stats.modelTrace.BT(stack.top().model);
                }
            }
        }
        else if (current.isSAT()) {
            Solver::stats.timeMs = std::chrono::duration<float, std::ratio<1, 1000>> {std::chrono::steady_clock::now() - start}.count();
            Solver::stats.sanityCheck = Solver::sanityCheck(current.model);
            return {Solver::stats, Result::Sat, {.sat {current.model}}};
        }
        else {
            current.chooseBranchVar();
            current.isMarked = true;
        }
    }

    Solver::stats.timeMs = std::chrono::duration<float, std::ratio<1, 1000>> {std::chrono::steady_clock::now() - start}.count();
    return {Solver::stats, Result::Unsat, {.unsat {}}};
}

Solver::Result::operator std::string() const {
    auto out = std::string {};
    auto statsMsg = std::format(
        "stats:\n"
        "    time: {:.3f}ms\n"
        "    nodes visited: {}\n"
        "    conflicts: {}\n",
        stats.timeMs,
        stats.nodesVisitedCnt,
        stats.conflictCnt
    );

    switch (type) {
        case Solver::Result::Sat: {
            out += "SAT\n" + (std::string) value.sat;
            statsMsg += std::format("    sanity check: {}\n", stats.sanityCheck);
        } break;
        case Solver::Result::Unsat: {
            out += "UNSAT";
        } break;
        case Solver::Result::Aborted: {
            out += std::string {"Aborted: "} + value.aborted;
        } break;
    }

    if constexpr (Solver::settings.modelTrace) {
        statsMsg += std::format("model trace:\n{}", stats.modelTrace.trace);
    }


    return out + "\n" + statsMsg;
}


bool Solver::sanityCheck(TerVec const& model) {
    for (u32 i = 0; i < Solver::cdb.clauseCnt; i += 1) {
        u32 j = 0;
        for (; j < Solver::cdb.varCnt; j += 1) {
            if (Solver::cdb.at(i, j) == model.at(j)) break;
        }

        if (j == Solver::cdb.varCnt) {
            return false;
        }
    }

    return true;
}
