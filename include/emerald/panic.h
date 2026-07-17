#ifndef EMERALD_OS_PANIC_H
#define EMERALD_OS_PANIC_H

#include <asm/processor.h>

[[noreturn]] void panic(const char *msg, struct hw_regs *regs);

#endif