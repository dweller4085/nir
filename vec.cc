#include "vec.hh"

BinVec::BinVec(u32 len) :
    BinVecView {
        (u64 *) malloc (len / 64 + 1),
        len / 64 + 1,
        len
    }
{}

TerVec::TerVec(u32 len) :
    TerVecView {
        (u64 *) malloc (len / 32 + 1),
        len / 32 + 1,
        len
    } 
{}

BinVec::~BinVec() {
    free(words);
}

TerVec::~TerVec() {
    free(words);
}
