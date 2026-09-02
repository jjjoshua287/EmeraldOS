#include <asm/desc.h>
#include <asm/ptrace.h>

#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/fbcon.h>
#include <emerald/kdebug.h>

static bool panicking = false;

/* Restart kernel by forcing a triple fault */
[[noreturn]] void emergency_restart(void)
{
        __asm__ volatile("cli");
        invalidate_idt();
        __asm__ volatile("int $64");

        /* Prevent compiler from throwing -Winvalid-noreturn */
        __builtin_unreachable();
}

/* prints an error message and registers if non-NULL. Halts PC */
[[noreturn]] void panic(const char *msg, struct pt_regs *regs)
{
        /* Prevent recursive calls to panic() */
        if (panicking)
                emergency_restart();
        panicking = true;

        fbcon_clear();
        printk("KERNEL PANIC!\n");
        printk("%s\n\n", msg);
        show_regs(regs);

        /* Halt CPU */
        __asm__ volatile ("cli");
        while (1)
                __asm__ volatile ("hlt");
}