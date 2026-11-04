#ifndef ASM_X86_64_PGTABLE_64_TYPES_H
#define ASM_X86_64_PGTABLE_64_TYPES_H

#ifndef __ASSEMBLER__
#include <emerald/types.h>

/* these are to allow use of C type-checking */
typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;

typedef struct { pteval_t pte; } pte_t;
typedef struct { pmdval_t pmd; } pmd_t;
typedef struct { pudval_t pud; } pud_t;
typedef struct { pgdval_t pgd; } pgd_t;

typedef struct { pteval_t pgprot; } pgprot_t;

#endif	/* !__ASSEMBLER__ */

/* Page Global Directory */
#define PGD_SHIFT 	39
#define PTRS_PER_PGD	512

/* Page Upper Directory */
#define PUD_SHIFT	30
#define PTRS_PER_PUD	512

/* Page Middle Directory */
#define PMD_SHIFT	21
#define PTRS_PER_PMD	512

/* entries per page directory level */
#define PTRS_PER_PTE	512

#define PMD_SIZE (1UL << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))

#define PUD_SIZE (1UL << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE - 1))

#define PGD_SIZE (1UL << PGD_SHIFT)
#define PGD_MASK (~(PGD_SIZE - 1))

/* Memory Map */
#define MAX_PHYSMEM_BITS 46
#define MAXMEM (1UL << MAX_PHYSMEM_BITS)

#define L3_START_KERNEL 510

#define KERNEL_LMA 	0x0000000000100000
#define KERNEL_VMA 	0xffffffff80000000

#endif	/* ASM_X86_64_PGTABLE_64_TYPES_H */