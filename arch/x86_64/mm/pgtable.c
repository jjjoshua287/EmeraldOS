#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/cpuid.h>
#include <asm/msr.h>
#include <asm/e820/api.h>

#include <emerald/types.h>
#include <emerald/panic.h>

#include "mm_internal.h"

bool init_nx(void)
{
	u32 nx_leaf = 0x80000001;
	u32 eax = cpuid_eax(0x80000000);
	
	if (eax >= nx_leaf) {
		u32 edx = cpuid_edx(nx_leaf);
		if (edx & CPUID_EDX_NX) {
			wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_NXE);
			return true;
		}
	}

	/* CPU doesn't support NX. bit 63 must never be set */
	__supported_pte_mask &= ~_PAGE_NX;
	return false;
}

extern bool direct_map_ready;

static inline void *table_va(phys_addr_t paddr) {
	if (direct_map_ready)
		return phys_to_virt(paddr);
	return (void *)(paddr + KERNEL_VMA);
}

static inline phys_addr_t table_pa(void *ptr) {
	if (direct_map_ready)
		return virt_to_phys(ptr);
	return __pa_symbol(ptr);
}

static inline pgprot_t pgprot_sethuge(pgprot_t prot)
{
	return __pgprot(pgprot_val(prot) | _PAGE_PS);
}

phys_addr_t map_pte_range(pte_t *pte_page, phys_addr_t paddr, phys_addr_t paddr_end,
		    	  pgprot_t prot)
{
	phys_addr_t paddr_last = paddr_end, paddr_next;

	int i = pte_index(paddr);
	pte_t *pte = pte_page + i;

	for (; i < PTRS_PER_PTE && paddr < paddr_end; i++, pte++, paddr = paddr_next) {
		paddr_next = (paddr & PAGE_MASK) + PAGE_SIZE;

		set_pte(pte, pfn_pte(paddr >> PAGE_SHIFT, prot));
		paddr_last = (paddr & PAGE_MASK) + PAGE_SIZE;
	}
	return paddr_last;
}

phys_addr_t map_pmd_range(pmd_t *pmd_page, phys_addr_t paddr, phys_addr_t paddr_end,
			  unsigned long page_size_mask, pgprot_t prot)
{
	phys_addr_t paddr_last = paddr_end, paddr_next;
	
	int i = pmd_index(paddr);

	for (; i < PTRS_PER_PMD && paddr < paddr_end; i++, paddr = paddr_next) {
		pmd_t *pmd = pmd_page + pmd_index(paddr);
		pte_t *pte;

		paddr_next = (paddr & PMD_MASK) + PMD_SIZE;
		if (pmd_present(*pmd)) {
			if (!pmd_leaf(*pmd)) {
				phys_addr_t page = (pmd_val(*pmd) & pmd_pfn_mask(*pmd));
				pte = (pte_t *)table_va(page);
				paddr_last = map_pte_range(pte, paddr, paddr_end,
							   prot);
				continue;
			}

			if (page_size_mask & (1UL << PG_LEVEL_2M)) {
				paddr_last = paddr_next;
				continue;
			} else {
				/* 
				 * panic for debugging, should be split 
				 * into multiple 4 KiB entries in the future
				 */
				panic("Requested page size doesn't match pmd\n", NULL);
			}
		}

		if (page_size_mask & (1UL << PG_LEVEL_2M)) {
			set_pmd(pmd, pfn_pmd(paddr >> PAGE_SHIFT, pgprot_sethuge(prot)));
			paddr_last = paddr_next;
			continue;
		}

		pte = alloc_pgt();
		paddr_last = map_pte_range(pte, paddr, paddr_end, prot);
		pmd_populate_phys(pmd, table_pa(pte));
	}
	return paddr_last;
}

phys_addr_t map_pud_range(pud_t *pud_page, phys_addr_t paddr, phys_addr_t paddr_end,
			  unsigned long page_size_mask, pgprot_t prot)
{
	phys_addr_t paddr_last = paddr_end, paddr_next;
	int i = pud_index(paddr);

	for (; i < PTRS_PER_PUD && paddr < paddr_end; i++, paddr = paddr_next) {
		pud_t *pud = pud_page + pud_index(paddr);
		pmd_t *pmd;

		paddr_next = (paddr & PUD_MASK) + PUD_SIZE;
		if (pud_present(*pud)) {
			if (!pud_leaf(*pud)) {
				phys_addr_t page = pud_val(*pud) & pud_pfn_mask(*pud);
				pmd = (pmd_t *)table_va(page);
				paddr_last = map_pmd_range(pmd, paddr, paddr_end,
							   page_size_mask, prot);
				continue;
			}

			if (page_size_mask & (1UL << PG_LEVEL_1G)) {
				paddr_last = paddr_next;
				continue;
			} else {
				/* 
				 * panic for debugging, should be split 
				 * into multiple 2 MiB entries in the future
				 */
				panic("Requested page size doesn't match pud\n", NULL);
			}
		}

		if (page_size_mask & (1UL << PG_LEVEL_1G)) {
			set_pud(pud, pfn_pud(paddr >> PAGE_SHIFT, pgprot_sethuge(prot)));
			paddr_last = paddr_next;
			continue;
		}
		pmd = alloc_pgt();
		paddr_last = map_pmd_range(pmd, paddr, paddr_end, page_size_mask, prot);
		pud_populate_phys(pud, table_pa(pmd));
	}
	return paddr_last;
}

phys_addr_t init_kernel_phys_mapping(phys_addr_t paddr_start, phys_addr_t paddr_end, 
				     unsigned long page_size_mask, pgprot_t prot)
{
	unsigned long vaddr, vaddr_end, vaddr_next;

	phys_addr_t paddr_last = paddr_end;
	vaddr = (unsigned long)__va(paddr_start);
	vaddr_end = (unsigned long)__va(paddr_end);

	for (; vaddr < vaddr_end; vaddr = vaddr_next) {
		pgd_t *pgd = kernel_pgd() + pgd_index(vaddr);
		pud_t *pud;

		vaddr_next = (vaddr & PGD_MASK) + PGD_SIZE;
		if (pgd_val(*pgd)) {
			phys_addr_t page = (pgd_val(*pgd) & PTE_PFN_MASK);
			pud = table_va(page);
			paddr_last = map_pud_range(pud, __pa(vaddr), paddr_end,
						   page_size_mask, prot);
			continue;
		}

		pud = alloc_pgt();
		paddr_last = map_pud_range(pud, __pa(vaddr), paddr_end, 
					   page_size_mask, prot);
		pgd_populate_phys(pgd, table_pa(pud));
	}

	return paddr_last;
}