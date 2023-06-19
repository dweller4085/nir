#include "vec.hh"

BinVec::BinVec(u32 len, bool value):
    BinVecSlice {
        len,
        value,
        (u64 *) malloc (memoryFor(len)),
    } 
{}

TerVec::TerVec(u32 len, Ternary value):
    TerVecSlice {
        len,
        value,
        (u64 *) malloc (memoryFor(len)),
    }
{}

BinVec::~BinVec() {
    if (words) free(words);
}

TerVec::~TerVec() {
    if (words) free(words);
}

BinVec::BinVec(BinVec const& other) noexcept:
    BinVecSlice {
        other.len,
        (u64 *) malloc(other.wordCnt * sizeof(u64)),
    } {
    memcpy(words, other.words, wordCnt * sizeof(u64));
}

TerVec::TerVec(TerVec const& other) noexcept:
    TerVecSlice {
        other.len,
        (u64 *) malloc(other.wordCnt * sizeof(u64)),
    } {
    memcpy(words, other.words, wordCnt * sizeof(u64));
}

TerVec::TerVec(TerVecSlice const& other) noexcept:
    TerVecSlice {
    other.len,
    (u64 *) malloc(other.wordCnt * sizeof(u64)),
} {
    memcpy(words, other.words, wordCnt * sizeof(u64));
}

BinVec::BinVec(BinVec&& other) noexcept:
    BinVecSlice {
        other.len,
        other.words,
    } {
    other.words = nullptr;
}

TerVec::TerVec(TerVec&& other) noexcept:
    TerVecSlice {
        other.len,
        other.words,
    } {
    other.words = nullptr;
}

BinVecSlice::operator std::string () const {
    auto out = std::string {};
    for (u32 i = 0; i < len; i += 1) {
        switch (at(i)) {
            case false: out += '0'; break;
            case true: out += '1'; break;
        }
    } return out;
}

TerVecSlice::operator std::string () const {
    auto out = std::string {};
    for (u32 i = 0; i < len; i += 1) {
        switch (at(i)) {
            case False: out += '0'; break;
            case True: out += '1'; break;
            case Undef: out += '-'; break;
        }
    } return out;
}
