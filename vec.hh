#pragma once
#include "common.hh"

struct BinVecView {
    u64 * words;
    u32 wordCnt;
    u32 len;

    bool operator [] (u32) const;
};

struct TerVecView {
    u64 * words;
    u32 wordCnt;
    u32 len;

    enum class Value {True, False, Undef};
    Value operator [] (u32) const;
};

struct BinVec : BinVecView {
    //BinVec(BinVecView);
    BinVec(u32 len);
    ~BinVec();
};

struct TerVec : TerVecView {
    TerVec(u32 len);
    ~TerVec();
};
