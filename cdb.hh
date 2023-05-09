#pragma once
#include "vec.hh"

struct ClauseDB {
    u64 * clauses;
    u32 clauseCnt;
    u32 varCnt;
    u32 wordsPerVar;
    /*------------*/
    ClauseDB() = default;
    ClauseDB& operator = (ClauseDB&&) noexcept;
    ClauseDB(u32 varCnt, u32 clauseCnt) noexcept;
    ~ClauseDB();
    void set(u32 clause, u32 var, TerVec::Value val) {
        u64 const j = (clause % 32) * 2;
        u64 * const word = clauses + wordsPerVar * var + clause / 32;
        *word &= ~(u64 {0b11} << j);
        *word |= (u64) val << j;
    }
    // probably wrong
    TerVec::Value at(u32 clause, u32 var) const {
        u64 const word = *(clauses + wordsPerVar * var + clause / 32);
        return TerVec::Value {word >> ((clause % 32) * 2) & u64 {0b11}};
    }
    TerVecSlice column(u32 var) const {
        return {
            clauses + wordsPerVar * var,
            wordsPerVar,
            clauseCnt
        };
    }
};
