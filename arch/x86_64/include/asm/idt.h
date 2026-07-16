#ifndef X86_64_IDT_H
#define X86_64_IDT_H

#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <asm/desc.h>

static inline void invalidate_idt()
{
    struct desc_ptr idt = {
        .size = 0,
        .addr = 0
    };
    load_idt(&idt);
}

#endif // X86_64_IDT_H