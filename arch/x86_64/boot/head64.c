#include <emerald/runtime.h>
#include <asm/e820/api.h>
#include <asm/setup.h>

/* forward reference of kernel's main entry point */
int start_kernel(struct boot_info *boot);

/* architecture-specific entry point for kernel */
void x86_64_start_kernel(struct boot_info *boot)
{
        /* TODO: setup kernel's final page tables */

        setup_gdt();
        setup_idt();

        sanitize_e820_table(&boot->mem);

        start_kernel(boot);
}