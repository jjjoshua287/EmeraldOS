#include <asm/desc.h>
#include <asm/irq.h>

[[noreturn]] void machine_emergency_restart(void)
{
        native_irq_disable();
        invalidate_idt();
        __asm__ volatile("int $64");
}