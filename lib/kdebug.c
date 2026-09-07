#include <emerald/kernel.h>
#include <emerald/printk.h>

#include <asm/bug.h>
#include <asm/ptrace.h>

void show_regs_print_info()
{
        printk("Kernel Version: v%s\n", KERNEL_VERSION);
        printk("REGISTER DUMP:\n");
}

/**
 * show_regs: Print the values of every CPU register onto the screen
 * @regs: An interrupt frame with all the CPU registers
 * 
 * If regs == NULL, nothing is printed to the screen
 */
void show_regs(struct pt_regs *regs)
{
        if (unlikely(regs == NULL))
                return;
        show_regs_print_info();
        __show_regs(regs);
}

/**
 * dump_stack: print a Stack Trace onto the screen
 * @regs: An interrupt frame with all the CPU registers
 * 
 * regs is allowed to equal NULL here, handling of NULL values
 * is offloaded to architecture-specific functions.
 */
void dump_stack(struct pt_regs *regs)
{
	printk("Call Trace:\n");
	__dump_stack(regs);
}