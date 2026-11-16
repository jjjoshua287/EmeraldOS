#include <asm/pgtable.h>
#include <emerald/types.h>
#include <emerald/panic.h>
#include <emerald/string.h>
#include <emerald/memblock.h>

// included for debugging purposes
#include <emerald/printk.h>

/* bits supported by the hardware */
pteval_t __supported_pte_mask = ~0;
/* bits allowed in normal kernel mappings */
pteval_t __default_kernel_pte_mask = ~0;

static pmd_t early_dynamic_pgts[EARLY_DYNAMIC_PGTS][PTRS_PER_PMD] __aligned(PAGE_SIZE);
static unsigned int next_early_pgt;

unsigned long min_pfn_mapped = 0;
unsigned long max_pfn_mapped = (phys_addr_t)EARLY_MAP_END >> PAGE_SHIFT;

bool direct_map_ready = false;

// allocate a new page table before the direct map is built
void *alloc_pgt(void)
{
	if (next_early_pgt + 1 <= EARLY_DYNAMIC_PGTS)
		return early_dynamic_pgts[next_early_pgt++];

	// debug print
	printk("INFO: early_dynamic_pgts consumed; using memblock\n");
	
	phys_addr_t page = 0;
	if (min_pfn_mapped < max_pfn_mapped) {
		page = memblock_phys_alloc_range(PAGE_SIZE, PAGE_SIZE,
						min_pfn_mapped << PAGE_SHIFT,
						max_pfn_mapped << PAGE_SHIFT);
	}

	if (!page)
		panic("alloc_low_pages: Failed to allocate memory!", NULL);

	void *pgt = (void *)(page + KERNEL_VMA);
	memset(pgt, 0, PAGE_SIZE);
	return pgt;
}