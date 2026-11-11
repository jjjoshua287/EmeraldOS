#include <emerald/types.h>
#include <emerald/align.h>
#include <emerald/runtime.h>
#include <emerald/fbcon.h>
#include <emerald/panic.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>
#include <asm/setup.h>

/* NOTE: This may be moved to arch/x86_64/mm/fault.c */
void handle_early_page_fault(struct pt_regs *regs)
{
	panic("PAGE FAULT", regs);
}

#define EARLY_FB_VIRT_BASE ((void *)0xffffffffc0000000UL)

extern u64 level2_fb_pgt_va[PTRS_PER_PMD];

void early_setup_framebuffer(struct screen_info *scr)
{
	phys_addr_t phys_base = ALIGN_DOWN(scr->lfb_base, HPAGE_SIZE);
	phys_addr_t offset = scr->lfb_base - phys_base;
	size_t hpages = ALIGN(scr->lfb_size + offset, HPAGE_SIZE) / HPAGE_SIZE;
	
	for (size_t i = 0; i < hpages && i < PTRS_PER_PMD; i++)
		level2_fb_pgt_va[i] = (phys_base + i * HPAGE_SIZE) | __EARLY_PAGE_KERNEL_IO;
	
	/* Invalidate TLB */
	asm volatile("movq %%cr3, %%rax\n\t"
		     "movq %%rax, %%cr3\n\t"
		     ::: "rax", "memory");
	scr->lfb_base = (u64)((uintptr_t)EARLY_FB_VIRT_BASE + offset);
}

void fixup_boot_info(struct boot_info *boot)
{
	early_setup_framebuffer(&boot->info);
	boot->rsdp = phys_to_virt((phys_addr_t)boot->rsdp);
}

/* forward reference of kernel's main entry point */
int start_kernel(struct boot_info *boot);

/* architecture-specific entry point for kernel */
void x86_64_start_kernel(struct boot_info *boot)
{
	setup_gdt();
	early_idt_init();

	fixup_boot_info(boot);
	init_fbcon(&boot->info);

	start_kernel(boot);
}