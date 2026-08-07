#ifndef X86_64_SETUP_H
#define X86_64_SETUP_H

#include <emerald/runtime.h>

/* Assembly stub to reload segment selectors. */
extern void reload_segments(void);

void setup_gdt(void);
void setup_idt(void);
void acpi_boot_init(struct boot_info *boot);

#endif /* X86_64_SETUP_H */