#ifndef ASM_X86_64_MSR_H
#define ASM_X86_64_MSR_H

#include <emerald/types.h>

#define MSR_EFER 0xc0000080
#define EFER_NXE (1ULL << 11)

static inline u64 rdmsr(u32 msr)
{
	u32 lo, hi;
	asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
	return ((u64)hi << 32) | lo;
}

static inline void wrmsr(u32 msr, u64 val)
{
	asm volatile("wrmsr"
		:: "c"(msr), 
		   "a"((u32)val), 
		   "d"((u32)(val >> 32)) 
		 : "memory");
}

#endif	/* ASM_X86_64_MSR_H */