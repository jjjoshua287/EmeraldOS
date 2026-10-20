#include <asm/e820/api.h>

#include <emerald/types.h>
#include <emerald/runtime.h>
#include <emerald/memblock.h>

extern unsigned char __kernel_start[];
extern unsigned char __kernel_end[];

void acpi_boot_init(void *sys_desc_ptr);

/* this will do more in the future, for now it's currently a wrapper for acpi_boot_init */

void setup_arch(struct boot_info *boot) 
{
	memblock_reserve_kern(0, 0x100000);
	memblock_reserve_kern((phys_addr_t)__kernel_start, (phys_addr_t)(__kernel_end - __kernel_start));
	e820_memblock_setup(&boot->mem);
	acpi_boot_init(boot->rsdp);
}