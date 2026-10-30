#ifndef ASM_X86_64_TLB_H
#define ASM_X86_64_TLB_H

static inline void invlpg(unsigned long addr)
{
	asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

#endif	/* ASM_X86_64_TLB_H */