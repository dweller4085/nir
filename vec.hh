#pragma once
#include "common.hh"

struct BinVecView {
    u64 * words;
    u32 wordCnt;
    u32 len;

    bool operator [] (u32) const;
    bool isEmpty() const;
    void set(u32, bool);
    s32 find(bool);
};

struct TerVecView {
    u64 * words;
    u32 wordCnt;
    u32 len;

    enum class Value {True, False, Undef, Def};
    Value operator [] (u32) const;
    void set(u32, Value);
    s32 find(Value);
};

struct BinVec : BinVecView {
    //BinVec(BinVecView);
    BinVec(u32 len);
    BinVec(u32 len, bool value);
    ~BinVec();
};

struct TerVec : TerVecView {
    TerVec() = default;
    TerVec(u32 len);
    ~TerVec();
};
