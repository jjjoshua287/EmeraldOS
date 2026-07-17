/* Interrupt Service Routines called by assembly stubs */

#include <asm/processor.h>

/* detects if an interrupt occurred in the Kernel or a user process
 * - this is currently unneeded at the moment because there is no concept of userspace processes,
 *   but it'll be useful once the concept of userspace exists
 */
#define FAULT_IN_KERNEL_SPACE(regs) !(regs->cs & 3)

static void handle_user_exception(struct hw_regs *regs)
{
        /* determine thing to do */
}

void handle_interrupt(struct hw_regs *regs)
{
        if (FAULT_IN_KERNEL_SPACE(regs))
                ; /* kernel_panic("CPU Interrupt", regs); */
        else
                handle_user_exception(regs);
}