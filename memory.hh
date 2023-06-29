#pragma once
#include "common.hh"
#include <stdlib.h>

struct Scratch {
    static u8 * memory;
    static u8 * head;
    static u8 ** frames;
    static usize currentFrame;

    static void init(usize size, usize depth) {
        if (memory) free(memory);
        head = memory = (u8 *) malloc(size);
        frames = (u8 **) malloc(depth * (sizeof (u8 *)));
        currentFrame = 0;
    }

    static void push() {
        frames[currentFrame++] = head;
    }

    static void * push(usize size, usize align) {
        push();
        return salloc(size, align);
    }

    static void pop() {
        head = frames[--currentFrame];
    }

    static void * salloc(usize size, usize align) {
        usize padding = (((usize)head - 1) / align + 1) * align - (usize)head;
        head += size + padding;
        return head - size;
    }

    Scratch() { push(); }
    Scratch(usize align) { push(0, align); }
    ~Scratch() { pop(); }

    void * alloc(usize size, usize align) { return salloc(size, align); }

    template<typename T>
    T * alloc(usize count = 1) {
        return (T *) salloc(sizeof(T) * count, alignof(T));
    }
};
