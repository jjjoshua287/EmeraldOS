#ifndef X86_64_TRAP_NR_H
#define X86_64_TRAP_NR_H

/* Vector Numbers corresponding to CPU Exceptions/Interrupts */
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
#define TRAP_PF     14  /* Page Fault*/
/* 15 is Reserved */
#define TRAP_MF     16    
#define TRAP_AC     17  
#define TRAP_MC     18  
#define TRAP_XM     19  
#define TRAP_VE     20  
#define TRAP_CP     21  
/* 22-27 are reserved */
#define TRAP_HV     28  
#define TRAP_VC     29  
#define TRAP_SX     30  

#endif