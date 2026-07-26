#ifndef X86_64_PROCESSOR_H
#define X86_64_PROCESSOR_H

#include <emerald/types.h>
#include <emerald/compiler_types.h>

/* 
 * More things will be added here as the kernel grows, but until the kernel
 * dynamically detects the CPU, this file will be accurate to the dev's CPU:
 * 
 * AMD Ryzen 5 7600x with 6 Cores / 12 Threads
 */

/**
 * x86_hw_tss - Hardware Task State Segment layout (64-bit)
 * @sp0: Stack pointer for ring 0
 * @sp1: Stack pointer for ring 1
 * @sp2: Stack pointer for ring 2
 * @ist: Interrupt Stack Table - 7 stack pointers for tagged IDT gates
 * @iomap_base: Offset to I/O permission bitmap
 */
struct x86_hw_tss {
    u32 reserved1;
    u64 sp0;
    u64 sp1;
    u64 sp2;
    u64 reserved2;
    u64 ist[7];
    u32 reserved3;
    u32 reserved4;
    u16 reserved5;
    u16 iomap_base;
} __packed;

struct hw_regs {
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

#endif // X86_64_PROCESSOR_H