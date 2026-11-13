#ifndef X86_64_SETUP_H
#define X86_64_SETUP_H

#include <emerald/runtime.h>

/* setup_gdt is called by x86_64_start_kernel */
void setup_gdt(void);
void setup_idt(void);

void setup_arch(struct boot_info *boot);

#endif /* X86_64_SETUP_H */