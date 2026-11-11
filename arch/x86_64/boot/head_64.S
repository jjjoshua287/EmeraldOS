#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <asm/pgtable_types.h>
#include <asm/page.h>

#include <emerald/init.h>

.macro PMD_ENTS s, f, count
	.set i, 0
	.rept \count
	.quad \s + (i << 21) + \f
	.set i, i + 1
	.endr
.endm

__HEAD_DATA
.balign 4

.balign PAGE_SIZE
.global early_top_pgt
early_top_pgt:
	.quad level3_kernel_pgt + _KERNEL_PAGE_TABLE
	.fill 510, 8, 0
	.quad level3_kernel_pgt + _KERNEL_PAGE_TABLE

.balign PAGE_SIZE
.global early_dynamic_pgts
early_dynamic_pgts:
	.fill 512*EARLY_DYNAMIC_PGTS

.balign PAGE_SIZE
.global init_top_pgt
init_top_pgt:
	.fill 511, 8, 0
	.quad level3_kernel_pgt + _KERNEL_PAGE_TABLE

.balign PAGE_SIZE
.global level3_kernel_pgt
level3_kernel_pgt:
	.fill L3_START_KERNEL, 8, 0
	.quad level2_kernel_pgt + _KERNEL_PAGE_TABLE
	.quad level2_fb_pgt + _PAGE_TABLE

.balign PAGE_SIZE
.global level2_kernel_pgt
level2_kernel_pgt:
	PMD_ENTS 0, _PAGE_KERNEL_LARGE_EXEC, 512

.balign PAGE_SIZE
.global level2_fb_pgt
level2_fb_pgt:
	.fill 512, 8, 0

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

.balign 8
boot_info_copy:
	.fill BOOT_INFO_SIZE, 1, 0

.section .text
__HEAD
.code64

.extern x86_64_start_kernel

.global start_64
.type start_64, @function
start_64:
	cli
	cld

	/* 
	 * copy boot_info out of stub memory while the firmware's
	 * identity map is still in CR3
	 */
	movq %rdi, %rsi
	leaq boot_info_copy(%rip), %rdi
	movq %rdi, %r12
	movl $BOOT_INFO_SIZE, %ecx
	rep movsb

	lgdt early_gdt_desc(%rip)

	movw $__KERNEL_DS, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw %ax, %fs
	movw %ax, %gs

	pushq $__KERNEL_CS
	leaq fix_pgt(%rip), %rax
	pushq %rax
	lretq

fix_pgt:
	leaq __kernel_start(%rip), %rbp

	/* Map Level 3 identity slot */
	movq %rbp, %rax
	shrq $PUD_SHIFT, %rax
	andq $0x1ff, %rax

	leaq level3_kernel_pgt(%rip), %rbx
	leaq level2_kernel_pgt(%rip), %rdx
	orq  $_KERNEL_PAGE_TABLE, %rdx
	movq %rdx, (%rbx, %rax, 8)

	/* Map Level 4 identity slot */
	movq %rbp, %rax
	shrq $PGD_SHIFT, %rax
	andq $0x1ff, %rax

	leaq early_top_pgt(%rip), %rbx
	leaq level3_kernel_pgt(%rip), %rdx
	orq  $_KERNEL_PAGE_TABLE, %rdx
	movq %rdx, (%rbx, %rax, 8)

	/* Check if we were relocated */
	subq $__kernel_start, %rbp
	testq %rbp, %rbp
	jz load_early_pgt

	/* fixup patch tables */
	addq %rbp, init_top_pgt + 0(%rip)
	addq %rbp, init_top_pgt + (511 * 8)(%rip)

	addq %rbp, early_top_pgt + 0(%rip);
	addq %rbp, early_top_pgt + (511 * 8)(%rip)

	addq %rbp, level3_kernel_pgt + (L3_START_KERNEL * 8)(%rip)

load_early_pgt:
	leaq early_top_pgt(%rip), %rax
	movq %rax, %cr3
	movabsq $swap_pgt, %rax
	jmp *%rax

.section .text
swap_pgt:
	/* Load physical address of init_top_pgt into CR3 */
	movabsq $init_top_pgt, %rax
	addq %rbp, %rax
	movq %rax, %cr3

secondary_start_64:
	xorl %eax, %eax
	leaq __bss_start(%rip), %rdi
	leaq __bss_stop(%rip), %rcx
	subq %rdi, %rcx
	shrq $3, %rcx
	rep stosq

	movabsq $kernel_stack_top, %rsp
	xor %rbp, %rbp

	/* hand the kernel a higher-half alias of boot_info_copy */
	movabsq $KERNEL_VMA, %rdi
	addq %r12, %rdi

enter_kernel:
	movabsq $x86_64_start_kernel, %rax
	call *%rax
	ud2

.extern handle_early_fault
.global asm_exc_early_page_fault
asm_exc_early_page_fault:
	pushq $14	/* Vector */
	pushq %rdi
    	pushq %rsi
    	pushq %rdx
    	pushq %rcx
    	pushq %rax
    	pushq %r8
    	pushq %r9
    	pushq %r10
    	pushq %r11
    	pushq %rbx
    	pushq %rbp
    	pushq %r12
    	pushq %r13
    	pushq %r14
    	pushq %r15
	
	movq %rsp, %rdi
	call handle_early_page_fault

	popq %r15
    	popq %r14
    	popq %r13
    	popq %r12
    	popq %rbp
    	popq %rbx
    	popq %r11
    	popq %r10
    	popq %r9
    	popq %r8
    	popq %rax
    	popq %rcx
    	popq %rdx
    	popq %rsi
    	popq %rdi

	/* discard vector & error code */
	addq $16, %rsp
    	iretq