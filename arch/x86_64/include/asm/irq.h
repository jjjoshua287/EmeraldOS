#ifndef ASM_X86_64_IRQ_H
#define ASM_X86_64_IRQ_H

#include <emerald/compiler.h>

static __always_inline void native_irq_disable(void)
{
    asm volatile("cli": : :"memory");
}

static __always_inline void native_irq_enable(void)
{
    asm volatile("sti": : :"memory");
}

static __always_inline void native_halt(void)
{
    asm volatile("hlt": : :"memory");
}

#endif /* ASM_X86_64_IRQ_H */