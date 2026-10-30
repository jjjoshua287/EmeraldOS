#ifndef ASM_X86_64_PGTABLE_H
#define ASM_X86_64_PGTABLE_H

#include <asm/page.h>
#include <asm/pgtable_types.h>

void early_init_nx(void);

/*
 * Check various bits of page tables
 */

static inline bool pte_present(pte_t pte)
{
	return pte_flags(pte) & _PAGE_PRESENT;
}

static inline bool pte_writable(pte_t pte)
{
	return pte_flags(pte) & _PAGE_RW;
}

static inline bool pte_user(pte_t pte)
{
	return pte_flags(pte) & _PAGE_USER;
}

static inline bool pte_young(pte_t pte)
{
	return pte_flags(pte) & _PAGE_ACCESSED;
}

static inline bool pte_dirty(pte_t pte)
{
	return pte_flags(pte) & _PAGE_DIRTY;
}

static inline bool pte_huge(pte_t pte)
{
	return pte_flags(pte) & _PAGE_PS;
}

static inline bool pte_global(pte_t pte)
{
	return pte_flags(pte) & _PAGE_GLOBAL;
}

static inline bool pte_exec(pte_t pte)
{
	return !(pte_flags(pte) & _PAGE_NX);
}

static inline bool pmd_present(pmd_t pmd)
{
	return pmd_flags(pmd) & _PAGE_PRESENT;
}

static inline bool pmd_writable(pmd_t pmd)
{
	return pmd_flags(pmd) & _PAGE_RW;
}

static inline bool pmd_young(pmd_t pmd)
{
	return pmd_flags(pmd) & _PAGE_ACCESSED;
}

static inline bool pmd_dirty(pmd_t pmd)
{
	return pmd_flags(pmd) & _PAGE_DIRTY;
}

static inline bool pmd_huge(pmd_t pmd)
{
	return pmd_flags(pmd) & _PAGE_PS;
}

static inline bool pud_present(pud_t pud)
{
	return pud_flags(pud) & _PAGE_PRESENT;
}

static inline bool pud_young(pud_t pud)
{
	return pud_flags(pud) & _PAGE_ACCESSED;
}

static inline bool pud_dirty(pud_t pud)
{
	return pud_flags(pud) & _PAGE_DIRTY;
}

static inline bool pgd_present(pgd_t pgd)
{
	return pgd_flags(pgd) & _PAGE_PRESENT;
}

static inline bool pgd_young(pgd_t pgd)
{
	return pgd_flags(pgd) & _PAGE_ACCESSED;
}

static inline bool pgd_dirty(pgd_t pgd)
{
	return pgd_flags(pgd) & _PAGE_DIRTY;
}

#define PTE_PFN_SHIFT PAGE_SHIFT

static inline pfn_t pte_pfn(pte_t pte)
{
	pteval_t pte_val = native_pte_val(pte);
	return (pte_val & PTE_PFN_MASK) >> PTE_PFN_SHIFT;
}

static inline pfn_t pmd_pfn(pmd_t pmd)
{
	pmdval_t pmd_val = native_pmd_val(pmd);
	return (pmd_val & pmd_pfn_mask(pmd)) >> PTE_PFN_SHIFT;
}

static inline pfn_t pud_pfn(pud_t pud)
{
	pudval_t pud_val = native_pud_val(pud);
	return (pud_val & pud_pfn_mask(pud)) >> PTE_PFN_SHIFT;
}

static inline pfn_t pgd_pfn(pgd_t pgd)
{
	pgdval_t pgd_val = native_pgd_val(pgd);
	return (pgd_val & PTE_PFN_MASK) >> PTE_PFN_SHIFT;
}

static inline int pgd_index(unsigned long addr)
{
	return (addr >> PGD_SHIFT) & 0x1ff;
}

static inline int pud_index(unsigned long addr)
{
	return (addr >> PUD_SHIFT) & 0x1ff;
}

static inline int pmd_index(unsigned long addr)
{
	return (addr >> PMD_SHIFT) & 0x1ff;
}

static inline int pte_index(unsigned long addr)
{
	return (addr >> PAGE_SHIFT) & 0x1ff;
}

static inline pteval_t pte_make(pfn_t pfn, pteval_t flags)
{
	return (pfn << PTE_PFN_SHIFT) | flags;
}

#endif	/* ASM_X86_64_PGTABLE_H */