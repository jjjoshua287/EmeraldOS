/* Interrupt Service Routines called by assembly stubs */

#include <asm/ptrace.h>
#include <emerald/panic.h>

static void handle_user_exception()
{
        /* do something */
}

void handle_interrupt(struct pt_regs *regs)
{
        if (user_mode(regs))
                handle_user_exception();
        else
                panic("Fault occured in Kernel!", regs);
}