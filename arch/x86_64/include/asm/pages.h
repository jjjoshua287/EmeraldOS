#ifndef X86_64_ASM_PAGES_H
#define X86_64_ASM_PAGES_H

#define PT_SIZE         4096
#define PT_ENTRIES      512
#define PT_ENTRY_SIZE   8

#define KERNEL_PAGE_SIZE 0x200000    /* 2 MiB */

/* Page Table Entry (PTE) Bits */
#define PTE_P   1       /* Present Bit */
#define PTE_RW  2       /* Read/Write Bit */
#define PTE_US  4       /* User/Supervisor Bit */
#define PTE_PWT 8       /* Write-Through Bit */
#define PTE_PCD 16      /* Cache Disable Bit */
#define PTE_A   32      /* Accessed Bit */
#define PTE_D   64      /* Dirty Bit */
#define PTE_PS  128     /* Page Size Bit */
#define PTE_G   256     /* Global Bit */

/* Combinations of PTE bits */
#define PTE_KERNEL (PTE_P | PTE_RW)
#define PTE_KERNEL_BIG (PTE_KERNEL | PTE_PS)
#define PTE_MMIO (PTE_P | PTE_RW | PTE_PWT | PTE_PCD | PTE_A | PTE_PS)

/* hard coded, but determined from KERNEL_VMA - KERNEL_LMA */
#define PAGE_OFFSET 0xffffffff7fe00000

#ifndef __ASSEMBLER__
#include <emerald/types.h>

#define __va(x) ((void *)(unsigned long)(x) + PAGE_OFFSET)
#define __pa(x) ((unsigned long)(x) - PAGE_OFFSET)

/* convert a physical address into a virtual address. The address must be directly mapped */
static inline void *phys_to_virt(phys_addr_t paddr)
{
	return __va(paddr);
}

static inline phys_addr_t virt_to_phys(u64 vaddr) {
	return __pa(vaddr);
}

#endif /* !__ASSEMBLER__ */

#endif // X86_64_ASM_PAGES_H