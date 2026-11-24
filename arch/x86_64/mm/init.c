#include <asm/pgtable.h>
#include <asm/e820/api.h>
#include <asm/processor.h>
#include <asm/cpuid.h>
#include <asm/tlb.h>
#include <asm/bug.h>

#include <emerald/types.h>
#include <emerald/string.h>
#include <emerald/panic.h>
#include <emerald/align.h>
#include <emerald/minmax.h>
#include <emerald/memblock.h>
#include <emerald/pfn.h>
#include <emerald/mm.h>

// included for debugging purposes
#include <emerald/printk.h>

#include "mm_internal.h"

/* bits supported by the hardware */
pteval_t __supported_pte_mask = (~0 & ~_PAGE_GLOBAL);
/* bits allowed in normal kernel mappings */
pteval_t __default_kernel_pte_mask;

static pmd_t early_dynamic_pgts[EARLY_DYNAMIC_PGTS][PTRS_PER_PMD] __aligned(PAGE_SIZE);
static unsigned int next_early_pgt;

unsigned long min_pfn_mapped = 0;
unsigned long max_low_pfn_mapped = 0;
unsigned long max_pfn_mapped = 0;

/* 1 GiB Pages supported */
bool huge_pages = false;
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

struct map_range {
	phys_addr_t start;
	phys_addr_t end;
	unsigned long page_size_mask;
};

/* split a range into a page and tell the caller if it needs split again */
static bool split_range(struct map_range *map)
{
	unsigned long page_size = PAGE_SIZE;
	unsigned long size = map->end - map->start;

	if (huge_pages && size >= PUD_SIZE && IS_ALIGNED(map->start, PUD_SIZE)) {
		page_size = PUD_SIZE;
		map->page_size_mask = (1UL << PG_LEVEL_1G);
	} else if (size >= PMD_SIZE && IS_ALIGNED(map->start, PMD_SIZE)) {
		page_size = PMD_SIZE;
		map->page_size_mask = (1UL << PG_LEVEL_2M);
	} else {
		page_size = size;
		map->page_size_mask = (1UL << PG_LEVEL_4K);
	}

	if (IS_ALIGNED(size, page_size))
		return false;
	
	map->end = ALIGN_DOWN(map->end, page_size);
	return true;
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
	struct map_range map = { start, end, page_size_mask };

	printk("init_memory_mapping: [mem 0x%016lx-0x%016lx]\n", 
		start, end - 1);

	bool split = split_range(&map);
	do {
		map.start = init_kernel_phys_mapping(map.start, map.end,
					       	     map.page_size_mask, 
						     prot);
		if (split) {
			map.end = end;
			split = split_range(&map);
		}
	} while (split);

	unsigned long pfn = map.start >> PAGE_SHIFT;
	update_mapped_pfn_ranges(start >> PAGE_SHIFT, pfn);
	return pfn;
}

static void init_mem_map(void)
{
	struct memblock_region *rgn;
	for_each_mem_region(rgn) {
		phys_addr_t start = PAGE_ALIGN(rgn->base);
		phys_addr_t rend = rgn->base + rgn->size;
		phys_addr_t end = PAGE_ALIGN_DOWN(rend);
		
		if (end > start)
			init_memory_mapping(start, end, PAGE_KERNEL);
	}
}

static void probe_page_size_mask(void)
{
	/* Enable Global pages if available */
	u32 edx = cpuid_edx(0x1);
	if (edx & CPUID_EDX_PGE) {
		native_cr4_set_bits(X86_CR4_PGE);
		__supported_pte_mask |= _PAGE_GLOBAL;
	}

	/* everything is supported by default */
	__default_kernel_pte_mask = __supported_pte_mask;
	
	/* enable 1 GiB pages if available */
	u32 eax = cpuid_eax(CPUID_LEAF_BASE);
	if (eax >= CPUID_LEAF_EXT1) {
		edx = cpuid_edx(CPUID_LEAF_EXT1);
		if (edx & CPUID_EDX_1GB_PAGE) {
			printk("INFO: Using 1 GB pages for direct mapping\n");
			page_size_mask |= (1UL << PG_LEVEL_1G);
			huge_pages = true;
		}
	}
}

void init_mem_mapping()
{
	unsigned long end = max_pfn << PAGE_SHIFT;
	probe_page_size_mask();

	/* the ISA range is always mapped */
	init_memory_mapping(0, ISA_END_ADDRESS, PAGE_KERNEL);
	init_mem_map();

	if (max_pfn > max_low_pfn)
		max_low_pfn = max_pfn;
	native_write_cr3((unsigned long)swapper_pgd_dir);
	flush_tlb();
	
	direct_map_ready = true;
	printk("early_dynamic_pgt usage: %d/%d consumed, %d remaining\n",
		next_early_pgt, EARLY_DYNAMIC_PGTS, 
		EARLY_DYNAMIC_PGTS - next_early_pgt);
}