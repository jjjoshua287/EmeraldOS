#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <emerald/init.h>

.section .head.data, "aw", @progbits

/* early page tables */
.balign 4096
early_pml4:
    .zero 4096
early_pdpt:
    .zero 4096
early_pd:
    .zero 4096

early_pd2:
    .zero 4096

.balign 8
early_gdt_start:
    .quad 0x0
    .quad GDT_ENTRY(DESC_CODE64, 0, 0xFFFFF)
    .quad GDT_ENTRY(DESC_DATA64, 0, 0xFFFFF)
    .quad GDT_ENTRY(DESC_CODE64 | DESC_USER, 0, 0xFFFFF)
    .quad GDT_ENTRY(DESC_DATA64 | DESC_USER, 0, 0xFFFFF)
early_gdt_end:

.balign 4
early_gdt_desc:
    .word (early_gdt_end - early_gdt_start - 1)
    .quad early_gdt_start

.section .text
__HEAD
.code64

.set KERNEL_CS, __KERNEL_CS
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

    /* save the boot_info pointer passed from UEFI */
    movq %rdi, %r12

.reload_gdt:
    lgdt early_gdt_desc(%rip)

    /* setup kernel data segments */
    movw $KERNEL_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    /* reload data segment registers */
    pushq $KERNEL_CS
    leaq .setup_pgt(%rip), %rax
    pushq %rax
    lretq

.setup_pgt:
    /* setup initial page tables */
    leaq early_pdpt(%rip), %rax
    orq $0x3, %rax                  /* Present (0x1) + Read/Write (0x2) */
    leaq early_pml4(%rip), %rbx
    movq %rax, (511 * 8)(%rbx)      /* Store into PML4[511] for higher-half */
    
    movq %rax, 0(%rbx)  /* store into PML4[0] so CPU doesn't crash when we reload CR3 */

    /* link early_pdpt[510] and early_pdpt[0] to PD */
    leaq early_pd(%rip), %rax
    orq $0x3, %rax                  /* Present + Read/Write */
    leaq early_pdpt(%rip), %rbx
    movq %rax, (510 * 8)(%rbx)      /* early_pdpt[510] */
    movq %rax, 0(%rbx)              /* early_pdpt[0] */

    /* map the framebuffer */
    leaq early_pd2(%rip), %rax
    orq $0x3, %rax
    movq %rax, (2 * 8)(%rbx)        /* early_pdpt[2] */

    leaq early_pd(%rip), %rdi
    xorq %rcx, %rcx     /* Index counter */
    xorq %rsi, %rsi     /* Physical address accumulator */

.map_kernel_pages:
    movq %rsi, %rbx
    orq $0x83, %rbx             /* Present + Read/Write + Page Size */
    movq %rbx, (%rdi, %rcx, 8)  /* early_pd[rcx] = rbx */
    addq $0x200000, %rsi
    incq %rcx
    cmpq $512, %rcx
    jne .map_kernel_pages

    leaq early_pd2(%rip), %rdi
    xorq %rcx, %rcx
    movq $0x80000000, %rsi

/* HACK: Do it dynamically once I implement a Page Table mapper */
.map_mmio_pages:
    movq %rsi, %rbx
    orq $0x9B, %rbx     /* Present + Read/Write + PWT + PCD + Accessed */
    mov %rbx, (%rdi, %rcx, 8)
    addq $0x200000, %rsi
    incq %rcx
    cmpq $512, %rcx
    jne .map_mmio_pages

    /* set CR3 to point at early_pml4 */
    leaq early_pml4(%rip), %rax
    movq %rax, %cr3

.prepare_for_entry:
    /* Clear BSS */
    xorl %eax, %eax
    leaq __bss_start(%rip), %rdi
    leaq __bss_stop(%rip), %rcx
    subq %rdi, %rcx
    shrq $3, %rcx
    rep stosq

    /* Pivot the stack pointer into virtual address space */
    movabsq $kernel_stack_top, %rsp
    xor %rbp, %rbp

    /* restore the boot_info pointer */
    movq %r12, %rdi

/* entry point: start_kernel */
enter_kernel:
    movabsq $start_kernel, %rax
    call *%rax
    ud2