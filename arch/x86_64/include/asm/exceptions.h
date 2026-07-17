#ifndef X86_64_CPU_EXCEPTIONS_H
#define X86_64_CPU_EXCEPTIONS_H

/* 
 * Vector Numbers corresponding to CPU Exceptions/Interrupts 
 * Vectors 15 and 22-27 are reserved
 */
#define EXCEPT_DE     0   /* Division Error */
#define EXCEPT_DB     1   /* Debug */
#define EXCEPT_NMI    2   /* Non-Maskable Interrupt */
#define EXCEPT_BP     3   /* Breakpoint */
#define EXCEPT_OF     4   /* Overflow */
#define EXCEPT_BR     5   /* Bound range exceeded */
#define EXCEPT_UD     6   /* Invalid Opcode */
#define EXCEPT_NM     7   /* Device not available */
#define EXCEPT_DF     8   /* Double Fault */
#define EXCEPT_NM     9   /* Coprocessor Segment Overrun (Deprecated in modern CPUs) */
#define EXCEPT_TS     10  /* Invalid TSS */
#define EXCEPT_NP     11  /* Segment not present */
#define EXCEPT_SS     12  /* Stack-Segment Fault */
#define EXCEPT_GP     13  /* General Protection Fault */
#define EXCEPT_PF     14  /* Page Fault */
#define EXCEPT_MF     16  /* x87 Floating Point Exception */
#define EXCEPT_AC     17  /* Alignment Check */
#define EXCEPT_MC     18  /* Machine Check */
#define EXCEPT_XM     19  /* SIMD Floating-Point Exception */
#define EXCEPT_VE     20  /* Virtualization Exception */
#define EXCEPT_CP     21  /* Control Protection Exception */
#define EXCEPT_HV     28  /* Hypervisor Injection Exception */
#define EXCEPT_VC     29  /* VMM Communication Exception */
#define EXCEPT_SX     30  /* Security Exception */

#endif