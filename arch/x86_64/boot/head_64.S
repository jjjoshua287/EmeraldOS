.section .head.data, "aw", @progbits
.align 4096

/* early page tables */
.global early_pml4
early_pml4:
    .fill 512, 8, 0

.global early_pdpt
early_pdpt:
    .fill 512, 8, 0

.global early_pd
early_pd:
    /* Map the first 2 MiB of physical memory.
     * Flags:
     *  Bit 0: Present
     *  Bit 1: Writable
     *  But 7: Page Size
     * set the Page Size (PS) bit, which is bit 7 to use 2 MiB pages for early page tables
     */
     .quad 0x83
     .fill 511, 8, 0

.section .text
#include <asm/segment.h>
#include <emerald/init.h>

__HEAD
.code64

.set KERNEL_DS, __KERNEL_DS

/* 
 * This will need to be swapped to an architecture-specific version
 * if I ever add support for a different architecture, but that would
 * only occur after the kernel is complete for the x86_64 architecture.
 */
.extern start_kernel
.extern kernel_stack_top

.global start_64
.type start_64, @function
start_64:
    cli
    cld     /* SysV requires the Direction flag to be clear on function entry */ 

    /* setup kernel data segments */
    mov $KERNEL_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    /* fs and gs are left as NULL selectors for per-CPU data in the future */
    xor %ax, %ax
    mov %ax, %fs
    mov %ax, %gs

    /* setup initial page tables */
    leaq early_pdpt(%rip), %rax
    orq $0x3, %rax
    leaq early_pml4(%rip), %rbx     /* PML4[0] */
    movq %rax, 0(%rbx)
    movq %rax, (511 * 8)(%rbx)      /* PML4[511] */
    
    /* write early_pd into early_pdpt[0] and early_pdpt[510] */
    leaq early_pd(%rip), %rax
    orq $0x3, %rax
    leaq early_pdpt(%rip), %rbx
    movq %rax, 0(%rbx)              /* early_pdpt[0]   */
    movq %rax, (510 * 8)(%rbx)      /* early_pdpt[510] */

    /* set CR3 to point at early_pml4 */
    leaq early_pml4(%rip), %rax
    movq %rax, %cr3

    /* save the boot_info pointer passed from UEFI */
    movq %rdi, %r12

    /* pivot stack pointer to the higher half */
    leaq kernel_stack_top(%rip), %rsp
    andq $~0xF, %rsp
    xorq %rbp, %rbp

    /* Clear BSS */
    xorl %eax, %eax
    leaq __bss_start(%rip), %rdi
    leaq __bss_stop(%rip), %rcx
    subq %rdi, %rcx
    shrq $3, %rcx
    rep stosq

/* entry point: start_kernel */
enter_kernel:
    /* restore the boot_info pointer */
    movq %r12, %rdi
    leaq start_kernel(%rip), %rax
    call *%rax
    ud2