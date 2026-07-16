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

.macro ISR_NOERR name, vec
.global asm_isr_\name
asm_isr_\name:
    push $0     /* dummy error code */
    push $\vec
    jmp isr_wrapper
.endm

.macro ISR_ERR name, vec
.global asm_isr_\name
    push $\vec
    jmp isr_wrapper
.endm

.altmacro
.set vec, 32
.rept 224
    ISR_NOERR asm_irq%vec, %vec
    .set vec, vec+1
.endr
.noaltmacro

ISR_NOERR divide_error,          0
ISR_NOERR debug,                 1
ISR_NOERR nmi,                   2
ISR_NOERR breakpoint,            3
ISR_NOERR overflow,              4
ISR_NOERR bound_range,           5
ISR_NOERR invalid_opcode,        6
ISR_NOERR device_not_available,  7
ISR_ERR   double_fault,          8
ISR_NOERR coprocessor_segment,   9
ISR_ERR   invalid_tss,          10
ISR_ERR   segment_not_present,  11
ISR_ERR   stack_segment,        12
ISR_ERR   general_protection,   13
ISR_ERR   page_fault,           14
ISR_NOERR x87_fpe,              16
ISR_ERR   alignment_check,      17
ISR_NOERR machine_check,        18
ISR_NOERR simd_fpe,             19
ISR_NOERR virtualization,       20
ISR_ERR   control_protection,   21
ISR_NOERR hypervisor_injection, 28
ISR_ERR   vmm_comms_exception,  29
ISR_ERR   security_exception,   30