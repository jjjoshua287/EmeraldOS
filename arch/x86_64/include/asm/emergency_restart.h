#ifndef ASM_X86_64_EMERGENCY_RESTART_H
#define ASM_X86_64_EMERGENCY_RESTART_H

/* Intentionally cause a Triple Fault to force a reboot */
[[noreturn]] void machine_emergency_restart(void);

#endif /* ASM_X86_64_EMERGENCY_RESTART_H */