#ifndef ASM_X86_64_PTRACE_H
#define ASM_X86_64_PTRACE_H

#include <emerald/types.h>

struct pt_regs {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 rbp;
    u64 rbx;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rax;
    u64 rcx;
    u64 rdx;
    u64 rsi;
    u64 rdi;
    u64 vector;
    u64 error_code;
    /* Instruction pointer */
    u64 rip;
    u64 cs;
    u64 eflags;
    u64 rsp;
    u64 ss;
};

#define user_mode(regs) ((regs)->cs & 3)

void __show_regs(struct pt_regs *regs);

#endif