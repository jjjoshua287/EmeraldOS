#ifndef X86_64_IDT_H
#define X86_64_IDT_H

#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <asm/desc.h>

/* holds things for the Interrupt Descriptor Table */
extern void *isr_stub_table[IDT_ENTRIES];

static inline void invalidate_idt()
{
    struct desc_ptr idt = {
        .size = 0,
        .addr = 0
    };
    load_idt(&idt);
}

#endif // X86_64_IDT_H