#include "mat.hh"

TerMat::TerMat(u32 vecCnt, u32 var_cnt) : vecCnt {vecCnt}, varCnt {varCnt} {
    usize size = (var_cnt / 32 + 1) * vecCnt * sizeof(u64);
    size += 64 - size % 64;
    vecs = (u64 *) _aligned_malloc(size, 64);
}

TerMat::~TerMat() {
    _aligned_free(vecs);
}

TerVecView TerMat::operator[] (u32 i) {
    return TerVecView {
        vecs + (vecCnt / 32 + 1) * i,
        (vecCnt / 32 + 1),
        vecCnt
    };
}
