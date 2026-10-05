#include <asm/pages.h>
#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <emerald/init.h>

.section .head.data, "aw", @progbits

/* early page tables */
.balign PT_SIZE
early_pml4:
    .zero PT_SIZE
early_pdpt:
    .zero PT_SIZE
early_pd_kernel:
    .zero PT_SIZE

early_pd_mmio:
    .zero PT_SIZE

/* early Global Descriptor Table (GDT) */
.balign 8
early_gdt_start:
    .quad 0x0                                               /* 0x00: null */
    .quad GDT_ENTRY(DESC_CODE64, 0, 0xFFFFF)                /* 0x08: kernel code */
    .quad GDT_ENTRY(DESC_DATA64, 0, 0xFFFFF)                /* 0x10: kernel data */
    .quad GDT_ENTRY(DESC_CODE64 | DESC_USER, 0, 0xFFFFF)    /* 0x18: user code */
    .quad GDT_ENTRY(DESC_DATA64 | DESC_USER, 0, 0xFFFFF)    /* 0x20: user data */
early_gdt_end:

.balign 4
early_gdt_desc:
    .word (early_gdt_end - early_gdt_start - 1)
    .quad early_gdt_start

.section .text
__HEAD
.code64

.extern x86_64_start_kernel
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
    movw $__KERNEL_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    /* reload code segment */
    pushq $__KERNEL_CS
    leaq .setup_pgt(%rip), %rax
    pushq %rax
    lretq

.setup_pgt:
    /* setup initial page tables */
    leaq early_pdpt(%rip), %rax
    orq $PTE_KERNEL, %rax
    leaq early_pml4(%rip), %rbx
    movq %rax, (511 * PT_ENTRY_SIZE)(%rbx)     /* Store into PML4[511] for higher-half */
    
    movq %rax, 0(%rbx)  /* store into PML4[0] so CPU doesn't crash when we reload CR3 */

    /* link early_pdpt[510] and early_pdpt[0] to PD */
    leaq early_pd_kernel(%rip), %rax
    orq $PTE_KERNEL, %rax
    leaq early_pdpt(%rip), %rbx
    movq %rax, (510 * PT_ENTRY_SIZE)(%rbx)
    movq %rax, 0(%rbx)

    /* map kernel pages */
    leaq early_pd_kernel(%rip), %rdi
    movq $PTE_KERNEL_BIG, %rsi
    xorq %rdx, %rdx
    call map_pages

    /* map the framebuffer */
    leaq early_pd_mmio(%rip), %rax
    orq $PTE_KERNEL, %rax
    movq %rax, (2 * PT_ENTRY_SIZE)(%rbx)

    /* map mmio pages (currently framebuffer) */
    leaq early_pd_mmio(%rip), %rdi
    movq $PTE_MMIO, %rsi
    movq BOOT_INFO_FB_BASE(%r12), %rdx
    call map_pages

    leaq early_pml4(%rip), %rax
    movq %rax, %cr3
    jmp prepare_for_entry

/* 
 * map pages into a Page Directory
 * %rdi - effective address of a Page Directory
 * %rsi - flags
 * %rdx - physical address accumulator
 *
 * Clobbers: %rcx, %rdx, %r8
 */
.type map_pages, @function
map_pages:
    xor %rcx, %rcx  /* index counter */
.map_page:
    mov %rdx, %r8
    orq %rsi, %r8
    movq %r8, (%rdi, %rcx, PT_ENTRY_SIZE)  /* pd[%rcx] = %rbx */
    addq $KERNEL_PAGE_SIZE, %rdx
    incq %rcx
    cmpq $PT_ENTRIES, %rcx
    jne .map_page
    ret

prepare_for_entry:
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

/* entry point: x86_64_start_kernel */
enter_kernel:
    movabsq $x86_64_start_kernel, %rax
    call *%rax
    ud2