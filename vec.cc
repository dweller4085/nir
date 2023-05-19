#include "vec.hh"

BinVec::BinVec(u32 len):
    BinVecSlice {
        (u64 *) calloc (((len - 1) / 64 + 1), sizeof(u64)),
        (len - 1) / 64 + 1,
        len
    }
{}

BinVec::BinVec(u32 len, bool value):
    BinVecSlice {
        (u64 *) malloc (((len - 1) / 64 + 1) * sizeof(u64)),
        (len - 1) / 64 + 1,
        len
    } {
    int static constexpr fill[2] {
        0x00,
        0xFF
    };

    memset(words, fill[(int)value], wordCnt * sizeof(u64));
    words[wordCnt - 1] &= 0xFFFFFFFFFFFFFFFF >> (64 - len % 64);
}

TerVec::TerVec(u32 len):
    TerVecSlice {
        (u64 *) calloc (((len - 1) / 32 + 1), sizeof(u64)),
        (len - 1) / 32 + 1,
        len
    }
{}

TerVec::TerVec(u32 len, Value value):
    TerVecSlice {
        (u64 *) malloc (((len - 1) / 32 + 1) * sizeof(u64)),
        (len - 1) / 32 + 1,
        len
    } {
    u64 static constexpr fill[3] {
        0x00,
        0x55,
        0xAA
    };

    memset(words, fill[(u64)value], wordCnt * sizeof(u64));
    words[wordCnt - 1] &= 0xFFFFFFFFFFFFFFFF >> (32 - len % 32) * 2;
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

BinVec::operator std::string () const {
    auto out = std::string {};
    for (u32 i = 0; i < len; i += 1) {
        switch (at(i)) {
            case false: out += '0'; break;
            case true: out += '1'; break;
        }
    } return out;
}

TerVec::operator std::string () const {
    auto out = std::string {};
    for (u32 i = 0; i < len; i += 1) {
        switch (at(i)) {
            case Value::False: out += '0'; break;
            case Value::True: out += '1'; break;
            case Value::Undef: out += '-'; break;
        }
    } return out;
}
