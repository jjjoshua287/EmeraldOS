#include <emerald/runtime.h>
#include <emerald/kernel.h>
#include <emerald/printk.h>

#if CFG_PANIC
#include <asm/bug.h>
#endif

#include <asm/setup.h>

#include "tests.h"

int kernel_main(struct boot_info *boot)
{
	printk("EmeraldOS Kernel v%s\n\n", KERNEL_VERSION);
	printk("Hello from ELF Kernel!\n");
	printk("Hello from virtual address: %#p", &kernel_main);
	
	#if CFG_TEST
	run_dev_tests();
	#endif

	#if CFG_PANIC
	BUG();
	#endif

	while (1);
	return 0;
}

// entry point into kernel, ExitBootServices should be called before entering this
int start_kernel(struct boot_info *boot)
{
	setup_arch(boot);
	
	printk("\nFramebuffer region: [0x%016llx-0x%016llx]\n", 
		boot->info.lfb_base, boot->info.lfb_base + boot->info.lfb_size);
	printk("RSDP addr: %#p\n", boot->rsdp);
	
	kernel_main(boot);
	return 0;
}