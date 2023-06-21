#pragma once
#include "common.hh"
#include <stdlib.h>

struct Scratch {
    static u8 * const memory;
    static u8 * head;
    static u8 * tail;

    static void push() { tail = head; }
    static void pop() { head = tail; }
    static void * salloc(usize size) {
        head += size;
        return head - size;
    }

    Scratch() { push(); }
    ~Scratch() { pop(); }

    void * alloc(usize size) { return salloc(size); }
};
