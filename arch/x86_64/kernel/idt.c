#include <asm/desc_defs.h>
#include <asm/desc.h>
#include <asm/exceptions.h>
#include <asm/segment.h>
#include <asm/idtentry.h>

#include <emerald/types.h>

enum {
        DPL0 = 0x0,
        DPL3 = 0x3
};

#define DEFAULT_STACK 0

#define G(_vector, _addr, _ist, _type, _dpl, _segment)  \
        {                                               \
                .vector    = _vector,                   \
                .segment   = _segment,                  \
                .addr      = _addr,                     \
                .bits.ist  = _ist,                      \
                .bits.type = _type,                     \
                .bits.dpl  = _dpl,                      \
                .bits.p    = 1,                         \
        }

/* Standard Interrupt Gate */
#define INTG(_vector, _addr)                            \
        G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL0, __KERNEL_CS)

/* System Interrupt Gate */
#define SYSG(_vector, _addr)                            \
        G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL3, __KERNEL_CS)

/* Interrupt Gate with Interrupt Stack, The _ist index is the index in the
 * tss.ist[] array. The descriptor needs it to start at 1.
 */
#define ISTG(_vector, _addr, _ist)                      \
        G(_vector, _addr, _ist + 1, GATE_INTERRUPT, DPL0, __KERNEL_CS)

#define IDT_TABLE_SIZE (IDT_ENTRIES * sizeof(gate_desc))

gate_desc idt_table[IDT_ENTRIES];

struct desc_ptr idt_desc = {
        .size = IDT_TABLE_SIZE - 1,
        .addr = (u64)idt_table
};

void invalidate_idt(void)
{
    static const struct desc_ptr idt = {
        .size = 0,
        .addr = 0
    };
    load_idt(&idt);
}