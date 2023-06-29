#pragma once
#include <intrin.h>
#include <string>
#include "common.hh"
#include "memory.hh"

enum Ternary: u64 { False = 0b00, True = 0b01, Undef = 0b10 };

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
    bool isAllZeroes() const {
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
    TerVecSlice(void * memory, TerVecSlice const& other) {
        // assumes size of `memory` >= `other.words`
        words = (u64 *) memory;
        len = other.len;
        wordCnt = other.wordCnt;
        memcpy(words, other.words, wordCnt * sizeof(u64));
    }
    TerVecSlice(u32 len, void * words): len {len}, words {(u64 *) words}, wordCnt {wordCntFor(len)} {}
    TerVecSlice(u32 len, Ternary value, void * words): TerVecSlice {len, words} { init(value); }
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
        return (Ternary) ((words[i / 32] >> ((i % 32) * 2)) & u64 { 0b11 });
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
        u32 r = len;
        for (u32 i = 0; i < wordCnt; i += 1) {
            r -= __popcnt64(words[i] & 0xAAAAAAAAAAAAAAAA);
        } return r;
    }
    bool isEmpty() const { return rang() == 0; }
    void set(u32 i) {
        _bittestandset64((__int64 *) words + i / 32, (i % 32) * 2);
        _bittestandreset64((__int64 *) words + i / 32, (i % 32) * 2 + 1);
    }
    void clear(u32 i) {
        _bittestandreset64((__int64 *) words + i / 32, (i % 32) * 2);
        _bittestandreset64((__int64 *) words + i / 32, (i % 32) * 2 + 1);
    }
    bool isAllZeroes() const {
        u64 s = 0;
        for (u32 i = 0; i < wordCnt; i += 1) {
            s |= words[i];
        }

        return s == 0;
    }
    Ternary isMonotone() const {
        if (rang() > 0) {
            if (countOnes() == 0) {
                return False;
            } else if (countZeroes() == 0) {
                return True;
            }
        }
        
        return Undef;
    }
    u32 countOnes() const {
        u32 count = 0;
        for (u32 i = 0; i < wordCnt; i += 1) {
            u64 mask = (words[i] & 0xAAAAAAAAAAAAAAAA);
            mask |= mask >> 1;
            mask = ~mask;
            count += __popcnt64(words[i] & mask);
        } return count;
    }
    u32 countZeroes() const {
        // bleh
        return rang() - countOnes();
    }

    s32 isUnit() const {
        // see if this clause is a unit clause, if it is - return the def var index, else -1.

        //it's fine it's fine
        if (rang() != 1) return -1;

        u32 i = 0;
        unsigned long j = 0;

        while (!_BitScanForward64(&j, ~words[i] & 0xAAAAAAAAAAAAAAAA)) {
            i += 1;
        }

        return 32 * i + (j - 1) / 2;
    }
    void applyVis(TerVecSlice const& vis) {
        // assumes `this->len` == `vis.len`
        // for each `False` component in `vis` set the corresponding component here to `Undef`.
        // and for each `True` component in `vis` leave the coresspond. comp. here as is.
        for (u32 i = 0; i < wordCnt; i += 1) {
            u64 mask = vis.words[i] | vis.words[i] << 1;
            words[i] = words[i] & mask | 0xAAAAAAAAAAAAAAAA & ~mask;
        }

        // clean up the mess in the leftover bits :P
        words[wordCnt - 1] &= 0xFFFFFFFFFFFFFFFF >> (32 - len % 32) * 2;
    }

    operator std::string () const;

    u64 * words;
    u32 wordCnt;
    u32 len;
};

// template<typename T>
struct BinVec: BinVecSlice {
    BinVec() = default;
    BinVec(u32 len, bool value);
    BinVec(BinVec const&) noexcept;
    BinVec(BinVec&&) noexcept;
    ~BinVec();
};

// template<typename T>
struct TerVec: TerVecSlice {
    TerVec() = default;
    TerVec(u32 len, Ternary value);
    TerVec(TerVecSlice const&) noexcept;
    TerVec(TerVec const&) noexcept;
    TerVec(TerVec&&) noexcept;
    ~TerVec();
};

// here's an idea if I'll ever need to have custom alloc
//    gStackAllocator for the binvecs/terves in sttnodes
//    a little scratch for temp calculations
/*
template<> TerVec<StackAllocator alloc?>::TerVec(u32 len, Ternary value):
    TerVecSlice {len, value, alloc.alloc(TerVecSlice::memoryFor(len))}
{}

template<> TerVec<StackAllocator>::~TerVec() {}

template<> TerVec<void>::~TerVec() {
    if (words) free(words);
}

*/
