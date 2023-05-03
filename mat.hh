#pragma once
#include "common.hh"
#include "vec.hh"

struct TerMat {
    u64 * vecs;
    u32 vecCnt;
    u32 varCnt;

    TerMat() = default;
    TerMat(u32 vecCnt, u32 varCnt);
    ~TerMat();

    TerVecView operator [] (u32);
};
