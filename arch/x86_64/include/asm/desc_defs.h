// Hardware structures for the x86_64 Architecture
#ifndef X86_64_DESC_DEFS_H
#define X86_64_DESC_DEFS_H

#include <emerald/types.h>
#include <emerald/compiler_types.h>

/*
 * Low-Level interface mapping bits in bitfields to names
*/

/* Flags for non-system Segment Selectors */
#define _DESC_ACCESSED 		0x0001
#define _DESC_CODE_READABLE 	0x0002
#define _DESC_DATA_WRITABLE 	0x0002
#define _DESC_DATA_GROW_DOWN	0x0004
#define _DESC_CODE_CONFORMING 	0x0004
#define _DESC_CODE_EXECUTABLE 	0x0008

/* Common Flags */
#define _DESC_S 0x0010
#define _DESC_DPL(dpl) (((dpl) << 5))
#define _DESC_PRESENT  0x0080

/* Segment selector Flags */
#define _DESC_LONG_CODE 	0x2000
#define _DESC_DB		0x4000
#define _DESC_GRANULARITY_4K 	0x8000

/* System descriptors have a numeric "type" field instead of flags */
#define _DESC_SYSTEM(type) (type)

/*
 * High-level flag combinations
 */

#define _DESC_DATA (_DESC_S | _DESC_PRESENT | _DESC_ACCESSED | _DESC_DATA_WRITABLE)
#define _DESC_CODE (_DESC_S | _DESC_PRESENT | _DESC_ACCESSED | _DESC_CODE_READABLE | \
		    _DESC_CODE_EXECUTABLE)

#define DESC_TSS32 (_DESC_SYSTEM(9) | _DESC_PRESENT)

#define DESC_DATA64 (_DESC_DATA | _DESC_GRANULARITY_4K | _DESC_DB)
#define DESC_CODE64 (_DESC_CODE | _DESC_GRANULARITY_4K | _DESC_LONG_CODE)

#define DESC_USER (_DESC_DPL(3))

/* 8 byte GDT Segment Selector */
struct desc_struct {
	u16 limit0;
	u16 base0;
	u16 base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	u16 limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __packed;

#define GDT_ENTRY_INIT(flags, base, limit)			\
	{							\
		.limit0		= ((limit) >>  0) & 0xFFFF,	\
		.limit1		= ((limit) >> 16) & 0x000F,	\
		.base0		= ((base)  >>  0) & 0xFFFF,	\
		.base1		= ((base)  >> 16) & 0x00FF,	\
		.base2		= ((base)  >> 24) & 0x00FF,	\
		.type		= ((flags) >>  0) & 0x000F,	\
		.s		= ((flags) >>  4) & 0x0001,	\
		.dpl		= ((flags) >>  5) & 0x0003,	\
		.p		= ((flags) >>  7) & 0x0001,	\
		.avl		= ((flags) >> 12) & 0x0001,	\
		.l		= ((flags) >> 13) & 0x0001,	\
		.d		= ((flags) >> 14) & 0x0001,	\
		.g		= ((flags) >> 15) & 0x0001,	\
	}

enum {
	GATE_INTERRUPT = 0xE,
	GATE_TRAP = 0xF,
};

enum {
	DESC_TSS = 0x9,
	DESCTYPE_S = 0x10	/* !system */
};

/* Long Mode Segment Selector for Task State Segment */
struct sys_desc_struct {
	u16 limit0;
	u16 base0;
	u16 base1: 8, type: 5, dpl: 2, p: 1;
	u16 limit1: 4, zero: 3, g: 1, base2: 8;
	u32 base3;
	u32 reserved;
} __packed;

typedef struct sys_desc_struct tss_desc;

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
	    type  : 5,
	    dpl   : 2,
	    p     : 1;
} __packed;

/**
 * struct gate_struct - Hardware format for entries of the Interrupt Descriptor Table
 * @offset_low:     Bits 0..15 of offset
 * @segment:        Segment Selector that typically points to __KERNEL_CS from GDT
 * @bits:           IST index, Type, DPL, and Present Bits. See struct idt_bits for more info.
 * @offset_middle:  Bits 16..31 of offset
 * @offset_high:    Bits 32..63 of offset
 * @reserved:       Reserved bits (must be 0)
 */
struct gate_struct {
    	u16 offset_low;
    	u16 segment;
    	struct idt_bits bits;
    	u16 offset_middle;
    	u32 offset_high;
    	u32 reserved;
} __packed;

typedef struct gate_struct gate_desc;

/* Structure for IDT + GDT registers */
struct desc_ptr {
	unsigned short size;
	unsigned long addr;
} __packed;

#endif // X86_64_DESC_DEFS_H