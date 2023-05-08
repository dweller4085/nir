#include "vec.hh"

BinVec::BinVec(u32 len) :
    BinVecSlice {
        (u64 *) calloc ((len / 64 + 1), sizeof(u64)),
        len / 64 + 1,
        len
    }
{}

BinVec::BinVec(u32 len, bool value) :
    BinVecSlice {
        (u64 *) malloc ((len / 64 + 1) * sizeof(u64)),
        len / 64 + 1,
        len
    }
{
    memset(words, (int) value, (len / 64 + 1) * sizeof(u64));
}

TerVec::TerVec(u32 len) :
    TerVecSlice {
        (u64 *) calloc ((len / 32 + 1), sizeof(u64)),
        len / 32 + 1,
        len
    }
{}

BinVec::~BinVec() {
    if (words) free(words);
}

TerVec::~TerVec() {
    if (words) free(words);
}
