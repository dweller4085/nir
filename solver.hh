#pragma once
#include <vector>
#include <string>
#include <format>
#include <iostream>
#include "cdb.hh"

struct Solver {
    struct Settings {
        f32 timeout_ms;
        bool modelTrace;
    };
    struct Result {
        struct Stats {
            float timeMs {0.f};
            u32 nodesVisitedCnt {1};
            u32 conflictCnt {0};
            bool sanityCheck {false};
            struct {
                std::string trace;
                s32 level {0};
                /*------------------*/
                void root(TerVec const& model) {
                    //trace += std::format("{}    (0)\n", (std::string) model);
                    std::cerr << std::format("{}    (0)\n", (std::string) model);
                }
                void BV(TerVec const& model) {
                    //trace += std::format("{} BV ({} -> {})\n", (std::string) model, level, level + 1);
                    std::cerr << std::format("{} BV ({} -> {})\n", (std::string) model, level, level + 1);
                    level += 1;
                }
                void UP(TerVec const& model) {
                    //trace += std::format("{} UP\n", (std::string) model);
                    std::cerr << std::format("{} UP\n", (std::string) model);
                }
                void BT(TerVec const& model) {
                    //trace += std::format("{} BT ({} -> {})\n", (std::string) model, level, level - 1);
                    std::cerr << std::format("{} BT ({} -> {})\n", (std::string) model, level, level - 1);
                    level -= 1;
                }
            } modelTrace {};
        } stats;

        enum Type {
            Sat,
            Unsat,
            Aborted,
        } type;

        struct Value {
            TerVec sat;
            s32 unsat;
            std::string aborted;
        } value;

        operator std::string() const;
    };

    static bool init(std::string const & cnf);
    static void reset();
    static Result solve();
    static bool sanityCheck(TerVec const& model);

private:
    static ClauseDB theClauseDB;

public:
    static ClauseDB const& cdb;
    static Result::Stats stats;
    static constexpr Settings settings {
        .timeout_ms = -1.f,
        .modelTrace = true
    };
};
