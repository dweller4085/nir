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
            float time_ms;
        } stats;

        enum Type {
            Sat,
            Unsat,
            Aborted,
        } type;

        struct Value {
            TerVec sat;
            s32 unsat;
            char const * aborted;
        } value;

        operator std::string() const;
    };

    static bool init(std::string const & cnf, Settings const & settings);
    static void reset();
    static Result solve();

private:
    static ClauseDB theClauseDB;
    static Settings theSettings;

public:
    static ClauseDB const& cdb;
    static Settings const& settings;
};
