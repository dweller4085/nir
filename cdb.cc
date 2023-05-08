#include "cdb.hh"

ClauseDB::ClauseDB(u32 varCnt, u32 clauseCnt) noexcept:
    clauseCnt {clauseCnt},
    varCnt {varCnt},
    wordsPerVar {clauseCnt / 32 + 1} {
    clauses = (u64 *) calloc(wordsPerVar * varCnt, sizeof(u64));
}

ClauseDB::~ClauseDB() {
    if (clauses) free(clauses);
}

ClauseDB& ClauseDB::operator = (ClauseDB&& other) noexcept {
    if (clauses) free(clauses);
    clauses = other.clauses;
    clauseCnt = other.clauseCnt;
    varCnt = other.varCnt;
    wordsPerVar = other.wordsPerVar;
    other.clauses = nullptr;
    return *this;
}
