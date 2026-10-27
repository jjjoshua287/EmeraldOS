#ifndef ASM_X86_64_PAGE_H
#define ASM_X86_64_PAGE_H

#include <asm/pgtable64_types.h>

#define PAGE_OFFSET 	0xffff888000000000

#define PAGE_SHIFT 	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)	/* 4 KiB */
#define HPAGE_SIZE 	PMD_SIZE

/* Early direct map ends at 4 GiB */
#define EARLY_MAP_END 	(4UL << 30)

#ifndef __ASSEMBLER__
#include <emerald/types.h>

#define __va(x) 	((void *)((unsigned long)(x) + PAGE_OFFSET))
#define __pa(x) 	((unsigned long)(x) - PAGE_OFFSET)
#define __pa_symbol(x)	((unsigned long)(x) - KERNEL_VMA)

/* convert a physical address into a virtual address. The address must be directly mapped */
static inline void *phys_to_virt(phys_addr_t addr)
{
	return __va(addr);
}

static inline phys_addr_t virt_to_phys(const void *addr) {
	return ((unsigned long)addr >= KERNEL_VMA) ? __pa_symbol(addr) : __pa(addr);
}

#endif /* !__ASSEMBLER__ */

#endif	/* ASM_X86_64_PAGE_H */