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
    TerVec::Value at(u32 clause, u32 var) const {
        // todo!
    }
    TerVecSlice col(u32 var) {
        // todo!
    }
};

struct CDBView {
    BinVec varVis;
    BinVec clauseVis;
    ClauseDB const& cdb;
    /*-------------*/
    CDBView(ClauseDB const& cdb): varVis {cdb.varCnt, 1}, clauseVis {cdb.clauseCnt, 1}, cdb {cdb} {}
    CDBView(CDBView const &) = default;
    CDBView(CDBView&&) = default;
    ~CDBView() = default;
    void apply(u32 var); // F|A application from the book? Instead of manually setting Vis
};
