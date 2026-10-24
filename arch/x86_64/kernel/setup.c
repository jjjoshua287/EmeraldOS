#include <asm/e820/api.h>
#include <asm/pages.h>

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

static inline void do_alloc_test()
{
	void *alloc = memblock_alloc(0xf, PAGE_SIZE);
	printk("Allocated %d bytes at %p\n\n", 0xf, alloc);
}

void setup_arch(struct boot_info *boot) 
{
	do_early_reserves(boot);
	e820_memblock_setup(&boot->mem);
	do_alloc_test();

	/* BUG: This function is commented out to avoid a #PF. it will resolved once I get to doing that. */
	//acpi_boot_init(boot->rsdp);
}