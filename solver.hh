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
            uint64_t time_ms;
        } stats;

        enum {
            Sat,
            Unsat,
            Aborted,
        } type;

        struct {
            TerVec sat;
            char const * unsat;
            char const * aborted;
        } value;
    };

    static bool init(std::string const & dimacs, Settings const & settings);
    static void reset();
    static Result solve();

    static ClauseDB cdb;
    static Settings settings;
};
