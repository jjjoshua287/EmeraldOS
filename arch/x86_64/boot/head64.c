#include <emerald/types.h>
#include <emerald/align.h>
#include <emerald/runtime.h>
#include <emerald/fbcon.h>

#include <asm/pages.h>
#include <asm/setup.h>
#include <asm/e820/types.h>
#include <asm/e820/api.h>

extern u64 level2_kernel_pgt_va[PT_ENTRIES];

#define EARLY_FB_PMD_SLOT   510
#define EARLY_FB_VIRT_BASE  ((void *)(KERNEL_VMA + ((u64)EARLY_FB_PMD_SLOT * HPAGE_SIZE)))

extern u64 level2_kernel_pgt[PT_ENTRIES];

void *early_setup_framebuffer(struct screen_info *scr)
{
	phys_addr_t phys_base = ALIGN_DOWN(scr->lfb_base, HPAGE_SIZE);
	phys_addr_t offset = scr->lfb_base - phys_base;
	size_t hpages = ALIGN(scr->lfb_size + offset, HPAGE_SIZE) / HPAGE_SIZE;

	/* Populate slots 510 and 511 */
	for (size_t i = 0; i < hpages && (EARLY_FB_PMD_SLOT + i) < PT_ENTRIES; i++) {
		level2_kernel_pgt_va[EARLY_FB_PMD_SLOT + i] = (phys_base + (i * HPAGE_SIZE)) | PTE_MMIO;
	}

	/* Invalidate TLB */
	asm volatile("movq %%cr3, %%rax\n\t"
	             "movq %%rax, %%cr3\n\t"
	             ::: "rax", "memory");

	return (void *)((uintptr_t)EARLY_FB_VIRT_BASE + offset);
}

/* forward reference of kernel's main entry point */
int start_kernel(struct boot_info *boot);

/* architecture-specific entry point for kernel */
void x86_64_start_kernel(struct boot_info *boot)
{
	setup_gdt();
	setup_idt(); 

	/* Map the framebuffer */
	boot->info.lfb_base = (u64)early_setup_framebuffer(&boot->info);

	init_fbcon(&boot->info);
	sanitize_e820_table(&boot->mem);

	start_kernel(boot);
}
