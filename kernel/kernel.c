#include <emerald/runtime.h>
#include <emerald/kernel.h>
#include <emerald/fbcon.h>
#include <emerald/printk.h>

#if CFG_PANIC
#include <asm/bug.h>
#endif

#include <asm/e820/api.h>
#include <asm/setup.h>

#include "tests.h"

/* temporary function for logging progress of kernel */
static void log_progress()
{
        printk("================ Current Progress ==================\n");
        printk(" 1. Working Interrupt Handlers for CPU exceptions\n");
        printk(" 2. Validated RSDT/XSDT\n");
        printk(" 3. Use the Kernel's stack instead of UEFI's stack.\n");
}

int kernel_main()
{
        printk("EmeraldOS Kernel v%s\n\n", KERNEL_VERSION);
        //log_progress();
        printk("Hello from ELF Kernel!\n");
        printk("Hello from virtual address: %#p", &kernel_main);
        //run_dev_tests();

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
        init_fbcon(&boot->info);
        
        print_e820_table(&boot->mem);
        printk("\n");

        kernel_main();
        return 0;
}