#include "vec.hh"

BinVec::BinVec(u32 len):
    BinVecSlice {
        (u64 *) calloc ((len / 64 + 1), sizeof(u64)),
        len / 64 + 1,
        len
    }
{}

BinVec::BinVec(u32 len, bool value):
    BinVecSlice {
        (u64 *) malloc ((len / 64 + 1) * sizeof(u64)),
        len / 64 + 1,
        len
    } {
    int static constexpr fill[2] {
        0x00,
        0xFF
    };

    memset(words, fill[(int)value], wordCnt * sizeof(u64));
}

TerVec::TerVec(u32 len):
    TerVecSlice {
        (u64 *) calloc ((len / 32 + 1), sizeof(u64)),
        len / 32 + 1,
        len
    }
{}

TerVec::TerVec(u32 len, Value value):
    TerVecSlice {
        (u64 *) malloc ((len / 32 + 1) * sizeof(u64)),
        len / 32 + 1,
        len
    } {
    u64 static constexpr fill[3] {
        0x0000000000000000,
        0x5555555555555555,
        0xAAAAAAAAAAAAAAAA
    };

    for (u32 i = 0; i < wordCnt - 1; i += 1) {
        words[i] = fill[(u64)value];
    } words[wordCnt - 1] = fill[(u64)value] & 0xFFFFFFFFFFFFFFFF >> (32 - len % 32) * 2;
}

BinVec::~BinVec() {
    if (words) free(words);
}

TerVec::~TerVec() {
    if (words) free(words);
}

BinVec::BinVec(BinVec const & other) noexcept:
    BinVecSlice {
        (u64 *) malloc(other.wordCnt * sizeof(u64)),
        other.wordCnt,
        other.len
    } {
    memcpy(words, other.words, wordCnt * sizeof(u64));
}

TerVec::TerVec(TerVec const & other) noexcept:
    TerVecSlice {
        (u64 *) malloc(other.wordCnt * sizeof(u64)),
        other.wordCnt,
        other.len
    } {
    memcpy(words, other.words, wordCnt * sizeof(u64));
}

BinVec::BinVec(BinVec&& other) noexcept:
    BinVecSlice {
        other.words,
        other.wordCnt,
        other.len
    } {
    other.words = nullptr;
}

TerVec::TerVec(TerVec&& other) noexcept:
    TerVecSlice {
        other.words,
        other.wordCnt,
        other.len
    } {
    other.words = nullptr;
}
