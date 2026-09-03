#include <emerald/kernel.h>
#include <emerald/printk.h>

#include <asm/bug.h>
#include <asm/ptrace.h>

void show_regs_print_info()
{
        printk("Kernel Version: v%s\n", KERNEL_VERSION);
        printk("REGISTER DUMP:\n");
}

void show_regs(struct pt_regs *regs)
{
        /* Guard against NULL regs, print nothing instead */
        BUG_ON(regs == NULL);
        show_regs_print_info();
        __show_regs(regs);
}