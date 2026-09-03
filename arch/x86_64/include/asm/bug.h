#ifndef ASM_x86_64_BUG_H
#define ASM_x86_64_BUG_H

#include <emerald/compiler.h>

#define BUG() __asm__ volatile("ud2")

#define BUG_ON(cond) do {   \
    if (unlikely(cond))     \
        BUG()               \
} while(0)                  \

#endif