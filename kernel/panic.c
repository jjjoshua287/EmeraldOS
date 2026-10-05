#include <asm/emergency_restart.h>
#include <asm/ptrace.h>
#include <asm/irq.h>

#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/fbcon.h>
#include <emerald/kdebug.h>
#include <emerald/compiler.h>

static bool panicking = false;

/* Restart kernel by forcing a triple fault */
[[noreturn]] void emergency_restart(void)
{
        machine_emergency_restart();
        /* Prevent compiler from throwing -Winvalid-noreturn */
        unreachable();
}

/* prints an error message and registers if non-NULL. Halts PC */
[[noreturn]] void panic(const char *msg, struct pt_regs *regs)
{
        /* Prevent recursive calls to panic() */
        if (unlikely(panicking))
                emergency_restart();
        panicking = true;

        fbcon_clear();
        printk("KERNEL PANIC!\n");
        printk("%s\n\n", msg);
        
        if (regs) {
                show_regs(regs);
                printk("\n");
        }
        dump_stack(regs);

        /* Halt CPU */
        native_irq_disable();
        while (1)
                native_halt();
}