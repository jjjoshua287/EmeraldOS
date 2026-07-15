.section .text
extern do_interrupt_handler

/* shared logic between all exception handlers */
isr_wrapper:
    /* push registers */
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rax
    push %r8
    push %r9
    push %r10
    push %r11
    push %rbx
    push %rbp
    push %r12
    push %r13
    push %r14
    push %r15

    /* enter kernel */
    mov %rsp, %rcx  /* frame pointer arg */
    sub $40, %rsp   /* 32 bytes shadow space + 8 bytes padding to fix alignment */
    call do_interrupt_handler
    add $40, %rsp   /* restore rsp */

    /* pop registers */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbp
    pop %rbx
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rax
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi

    /* discard vector + error code */
    add $16, %rsp
    iretq

.macro ISR_NOERR vec
.global isr_stub_\vec
isr_stub_\vec:
    push $0     /* dummy error code */
    push $\vec
    jmp isr_wrapper
.endm

.macro ISR_ERR vec
.global istr_stub_\vec
    push $\vec
    jmp isr_wrapper
.endm

.irp vec, 8, 10, 11, 12, 13, 14, 17, 21, 29, 30
    ISR_ERR \vec
.endr

.irp vec, 0, 1, 2, 3, 4, 5, 6, 7, 9, 15, 16, 18, 19, 20, 22, 23, 24, 25, 26, 27, 28, 31
    ISR_NOERR \vec
.endr

.altmacro
.set vec, 32
.rept 224
    ISR_NOERR %vec
    .set vec, vec+1
.endr
.noaltmacro

.section rodata
.global isr_stub_table
isr_stub_table:
.irp vec, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    .quad isr_stub_\vec
.endr

.altmacro
.set vec, 32
.rept 224
    .quad isr_stub_%vec
    vec = vec + 1
.endr
.noaltmacro