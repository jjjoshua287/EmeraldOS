#include <emerald/runtime.h>
#include <emerald/kernel.h>
#include <emerald/printk.h>

#if CFG_PANIC
#include <asm/bug.h>
#endif

#include <asm/setup.h>

#include "tests.h"

int kernel_main()
{
	printk("\nEmeraldOS Kernel v%s\n\n", KERNEL_VERSION);
	
	#if CFG_TEST
	run_dev_tests();
	#endif

	while (1);
	return 0;
}

// entry point into kernel, ExitBootServices should be called before entering this
int start_kernel(struct boot_info *boot)
{
	setup_arch(boot);
	kernel_main();
	return 0;
}