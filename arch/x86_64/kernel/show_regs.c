#include <emerald/printk.h>
#include <asm/processor.h>
#include <asm/ptrace.h>

void __show_regs(struct pt_regs *regs)
{
        printk("Vector: 0x%02llx  Error Code: 0x%02llx\n", regs->vector, regs->error_code);
        printk("RIP: %016llx  CS: %04llx\n", regs->rip, regs->cs);
        printk("RSP: %016llx  SS: %04llx\n", regs->rsp, regs->ss);
        printk("RFLAGS: %016llx\n", regs->eflags);
        
        printk("RAX: %016llx  RBX: %016llx  RCX: %016llx\n",
                regs->rax, regs->rbx, regs->rcx);
        printk("RDX: %016llx  RSI: %016llx  RDI: %016llx\n",
                regs->rdx, regs->rsi, regs->rdi);
        printk("RBP: %016llx  R08: %016llx  R09: %016llx\n",
                regs->rbp, regs->r8, regs->r9);
        printk("R10: %016llx  R11: %016llx  R12: %016llx\n",
                regs->r10, regs->r11, regs->r12);
        printk("R13: %016llx  R14: %016llx  R15: %016llx\n",
                regs->r13, regs->r14, regs->r15);
 
        /* Get control state registers */
        printk("CR0: %016llx CR2: %016llx CR3: %016llx CR4: %016llx CR8: %016llx\n",
                native_read_cr0(), native_read_cr2(), native_read_cr3(), native_read_cr4(), native_read_cr8());
}