// Hardware structures for the x86_64 Architecture
#ifndef X86_64_DESC_DEFS_H
#define X86_64_DESC_DEFS_H

#include <emerald/types.h>
#include <emerald/compiler_types.h>

/**
 * struct idt_bits - Hardware format for IDT gate flags and types.
 * @ist: Interrupt Stack Table index.
 * @zero: Reserved bits, must be zero.
 * @type: Gate type designation (e.g., Interrupt vs Trap gate).
 * @zero1: Reserved bit, must be zero.
 * @dpl: Descriptor Privilege Level defining required CPU ring.
 * @p: Present flag, must be 1 for a valid descriptor.
 */
struct idt_bits {
	u16 ist   : 3,
	    zero  : 5,
	    type  : 4,
	    zero1 : 1,
	    dpl   : 2,
	    p     : 1;
} __packed;

/**
 * struct gate_struct - Hardware format for entries of the Interrupt Descriptor Table
 * @offset_low:  Bits 0..15 of offset
 * @segment:     Segment Selector that typically points to __KERNEL_CS from GDT
 * @bits:        IST index, Type, DPL, and Present Bits. See struct idt_bits for more info.
 * @offset_mid:  Bits 16..31 of offset
 * @offset_high: Bits 32..63 of offset
 * @reserved:    Reserved bits (must be 0)
 */
struct gate_struct {
    u16 offset_low;
    u16 segment;
    struct idt_bits bits;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} __packed;

typedef struct gate_struct gate_desc;

enum gate_type {
    INTERRUPT_GATE = 0xE,
    TRAP_GATE = 0xF
};

enum dpl_modes {
    DPL_KERNEL_MODE = 0,
    DPL_USER_MODE   = 3
};

#define G(_vector, _addr, _ist, _type, _dpl, _segment)   \
    {                                                    \
        .offset_low  = (u16)(unsigned long)(_addr),      \
        .segment     = (_segment),                       \
        .bits        = {                                 \
            .ist     = (_ist),                           \
            .type    = (_type),                          \
            .dpl     = (_dpl),                           \
            .p       = 1,                                \
        },                                               \
        .offset_mid  = (u16)((unsigned long)(_addr) >> 16), \
        .offset_high = (u16)((unsigned long)(_addr) >> 32), \
        .reserved    = 0,                                \
    }

#endif // X86_64_DESC_DEFS_H