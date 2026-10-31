#ifndef ASM_X86_64_PGTABLE_H
#define ASM_X86_64_PGTABLE_H

#include <asm/page.h>
#include <asm/pgtable_types.h>
#include <emerald/compiler.h>

void early_init_nx(void);

#define set_pgd(pgdp, pgd) 	native_set_pgd(pgdp, pgd)
#define pgd_clear(pgd)		native_pgd_clear(pgd)

#define set_pud(pudp, pud) 	native_set_pud(pudp, pud)
#define pud_clear(pud)		native_pud_clear(pud)

#define set_pmd(pmdp, pmd) 	native_set_pmd(pmdp, pmd)
#define pmd_clear(pmd)		native_pmd_clear(pmd)

#define set_pte(ptep, pte) 	native_set_pte(ptep, pte)
#define pte_clear(pmd)		native_pte_clear(pte)

#define pgd_val(x) 	native_pgd_val(x)
#define __pgd(x) 	native_make_pgd(x)

#define pud_val(x) 	native_pud_val(x)
#define __pud(x) 	native_make_pud(x)

#define pmd_val(x) 	native_pmd_val(x)
#define __pmd(x) 	native_make_pmd(x)

#define pte_val(x) 	native_pte_val(x)
#define __pte(x) 	native_make_pte(x)

/*
 * Modifiers and Setters
 */

static inline void native_set_pte(pte_t *ptep, pte_t pte)
{
	WRITE_ONCE(*ptep, pte);
}

static inline void native_pte_clear(pte_t *ptep)
{
	native_set_pte(ptep, __pte(0));
}

static inline void native_set_pud(pud_t *pudp, pud_t pud)
{
	WRITE_ONCE(*pudp, pud);
}

static inline void native_pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));
}

static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	WRITE_ONCE(*pmdp, pmd);
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	set_pmd(pmdp, __pmd(0));
}

static inline void native_set_pgd(pgd_t *pgdp, pgd_t pgd)
{
	WRITE_ONCE(*pgdp, pgd);
}

static inline void native_pgd_clear(pgd_t *pgdp)
{
	set_pgd(pgdp, __pgd(0));
}

static inline pte_t pte_set_flags(pte_t pte, pteval_t set)
{
	pteval_t val = native_pte_val(pte);
	return native_make_pte(val | set);
}

static inline pte_t pte_clear_flags(pte_t pte, pteval_t clear)
{
	pteval_t val = native_pte_val(pte);
	return native_make_pte(val & ~clear);
}

static inline pmd_t pmd_set_flags(pmd_t pmd, pmdval_t set)
{
	pmdval_t val = native_pmd_val(pmd);
	return native_make_pmd(val | set);
}

static inline pmd_t pmd_clear_flags(pmd_t pmd, pmdval_t clear)
{
	pmdval_t val = native_pmd_val(pmd);
	return native_make_pmd(val & ~clear);
}

static inline pud_t pud_set_flags(pud_t pud, pudval_t set)
{
	pudval_t val = native_pud_val(pud);
	return native_make_pud(val | set);
}

static inline pud_t pud_clear_flags(pud_t pud, pudval_t clear)
{
	pudval_t val = native_pud_val(pud);
	return native_make_pud(val & ~clear);
}

/*
 * Check various bits of page tables
 * - assumes pte_present() is true
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

static inline bool pmd_exec(pmd_t pmd)
{
	return !(pmd_flags(pmd) & _PAGE_NX);
}

static inline bool pud_present(pud_t pud)
{
	return pud_flags(pud) & _PAGE_PRESENT;
}

static inline bool pud_writable(pud_t pud)
{
	return pud_flags(pud) & _PAGE_RW;
}

static inline bool pud_huge(pud_t pud)
{
	return pud_flags(pud) & _PAGE_PS;
}

static inline bool pud_exec(pud_t pud)
{
	return !(pud_flags(pud) & _PAGE_NX);
}

static inline bool pgd_present(pgd_t pgd)
{
	return pgd_flags(pgd) & _PAGE_PRESENT;
}

/*
 * Functions that return a Page Frame Number (PFN)
 */

#define PTE_PFN_SHIFT PAGE_SHIFT

static inline unsigned long pte_pfn(pte_t pte)
{
	pteval_t val = pte_val(pte);
	return (val & PTE_PFN_MASK) >> PTE_PFN_SHIFT;
}

static inline unsigned long pmd_pfn(pmd_t pmd)
{
	pmdval_t val = pmd_val(pmd);
	return (val & pmd_pfn_mask(pmd)) >> PTE_PFN_SHIFT;
}

static inline unsigned long pud_pfn(pud_t pud)
{
	pudval_t val = pud_val(pud);
	return (val & pud_pfn_mask(pud)) >> PTE_PFN_SHIFT;
}

static inline unsigned long pgd_pfn(pgd_t pgd)
{
	pgdval_t val = pgd_val(pgd);
	return (val & PTE_PFN_MASK) >> PTE_PFN_SHIFT;
}

/*
 * Functions for getting the index of a PGD/PUD/PMD/PTE given a virtual addr.
 */

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

/*
 * State Mutators
 */

static inline pte_t pte_wrprotect(pte_t pte)
{
	return pte_clear_flags(pte, _PAGE_RW);
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	return pte_set_flags(pte, _PAGE_RW);
}

static inline pte_t pte_mkexec(pte_t pte)
{
	return pte_clear_flags(pte, _PAGE_NX);
}

static inline pmd_t pmd_wrprotect(pmd_t pmd)
{
	return pmd_clear_flags(pmd, _PAGE_RW);
}

static inline pmd_t pmd_mkwrite(pmd_t pmd)
{
	return pmd_set_flags(pmd, _PAGE_RW);
}

static inline pmd_t pmd_mkexec(pmd_t pmd)
{
	return pmd_clear_flags(pmd, _PAGE_NX);
}

/*
 * Functions to build entries from a PFN + pgprot_t
 */

static inline pte_t pfn_pte(unsigned long pfn, pgprot_t prot)
{
	return __pte(((pteval_t)pfn << PTE_PFN_SHIFT) | pgprot_val(prot));
}

static inline pmd_t pfn_pmd(unsigned long pfn, pgprot_t prot)
{
	return __pmd(((pmdval_t)pfn << PTE_PFN_SHIFT) | pgprot_val(prot));
}

static inline pgprot_t pgprot_set(pgprot_t prot, pteval_t bits)
{
	return __pgprot(pgprot_val(prot) | bits);
}

static inline pgprot_t pgprot_clear(pgprot_t prot, pteval_t bits)
{
	return __pgprot(pgprot_val(prot) & ~bits);
}

/* split a 2M entry into 4K ones: drop PS, and move PAT from bit 12 to bit 7 */
static inline pgprot_t pgprot_split_large(pgprot_t prot)
{
	pteval_t v = pgprot_val(prot) & ~(_PAGE_PS | _PAGE_PAT_LARGE);

	if (pgprot_val(prot) & _PAGE_PAT_LARGE)
		v |= _PAGE_PAT;
	return __pgprot(v);
}

#endif	/* ASM_X86_64_PGTABLE_H */