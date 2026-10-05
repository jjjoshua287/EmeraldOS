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

#endif // X86_64_ASM_PAGES_H