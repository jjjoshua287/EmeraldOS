#include <asm/idt.h>
#include <asm/processor.h>

#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/fbcon.h>

static bool panicking = false;

/* Restart kernel by loading invalid IDT and triggering a Divide By Zero fault */
[[noreturn]] emergency_restart()
{
        /* Intentionally cause a Triple Fault to restart CPU */
        invalidate_idt();
        int x = 1 / 0;
}

void halt_cpu()
{

}

/* prints an error message and registers if non-NULL. Halts PC */
[[noreturn]] panic(const char *msg, struct hw_regs *regs)
{
        /* Prevent recursive calls to panic() */
        if (panicking)
                emergency_restart();
        panicking = true;

        fbcon_clear();
        printk("KERNEL PANIC!\n");
        printk("%s\n\n", msg);
        /* TODO: Register Dump */

        /* Halt CPU */
        __asm__ volatile ("cli");
        while (1)
                __asm__ volatile ("hlt");
}