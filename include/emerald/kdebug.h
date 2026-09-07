#ifndef EMERALD_KDEBUG_H
#define EMERALD_KDEBUG_H

#include <asm/ptrace.h>

void show_regs_print_info();
void show_regs(struct pt_regs *regs);
void dump_stack(struct pt_regs *regs);

#endif