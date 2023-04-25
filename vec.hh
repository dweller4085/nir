#pragma once
#include "common.hh"

struct BinVecView {
    u64 * words;
    u32 word_cnt;
    u32 len;

    u64 popcnt();
    bool operator [] (u32);
};

struct TerVecView {
    u64 * words;
    u32 word_cnt;
    u32 len;

    enum class Value {True, False, Undef};
    Value operator [] (u32);
};

struct BinVec : BinVecView {
    ~BinVec();
};

struct TerVec : TerVecView {
    ~TerVec();
};
