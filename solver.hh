#pragma once
#include "common.hh"
#include "mat.hh"

struct Solver {
    struct Settings {
        u64 timeout_ms;
    };

    struct Result {
        struct {
            uint64_t time_ms;
        } stats;

        enum {
            Sat,
            Unsat,
            Aborted,
        } type;

        union {
            TerVecView sat;
            char const * unsat;
            char const * aborted;
        };
    };

    static Solver from_dimacs (char const *) throw (std::invalid_argument) {
        return {};
    }

    Solver&& with_settings (Settings&& settings) && noexcept {
        self.settings = settings;
        return std::move(self);
    }

    Result solve() noexcept { return {}; }

    Settings settings;
    TerMat mat;
};
