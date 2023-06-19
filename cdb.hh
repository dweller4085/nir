#pragma once
#include "vec.hh"

struct ClauseDB {
    ClauseDB() = default;
    ClauseDB(std::string const& cnf, bool& ok);
    ClauseDB& operator = (ClauseDB&&) noexcept;
    ~ClauseDB();
    
    TerVecSlice column(u32 var) const {
        return {
            clauseCnt,
            cols.matrix + cols.words * var,
        };
    }
    
    TerVecSlice clause(u32 clause) const {
        return {
            varCnt,
            rows.matrix + rows.words * clause,
        };
    }

    Ternary at(u32 clause, u32 var) const {
        u64 const word = *(cols.matrix + cols.words * var + clause / 32);
        return (Ternary) (word >> ((clause % 32) * 2) & u64 { 0b11 });
    }

    u32 clauseCnt;
    u32 varCnt;
    struct {
        u64 * matrix;
        u32 words;
    } rows, cols;
};
