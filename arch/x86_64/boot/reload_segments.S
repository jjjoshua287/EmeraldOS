#include <asm/segment.h>

.global reload_segments
.text

.set KERNEL_CS, __KERNEL_CS
.set KERNEL_DS, __KERNEL_DS

reload_segments:
    /* Reload the data/stack segments */
    mov $KERNEL_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    /* fs and gs are left as NULL selectors for per-CPU data in the future */
    xor %ax, %ax
    mov %ax, %fs
    mov %ax, %gs

    /* far-return to flush the Code Segment (CS) register */
    pushq $KERNEL_CS
    lea .Lreload_cs(%rip), %rax
    pushq %rax
    lretq

.Lreload_cs:
    ret