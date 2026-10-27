#include <asm/pgtable.h>
#include <asm/cpuid.h>
#include <asm/msr.h>

/* used in PAGE_* macros */
pteval_t __supported_pte_mask = ~0ULL;

void early_init_nx(void)
{
	u32 nx_leaf = 0x80000001;
	u32 eax = cpuid_eax(0x80000000);
	
	if (eax >= nx_leaf) {
		u32 edx = cpuid_edx(nx_leaf);
		if (edx & CPUID_EDX_NX) {
			wrmsr(MSR_EFER, rdmsr(MSR_EFER | EFER_NXE));
			return;
		}
	}

	/* CPU doesn't support NX. bit 63 must never be set */
	__supported_pte_mask &= ~_PAGE_NX;
}