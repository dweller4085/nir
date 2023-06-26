#include <chrono>
#include <iostream>
#include "solver.hh"
#include "stt.hh"
#include "memory.hh"

Solver::Result::Stats Solver::stats {};
ClauseDB Solver::cdb {};

bool Solver::init(std::string const& cnf) {
    stats = {};
    
    bool ok = false;
    cdb = ClauseDB {cnf, ok};
    
    if (ok) {
        Scratch::init(4 * TerVecSlice::memoryFor(cdb.varCnt) + 4 * TerVecSlice::memoryFor(cdb.clauseCnt));
    }
    
    return ok;
}

void Solver::reset() {
    cdb = {};
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

                //continue;
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

bool Solver::sanityCheck(TerVecSlice const& model) {
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
