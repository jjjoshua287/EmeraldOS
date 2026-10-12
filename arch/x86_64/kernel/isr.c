/* Interrupt Service Routines called by assembly stubs */

#include <asm/ptrace.h>
#include <asm/exceptions.h>
#include <emerald/vsprintf.h>
#include <emerald/printk.h>
#include <emerald/panic.h>

static void handle_irq()
{
        /* do something */
}

static void handle_user_exception()
{
        /* do something */
}

void handle_invalid_opcode(struct pt_regs *regs)
{
        if (user_mode(regs))
                handle_user_exception();

        char msg[64];
        unsigned char *instn = (unsigned char *)regs->rip;
        
        /* BUG() uses "ud2" as the invalid opcode */
        if (instn[0] == 0x0F && instn[1] == 0x0B)
                snprintf(msg, sizeof(msg), "BUG: Kernel bug at %016llx\n", regs->rip);
        else
                snprintf(msg, sizeof(msg), "BUG: Invalid opcode at %016llx\n", regs->rip);
        
        panic(msg, regs);
}

void handle_interrupt(struct pt_regs *regs)
{
        if (regs->vector >= 32) {
                handle_irq();
                return;
        }
        
        if (user_mode(regs)) {
                handle_user_exception();
        }
        else {
                panic("Exception occured in Kernel!", regs);
        }
}