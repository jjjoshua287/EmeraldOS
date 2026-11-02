#include <asm/e820/api.h>
#include <asm/pages.h>

#include <emerald/types.h>
#include <emerald/runtime.h>
#include <emerald/memblock.h>

#include <emerald/printk.h>

extern unsigned char __kernel_start[];
extern unsigned char __kernel_end[];

extern u64 init_top_pgt[PT_ENTRIES];
extern u64 early_top_pgt[PT_ENTRIES];

void acpi_boot_init(void *sys_desc_ptr);

static void do_early_reserves(struct boot_info *boot)
{
	/* reserve the Real Mode / BIOS Area */
	memblock_reserve(0, 1024 * 1024);

	/* reserve the kernel image and the boot_info struct */
	memblock_reserve_kern((phys_addr_t)__kernel_start, (phys_addr_t)(__kernel_end - __kernel_start));
	memblock_reserve_kern(virt_to_phys(boot), sizeof(struct boot_info));
	
	/* reserve the boot page tables */
	memblock_reserve_kern((phys_addr_t)init_top_pgt, PT_SIZE);
	memblock_reserve_kern((phys_addr_t)early_top_pgt, PT_SIZE);
}

static inline void do_alloc_test()
{
	void *alloc = memblock_alloc(0xf, PAGE_SIZE);
	printk("Allocated %d bytes at %p\n\n", 0xf, alloc);
}

void setup_arch(struct boot_info *boot) 
{
	do_early_reserves(boot);
	e820_memblock_setup(&boot->mem);
	memblock_set_curr_limit(EARLY_MAP_END);

	/* BUG: This function is commented out to avoid a #PF. it will resolved once I get to doing that. */
	//acpi_boot_init(boot->rsdp);
}