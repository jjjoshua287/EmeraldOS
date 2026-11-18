#ifndef ASM_X86_64_TLB_H
#define ASM_X86_64_TLB_H

static inline void invlpg(unsigned long addr)
{
	asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

static inline void flush_tlb(void)
{
	asm volatile("movq %%cr3, %%rax; movq %%rax, %%cr3" ::: "rax", "memory");
}

#endif	/* ASM_X86_64_TLB_H */