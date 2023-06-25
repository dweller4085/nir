#pragma once
#include "common.hh"
#include <stdlib.h>

struct Scratch {
    static u8 * memory;
    static u8 * head;
    static u8 * tail;

    static void init(usize size) {
        if (memory) free(memory);
        tail = head = memory = (u8 *) malloc(size);
    }
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
