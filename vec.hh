#pragma once
#include <intrin.h>
#include <string>
#include "common.hh"

/*  NOTE:
    The leftover bits in BinVec and TerVec are guaranteed to be all zeroes upon
    these objects instantiation.

    As long as pass only correct indexes to mutating methods, those bits should remain zero.

    There are functions that rely on this!
*/



struct BinVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;
    /*--------------*/
    bool at(u32 i) const {
        return _bittest64((__int64 *) words + i / 64, i % 64);
    }
    bool set(u32 i) {
        return _bittestandset64((__int64 *) words + i / 64, i % 64);
    }
    bool clear(u32 i) {
        return _bittestandreset64((__int64 *) words + i / 64, i % 64);
    }
    bool isAllZeros() const {
        u64 s = 0;
        for (u32 i = 0; i < wordCnt; i += 1) {
            s |= words[i];
        }

        return s == 0;
    }
};

struct TerVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;
    /*-------------*/
    enum class Value: u32 { False = 0b00, True = 0b01, Undef = 0b10 };
    Value at(u32 i) const noexcept {
        return TerVecSlice::Value {(words[i / 32] >> ((i % 32) * 2)) & u64 { 3 }};
    }
    void set(u32 i, Value v) noexcept {
        u64 const j = (i % 32) * 2;
        words[i / 32] = words[i / 32] & ~(u64 {3} << j) | (u64)v << j;
    }
    s32 findUndef() const noexcept {
        for (s32 i = 0; i < (s32)wordCnt; i += 1) {
            unsigned long index;
            if (_BitScanForward64(&index, words[i] & 0xAAAAAAAAAAAAAAAA)) {
                return {i * 32 + (s32)((index - 1) / 2)};
            }
        }

        return {-1};
    }
    u32 rang() const {
        u32 undefs = 0;
        for (u32 i = 0; i < wordCnt; i += 1) {
            undefs += __popcnt64(words[i] & 0xAAAAAAAAAAAAAAAA);
        } return len - undefs;
    }
};

struct BinVec : BinVecSlice {
    BinVec() = default;
    BinVec(u32 len);
    BinVec(u32 len, bool value);
    BinVec(BinVec const &) noexcept;
    BinVec(BinVec&&) noexcept;
    ~BinVec();
    operator std::string () const;
};

struct TerVec : TerVecSlice {
    TerVec() = default;
    TerVec(u32 len);
    TerVec(u32 len, Value value);
    TerVec(TerVec const &) noexcept;
    TerVec(TerVec&&) noexcept;
    ~TerVec();
    operator std::string () const;
};

using TerVec::Value::Undef, TerVec::Value::True, TerVec::Value::False;
