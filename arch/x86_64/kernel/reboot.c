#include <asm/desc.h>
#include <asm/irq.h>

#include <emerald/compiler.h>

[[noreturn]] void machine_emergency_restart(void)
{
        native_irq_disable();
        invalidate_idt();
        __asm__ volatile("int $64");
	unreachable();
}
