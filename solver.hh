#pragma once
#include <vector>
#include <string>
#include "cdb.hh"

struct Solver {
    struct Settings {
        u64 timeout_ms;
    };
    struct Result {
        struct Stats {
            float timeMs {0.f};
            u32 nodesVisitedCnt {1};
            u32 conflictCnt {0};
            bool sanityCheck {false};
            std::string modelTrace {};
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

    static bool init(std::string const & cnf, Settings const & settings);
    static void reset();
    static Result solve();
    static bool sanityCheck(TerVec const& model);

private:
    static ClauseDB theClauseDB;
    static Settings theSettings;

public:
    static ClauseDB const& cdb;
    static Settings const& settings;
    static Result::Stats stats;
};
