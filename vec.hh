#pragma once
#include "common.hh"
#include <intrin.h>

struct BinVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;

    bool at(u32) const;
    bool isEmpty() const;
    void set(u32, bool);
    s32 find(bool) const;
};

struct TerVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;

    enum class Value : u32 { False = 0b00, True = 0b01, Undef = 0b10, Def = 0b11 };

    inline Value at(u32 i) const noexcept {
        return TerVecSlice::Value {(words[i / 32] >> ((i % 32) * 2)) & u64 { 3 }};
    }

    inline void set(u32 i, Value v) noexcept {
        u64 const j = (i % 32) * 2;
        words[i / 32] = words[i / 32] & ~(u64 {3} << j) | (u64)v << j;
    }

    inline s32 findUndef() const noexcept {
        // should take into consideration possible leftover garbage in the last word
        // although if we don't touch those bits, they will stay at zero, and so won't
        // have an effect here, since it searches for the first set bit with odd index
        u64 constexpr mask {0xAAAAAAAAAAAAAAAA};
        for (s32 i = 0; i < (s32)wordCnt; i += 1) {
            unsigned long index;
            if (_BitScanForward64(&index, words[i] & mask)) {
                return {i * 32 + (s32)((index - 1) / 2)};
            }
        }

        return {-1};
    }
};

struct BinVec : BinVecSlice {
    //BinVec(BinVecSlice);
    BinVec() = default;
    BinVec(u32 len);
    BinVec(u32 len, bool value);
    BinVec(BinVec const &);
    ~BinVec();
};

struct TerVec : TerVecSlice {
    TerVec() = default;
    TerVec(u32 len);
    TerVec(TerVec const &);
    ~TerVec();
};
