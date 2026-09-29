#ifndef ASM_X86_64_REBOOT_H
#define ASM_X86_64_REBOOT_H

/* Intentionally cause a Triple Fault to force a reboot */
[[noreturn]] void machine_emergency_restart(void);

#endif /* ASM_X86_64_REBOOT_H */