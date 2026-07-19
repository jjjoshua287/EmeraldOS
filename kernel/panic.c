#include <asm/desc.h>
#include <asm/processor.h>

#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/fbcon.h>

static bool panicking = false;

/* Restart kernel by forcing a triple fault */
[[noreturn]] void emergency_restart()
{
        __asm__ volatile("cli");
        invalidate_idt();
        __asm__ volatile("int $64");
}

/* prints an error message and registers if non-NULL. Halts PC */
[[noreturn]] void panic(const char *msg, struct hw_regs *regs)
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