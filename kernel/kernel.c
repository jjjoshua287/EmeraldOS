#include <emerald/runtime.h>
#include <emerald/kernel.h>
#include <emerald/fbcon.h>
#include <emerald/printk.h>

#include <asm/setup.h>

/* temporary function for logging progress of kernel */
static void log_progress()
{
        printk("============== Current Progress ===============\n");
        printk(" Working Interrupt Handlers for CPU exceptions\n");
}

int kernel_main()
{
        printk("EmeraldOS Kernel v%s\n\n", KERNEL_VERSION);
        log_progress();

        while (1);
        return 0;
}

// entry point into kernel, ExitBootServices should be called before entering this
int start_kernel(void)
{
        setup_gdt();
        setup_idt();
        init_fbcon(&boot.info);
        kernel_main();
        return 0;
}