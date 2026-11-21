#ifndef ASM_X86_64_CPUID_H
#define ASM_X86_64_CPUID_H

#include <emerald/types.h>

#define CPUID_LEAF_BASE 0x80000000
#define CPUID_LEAF_EXT1 0x80000001

#define CPUID_EDX_PGE		(1U << 13)
#define CPUID_EDX_NX 		(1U << 20)
#define CPUID_EDX_1GB_PAGE	(1U << 26)

static inline void native_cpuid(u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile("cpuid"
		: "=a" (*eax),
		  "=b" (*ebx),
		  "=c" (*ecx),
		  "=d" (*edx)
		: "0" (*eax), "2" (*ecx)
		: "memory");
}

/*
 * Generic CPUID function (from Linux)
 * 
 * NOTE: EmeraldOS currently doesn't support paravirt, so the 
 * expansion of `__cpuid` (native_cpuid) is used instead.
 *
 * Clear ECX since some CPUs (Cyrix MII) do not set or clear ECX,
 * resulting in stale register contents being returned.
 */
static void cpuid(u32 op, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	*eax = op;
	*ecx = 0;
	native_cpuid(eax, ebx, ecx, edx);
}

/* Some CPUID calls want 'count' to be placed in ECX */
static inline void cpuid_count(u32 op, int count, u32 *eax, u32 *ebx, 
			       u32 *ecx, u32 *edx)
{
	*eax = op;
	*ecx = count;
	native_cpuid(eax, ebx, ecx, edx);
}

/* CPUID functions returning a single register */

static inline u32 cpuid_eax(u32 op)
{
	u32 eax, ebx, ecx, edx;
	cpuid(op, &eax, &ebx, &ecx, &edx);
	return eax;
}

static inline u32 cpuid_ebx(u32 op)
{
	u32 eax, ebx, ecx, edx;
	cpuid(op, &eax, &ebx, &ecx, &edx);
	return ebx;
}

static inline u32 cpuid_ecx(u32 op)
{
	u32 eax, ebx, ecx, edx;
	cpuid(op, &eax, &ebx, &ecx, &edx);
	return ecx;
}

static inline u32 cpuid_edx(u32 op)
{
	u32 eax, ebx, ecx, edx;
	cpuid(op, &eax, &ebx, &ecx, &edx);
	return edx;
}

#endif	/* ASM_X86_64_CPUID_H */