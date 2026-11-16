#include <asm/pgtable.h>

#include <emerald/types.h>
#include <emerald/string.h>
#include <emerald/panic.h>
#include <emerald/align.h>
#include <emerald/minmax.h>
#include <emerald/memblock.h>

// included for debugging purposes
#include <emerald/printk.h>

#include "mm_internal.h"

/* bits supported by the hardware */
pteval_t __supported_pte_mask = ~0;
/* bits allowed in normal kernel mappings */
pteval_t __default_kernel_pte_mask = ~0;

static pmd_t early_dynamic_pgts[EARLY_DYNAMIC_PGTS][PTRS_PER_PMD] __aligned(PAGE_SIZE);
static unsigned int next_early_pgt;

unsigned long min_pfn_mapped = 0;
unsigned long max_low_pfn_mapped = 0;
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

void update_mapped_pfn_ranges(unsigned long start_pfn, unsigned long end_pfn)
{
	if (end_pfn > max_pfn_mapped)
		max_pfn_mapped = end_pfn;
	if (start_pfn < (1UL << (32 - PAGE_SHIFT))) {
		unsigned long low_pfn = min(end_pfn, 1UL << (32 - PAGE_SHIFT));
		if (low_pfn > max_low_pfn_mapped)
			max_low_pfn_mapped = low_pfn;
	}
}

static unsigned long page_size_mask = 1UL << PG_LEVEL_2M;

/* setup the direct mapping of the physical memory at PAGE_OFFSET */
phys_addr_t init_memory_mapping(phys_addr_t start, phys_addr_t end, 
				pgprot_t prot)
{
	phys_addr_t ret = 0;
	printk("init_memory_mapping: [mem 0x%016lx-0x%016lx]\n", 
		start, end - 1);

	start = ALIGN_DOWN(start, HPAGE_SIZE);
	end = ALIGN(end, HPAGE_SIZE);

	while (start < end)
		ret = init_kernel_phys_mapping(start, end, page_size_mask, prot);
	
	unsigned long pfn = ret >> PAGE_SHIFT;
	update_mapped_pfn_ranges(start >> PAGE_SHIFT, pfn);
	return pfn;
}