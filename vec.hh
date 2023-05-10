#pragma once
#include <intrin.h>
#include <string>
#include "common.hh"

struct BinVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;
    /*--------------*/
    inline bool at(u32 i) const {
        return _bittest64((__int64 *) words + i / 64, i % 64);
    }
    inline bool set(u32 i) {
        return _bittestandset64((__int64 *) words + i / 64, i % 64);
    }
    inline bool clear(u32 i) {
        return _bittestandreset64((__int64 *) words + i / 64, i % 64);
    }
    inline bool isAllZeros() const {
        u64 s = 0;
        for (u32 i = 0; i < wordCnt; i += 1) {
            s |= words[i];
        }

        // mask off garbage at leftover bits
        // (if there ever is garbage with current usage)
        // we only ever set individual bits, never shift or xor or anything
        // so those leftover bits are stuck as 0 from the calloc to the end
        // AND we're explicitely zeroing out them at the creation
        //s |= words[wordCnt - 1] & (0xFFFFFFFFFFFFFFFF >> (64 - len % 64));

        return s == 0;
    }
};

struct TerVecSlice {
    u64 * words;
    u32 wordCnt;
    u32 len;
    /*-------------*/
    enum class Value : u32 { False = 0b00, True = 0b01, Undef = 0b10 };
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
