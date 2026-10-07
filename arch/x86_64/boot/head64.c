#include <emerald/types.h>
#include <emerald/runtime.h>
#include <emerald/fbcon.h>

#include <asm/pages.h>
#include <asm/setup.h>
#include <asm/e820/types.h>
#include <asm/e820/api.h>

void fixup_boot_info(struct boot_info *boot, struct boot_info *out_virt)
{
        out_virt->rsdp = boot->rsdp;
        out_virt->info.lfb_base = (u64)phys_to_virt(boot->info.lfb_base);
        out_virt->mem = boot->mem;
}

/* forward reference of kernel's main entry point */
int start_kernel(struct boot_info *boot);

/* architecture-specific entry point for kernel */
void x86_64_start_kernel(struct boot_info *boot)
{
        /* TODO: setup kernel's final page tables */
        
        setup_gdt();
        setup_idt();

        /* 
         * initialize fbcon here because it doesn't require malloc (easier debugging in QEMU).
         * We can only do this in QEMU since the framebuffer sits in the right spot,
         * this won't work on real hardware.
         */
        init_fbcon(&boot->info);

        struct boot_info boot_virt;
        fixup_boot_info(boot, &boot_virt);

        sanitize_e820_table(&boot_virt.mem);

        start_kernel(&boot_virt);
}