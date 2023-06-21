#include "memory.hh"

u8 * const Scratch::memory = (u8 *) malloc(1024);
u8 * Scratch::head = Scratch::memory;
u8 * Scratch::tail = Scratch::memory;
