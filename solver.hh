#pragma once
#include <vector>
#include <string>
#include "cdb.hh"

namespace solver {
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

    extern bool init(std::string const & dimacs, Settings const & settings);
    extern void reset();
    extern Result solve();

    extern ClauseDB cdb;
    extern Settings settings;
}
