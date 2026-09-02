#include <emerald/kernel.h>
#include <emerald/printk.h>

#include <asm/ptrace.h>

void show_regs_print_info()
{
        printk("Kernel Version: v%s\n", KERNEL_VERSION);
        printk("REGISTER DUMP:\n");
}

void show_regs(struct pt_regs *regs)
{
        show_regs_print_info();
        
        /* TODO: get the value of regs if regs == NULL */
        if (regs == NULL)
                return;
        __show_regs(regs);
}