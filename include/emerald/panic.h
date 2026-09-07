#ifndef EMERALD_OS_PANIC_H
#define EMERALD_OS_PANIC_H

#include <asm/ptrace.h>

[[noreturn]] void panic(const char *msg, struct pt_regs *regs);

#endif