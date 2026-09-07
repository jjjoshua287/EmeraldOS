/* Interrupt Service Routines called by assembly stubs */

#include <asm/ptrace.h>
#include <emerald/panic.h>

static void handle_irq()
{
        /* do something */
}

static void handle_user_exception()
{
        /* do something */
}

void handle_interrupt(struct pt_regs *regs)
{
        if (regs->vector >= 32) {
                handle_irq();
                return;
        }
        if (user_mode(regs))
                handle_user_exception();
        else
                panic("Exception occured in Kernel!", regs);
}