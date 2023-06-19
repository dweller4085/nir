#pragma once
#include "common.hh"
#include <stdlib.h>

struct FrameAllocator {
    FrameAllocator() = default;

    FrameAllocator(usize frameSize, u32 slotCnt): frameSize {frameSize} {
        memory = (u8 *) malloc(frameSize * slotCnt);
        curr = frame = memory;
    }

    FrameAllocator& operator = (FrameAllocator const&) = default;

    FrameAllocator& operator = (FrameAllocator&& other) noexcept {
        if (memory) free(memory);
        *this = std::as_const(other);
        other.memory = nullptr;
        return *this;
    }

    ~FrameAllocator() {
        if (memory) free(memory);
    }

    void push() {
        frame += frameSize;
        curr = frame;
    }

    void pop() {
        frame -= frameSize;
        curr = frame;
    }

    void * alloc(usize size) {
        curr += size;
        return curr - size;
    }

    u8 * memory;
    u8 * frame;
    u8 * curr;
    usize frameSize;
};
