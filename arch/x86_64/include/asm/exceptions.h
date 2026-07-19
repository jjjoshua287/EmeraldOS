#ifndef X86_64_CPU_EXCEPTIONS_H
#define X86_64_CPU_EXCEPTIONS_H

/* 
 * Vector Numbers corresponding to CPU Exceptions/Interrupts 
 * Vectors 15 and 22-27 are reserved
 */
#define EXC_DE          0   /* Division Error */
#define EXC_DB          1   /* Debug */
#define EXC_NMI         2   /* Non-Maskable Interrupt */
#define EXC_BP          3   /* Breakpoint */
#define EXC_OF          4   /* Overflow */
#define EXC_BR          5   /* Bound range exceeded */
#define EXC_UD          6   /* Invalid Opcode */
#define EXC_NM          7   /* Device not available */
#define EXC_DF          8   /* Double Fault */
#define EXC_NM          9   /* Coprocessor Segment Overrun (Deprecated in modern CPUs) */
#define EXC_TS          10  /* Invalid TSS */
#define EXC_NP          11  /* Segment not present */
#define EXC_SS          12  /* Stack-Segment Fault */
#define EXC_GP          13  /* General Protection Fault */
#define EXC_PF          14  /* Page Fault */
#define EXC_SPURIOUS    15  /* Spurious Interrupt (Intel Reserved) */
#define EXC_MF          16  /* x87 Floating Point Exception */
#define EXC_AC          17  /* Alignment Check */
#define EXC_MC          18  /* Machine Check */
#define EXC_XM          19  /* SIMD Floating-Point Exception */
#define EXC_VE          20  /* Virtualization Exception */
#define EXC_CP          21  /* Control Protection Exception */
#define EXC_HV          28  /* Hypervisor Injection Exception */
#define EXC_VC          29  /* VMM Communication Exception */
#define EXC_SX          30  /* Security Exception */

/* Vectors 22-27 are also Intel Reserved */

#endif