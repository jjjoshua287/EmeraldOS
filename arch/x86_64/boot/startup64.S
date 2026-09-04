#include <asm/stack.h>

.set stack_top, KERNEL_STACK_SIZE

/* 
 * This will need to be swapped to an architecture-specific version
 * if I ever add support for a different architecture, but that would
 * only occur after the kernel is complete for the x86_64 architecture.
 */
.extern start_kernel
.extern kernel_stack

.global start_64
start_64:
    cli
    cld
    
    /*
    setup kernel data segments
    setup initial page tables (pml4) 
    set CR3 to point at initial page table
    */

    /* TODO: Clear BSS */

switch_stack:
    leaq kernel_stack+stack_top(%rip), %rsp
    andq $~0xF, %rsp
    xorq %rbp, %rbp
    subq $0x20, %rsp /* Windows x64 ABI requires shadow space for calls */
    leaq start_kernel(%rip), %rax
    call *%rax
    ud2