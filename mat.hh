#pragma once
#include "common.hh"
#include "vec.hh"

struct TerMat {
    u64 * vecs;
    u32 vec_cnt;
    u32 var_cnt;

    ~TerMat() {}
    void sort() {}

    TerVecView operator [] (u32);
};
