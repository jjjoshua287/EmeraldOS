#ifndef ASM_X86_64_MM_INTERNAL_H
#define ASM_X86_64_MM_INTERNAL_H

#include <asm/pgtable_types.h>
#include <emerald/types.h>

void *alloc_pgt(void);

phys_addr_t init_kernel_phys_mapping(phys_addr_t paddr_start, phys_addr_t paddr_end, 
				     unsigned long page_size_mask, pgprot_t prot);

#endif	/* ASM_X86_64_MM_INTERNAL_H */