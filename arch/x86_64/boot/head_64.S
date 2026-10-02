#include <emerald/init.h>

.section .text
__HEAD
.code64

.set stack_top, KERNEL_STACK_SIZE

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

    /*
    setup kernel data segments
    setup initial page tables (pml4) 
    set CR3 to point at initial page table
    */

    /* save the boot_info pointer passed from UEFI */
    movq %rdi, %r12

    /* Clear BSS */
    xorl %eax, %eax
    leaq __bss_start(%rip), %rdi
    leaq __bss_stop(%rip), %rcx
    subq %rdi, %rcx
    shrq $3, %rcx
    rep stosq

    /* Replace the now-invalid UEFI stack to a kernel-provided one */
    leaq kernel_stack_top(%rip), %rsp
    andq $~0xF, %rsp
    xorq %rbp, %rbp

/* entry point: start_kernel */
enter_kernel:
    /* restore the boot_info pointer */
    movq %r12, %rdi
    leaq start_kernel(%rip), %rax
    call *%rax
    ud2