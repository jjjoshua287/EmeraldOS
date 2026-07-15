#ifndef X86_64_TRAP_NR_H
#define X86_64_TRAP_NR_H

/* 
 * Vector Numbers corresponding to CPU Exceptions/Interrupts 
 * Vectors 15 and 22-27 are reserved
 */
#define TRAP_DE     0   /* Division Error */
#define TRAP_DB     1   /* Debug */
#define TRAP_NMI    2   /* Non-Maskable Interrupt */
#define TRAP_BP     3   /* Breakpoint */
#define TRAP_OF     4   /* Overflow */
#define TRAP_BR     5   /* Bound range exceeded */
#define TRAP_UD     6   /* Invalid Opcode */
#define TRAP_NM     7   /* Device not available */
#define TRAP_DF     8   /* Double Fault */
#define TRAP_NM     9   /* Coprocessor Segment Overrun (Deprecated in modern CPUs) */
#define TRAP_TS     10  /* Invalid TSS */
#define TRAP_NP     11  /* Segment not present */
#define TRAP_SS     12  /* Stack-Segment Fault */
#define TRAP_GP     13  /* General Protection Fault */
#define TRAP_PF     14  /* Page Fault */
#define TRAP_MF     16  /* x87 Floating Point Exception */
#define TRAP_AC     17  /* Alignment Check */
#define TRAP_MC     18  /* Machine Check */
#define TRAP_XM     19  /* SIMD Floating-Point Exception */
#define TRAP_VE     20  /* Virtualization Exception */
#define TRAP_CP     21  /* Control Protection Exception */
#define TRAP_HV     28  /* Hypervisor Injection Exception */
#define TRAP_VC     29  /* VMM Communication Exception */
#define TRAP_SX     30  /* Security Exception */

#endif