#ifndef ASM_X86_64_EXC_PAGE_FAULT_H
#define ASM_X86_64_EXC_PAGE_FAULT_H

#include <asm/ptrace.h>

#define X86_PF_PROT     (1 << 0)    /* protection fault */
#define X86_PF_WRITE    (1 << 1)    /* write access */
#define X86_PF_USER     (1 << 2)    /* user-mode access */
#define X86_PF_RSVD     (1 << 3)    /* reserved bit */
#define X86_PF_INSTR    (1 << 4)    /* instruction fetch */
#define X86_PF_PK       (1 << 5)    /* protection key */

void handle_page_fault(struct pt_regs *regs);

#endif