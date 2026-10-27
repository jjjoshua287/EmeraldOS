#ifndef ASM_X86_64_PGTABLE_64_TYPES_H
#define ASM_X86_64_PGTABLE_64_TYPES_H

#ifndef __ASSEMBLY__
/* these are to allow use of C type-checking */
typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;

typedef struct { pteval_t pte; } pte_t;
#endif	/* !__ASSEMBLY__ */

/* entries per page directory level */
#define PT_SIZE		4096
#define PT_ENTRIES	512

/* Page Global Directory */
#define PGD_SHIFT 	38
#define PTRS_PER_PGD	PT_ENTRIES

/* Page Upper Directory */
#define PUD_SHIFT	30
#define PTRS_PER_PUD	PT_ENTRIES

/* Page Middle Directory */
#define PMD_SHIFT	21
#define PTRS_PER_PMD	PT_ENTRIES

/* Sizes and masks */

#define PMD_SIZE (1UL << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))

#define PUD_SIZE (1UL << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE - 1))

#define PGD_SIZE (1UL << PGD_SIZE)
#define PGD_MASK (~(PGD_SIZE - 1))

/* Memory Map */
#define L3_START_KERNEL 510

#define KERNEL_LMA 	0x0000000000100000
#define KERNEL_VMA 	0xffffffff80000000

#endif	/* ASM_X86_64_PGTABLE_64_TYPES_H */