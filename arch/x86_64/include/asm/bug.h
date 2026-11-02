#ifndef ASM_x86_64_BUG_H
#define ASM_x86_64_BUG_H

#include <emerald/printk.h>
#include <emerald/compiler.h>

#define BUG() __asm__ volatile("ud2")

#define BUG_ON(cond) do {   \
    if (unlikely(cond))     \
        BUG();              \
} while(0)                  \

#define WARN() printk("WARNING in %s at line %d\n", __func__, __LINE__)

#define WARN_ON(cond) do {  \
    if (unlikely(cond))     \
        WARN();             \
} while(0)

#endif