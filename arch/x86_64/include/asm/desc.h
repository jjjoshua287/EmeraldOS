#ifndef X86_64_DESC_H
#define X86_64_DESC_H

#include <asm/desc_defs.h>
#include <asm/segment.h>
#include <asm/processor.h>

#include <emerald/string.h>

static inline void pack_gate(gate_desc *gate, unsigned type, unsigned long func, 
                             unsigned dpl, unsigned ist)
{
	gate->offset_low = (u16)func;
	gate->bits.p = 1;
	gate->bits.dpl = dpl;
	gate->bits.zero = 0;
	gate->bits.type = type;
	gate->offset_middle = (u16)(func >> 16);
	gate->segment = __KERNEL_CS;
	gate->bits.ist = ist;
	gate->reserved = 0;
	gate->offset_high = (u16)(func >> 32);
}

#define write_gdt_entry(dt, entry, desc, type)  native_write_gdt_entry(dt, entry, desc, type)
#define write_idt_entry(dt, entry, g)           native_write_idt_entry(dt, entry, g)

static inline void native_write_idt_entry(gate_desc *idt, int entry, const gate_desc *gate)
{
        memcpy(&idt[entry], gate, sizeof(*gate));
}

static inline void native_write_gdt_entry(struct desc_struct *gdt, int entry, const void *desc, int type)
{
        unsigned int size;
        if (type == DESC_TSS)
                size = sizeof(tss_desc);
        else
                size = sizeof(*gdt);

        memcpy(&gdt[entry], desc, size);
}

static inline void set_sys_descriptor(void *d, unsigned long addr, unsigned type, unsigned size)
{
        struct sys_desc_struct *desc = d;
        memset(desc, 0, sizeof(*desc));

	desc->limit0 = (u16)size;
        desc->base0  = (u16)addr;
        desc->base1  = (addr >> 16) & 0xFF;
        desc->type   = type;
        desc->p      = 1;
        desc->limit1 = (size >> 16) & 0xF;
        desc->base2  = (addr >> 24) & 0xFF;
        desc->base3  = (u32)(addr >> 32);
}

static inline void __set_tss_desc(struct desc_struct *d, unsigned entry, struct x86_hw_tss *addr)
{
        tss_desc tss;

        /* TODO: use a variable to limit size of TSS when io bitmap is implemented */
        set_sys_descriptor(&tss, (unsigned long)addr, DESC_TSS, sizeof(struct x86_hw_tss) - 1);
        write_gdt_entry(d, entry, &tss, DESC_TSS);
}

#define set_tss_desc(dt, addr) __set_tss_desc(dt, GDT_ENTRY_TSS, addr)

#endif // X86_64_DESC_H