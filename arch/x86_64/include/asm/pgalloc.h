#ifndef ASM_X86_64_PGALLOC_H
#define ASM_X86_64_PGALLOC_H

#include <asm/pgtable.h>
#include <emerald/types.h>

static inline void pmd_populate_phys(pmd_t *pmdp, phys_addr_t pte_table)
{
	set_pmd(pmdp, __pmd(pte_table | _PAGE_TABLE));
}

static inline void pud_populate_phys(pud_t *pudp, phys_addr_t pmd_table)
{
	set_pud(pudp, __pud(pmd_table | _PAGE_TABLE));
}

static inline void pgd_populate_phys(pgd_t *pgdp, phys_addr_t pud_table)
{
	set_pgd(pgdp, __pgd(pud_table | _PAGE_TABLE));
}

#endif	/* ASM_X86_64_PGALLOC_H */