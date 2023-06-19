#pragma once
#include <intrin.h>
#include <string>
#include "common.hh"

enum Ternary { False = 0b00, True = 0b01, Undef = 0b10 };

struct BinVecSlice {
    static u32 wordCntFor(u32 len) { return (len - 1) / 64 + 1; }
    static usize memoryFor(u32 len) { return wordCntFor(len) * sizeof(u64); }
    BinVecSlice() = default;
    BinVecSlice(u32 len, u64 * words): len {len}, words {words}, wordCnt {wordCntFor(len)} {}
    BinVecSlice(u32 len, bool value, u64 * words): BinVecSlice {len, words} { init(value); }
    void init(bool value) {
        int static constexpr fill[2] {
            0x00,
            0xFF
        };

        memset(words, fill[(int) value], wordCnt * sizeof(u64));
        words[wordCnt - 1] &= 0xFFFFFFFFFFFFFFFF >> (64 - len % 64);
    }
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
    operator std::string () const;

    u64 * words;
    u32 wordCnt;
    u32 len;
};

struct TerVecSlice {
    static u32 wordCntFor(u32 len) { return (len - 1) / 32 + 1; }
    static usize memoryFor(u32 len) { return wordCntFor(len) * sizeof(u64); }
    TerVecSlice() = default;
    TerVecSlice(u32 len, u64 * words): len {len}, words {words}, wordCnt {wordCntFor(len)} {}
    TerVecSlice(u32 len, Ternary value, u64 * words): TerVecSlice {len, words} { init(value); }
    void init(Ternary value) {
        u32 static constexpr fill[3] {
            0x00,
            0x55,
            0xAA
        };

        memset(words, fill[(u64) value], wordCnt * sizeof(u64));
        words[wordCnt - 1] &= 0xFFFFFFFFFFFFFFFF >> (32 - len % 32) * 2;
    }
    Ternary at(u32 i) const noexcept {
        return (Ternary) ((words[i / 32] >> ((i % 32) * 2)) & u64 { 3 });
    }
    void set(u32 i, Ternary v) noexcept {
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
    Ternary isMonotone() const {
        /* impl... */
        return {};
    }
    operator std::string () const;

    u64 * words;
    u32 wordCnt;
    u32 len;
};

struct BinVec: BinVecSlice {
    BinVec() = default;
    BinVec(u32 len, bool value);
    BinVec(BinVec const&) noexcept;
    BinVec(BinVec&&) noexcept;
    ~BinVec();
};

struct TerVec: TerVecSlice {
    TerVec() = default;
    TerVec(u32 len, Ternary value);
    TerVec(TerVecSlice const&) noexcept;
    TerVec(TerVec const&) noexcept;
    TerVec(TerVec&&) noexcept;
    ~TerVec();
};
