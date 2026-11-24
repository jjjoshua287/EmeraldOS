#include <asm/setup.h>
#include <asm/e820/api.h>
#include <asm/pgtable.h>
#include <asm/page.h>

#include <emerald/types.h>
#include <emerald/runtime.h>
#include <emerald/memblock.h>

#include <emerald/printk.h>

extern unsigned char __kernel_start[];
extern unsigned char __kernel_end[];

void acpi_boot_init(void *sys_desc_ptr);

/* this will do more in the future, for now it's currently a wrapper for acpi_boot_init */

static void do_early_reserves(struct boot_info *boot)
{
	/* reserve the Real Mode / BIOS Area */
	memblock_reserve(0, 1024 * 1024);
	memblock_reserve_kern((phys_addr_t)__kernel_start, (phys_addr_t)(__kernel_end - __kernel_start));
}

static inline void report_nx()
{
	if (init_nx())
		printk("NX: OK\n");
	else
		printk("NX Support Missing!\n");
}

void setup_arch(struct boot_info *boot) 
{
	setup_idt();
	report_nx();

	do_early_reserves(boot);
	e820_memory_setup(&boot->mem);

	max_pfn = e820_max_pfn();
	max_possible_pfn = max_pfn;
	if (max_pfn > (1UL << (32 - PAGE_SHIFT)))
		max_low_pfn = e820_max_low_pfn();
	else
		max_low_pfn = max_pfn;

	e820_memblock_setup();	
	init_mem_mapping();
	memblock_set_curr_limit(EARLY_MAP_END);

	/* ACPI isn't in the direct map yet, commented out to avoid a #PF */
	//acpi_boot_init(boot->rsdp);
}