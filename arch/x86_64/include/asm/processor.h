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

struct control_regs {
	unsigned long long cr0;
	unsigned long long cr2;
	unsigned long long cr3;
	unsigned long long cr4;
	unsigned long long cr8;
} __packed;

struct stack_frame {
	struct stack_frame *rbp;
	u64 ret_addr;
};

static __always_inline unsigned long long native_read_cr0(void)
{
    	unsigned long long val;
    	__asm__ volatile("mov %%cr0, %0" : "=r"(val));
    	return val;
}

static __always_inline unsigned long long native_read_cr2(void)
{
    	unsigned long long val;
    	__asm__ volatile("mov %%cr2, %0" : "=r"(val));
    	return val;
}

static __always_inline unsigned long long native_read_cr3(void)
{
    	unsigned long long val;
    	__asm__ volatile("mov %%cr3, %0" : "=r"(val));
    	return val;
}

static __always_inline unsigned long long native_read_cr4(void)
{
    	unsigned long long val;
    	__asm__ volatile("mov %%cr4, %0" : "=r"(val));
    	return val;
}

static __always_inline unsigned long long native_read_cr8(void)
{
    	unsigned long long val;
    	__asm__ volatile("mov %%cr8, %0" : "=r"(val));
    	return val;
}

#endif // X86_64_PROCESSOR_H