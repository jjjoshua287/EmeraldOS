#include <asm/stack.h>
#include <asm/ptrace.h>
#include <asm/processor.h>

#include <emerald/printk.h>
#include <emerald/compiler.h>

static inline bool is_valid_stack_addr(unsigned char *rbp)
{
        return (rbp >= kernel_stack && rbp <= (kernel_stack + KERNEL_STACK_SIZE - sizeof(struct stack_frame)));
}

void __dump_stack(struct pt_regs *regs)
{
        int depth = 0;
        struct stack_frame *frame;
        if (regs == NULL || unlikely(!regs->rbp)) {
                frame = (struct stack_frame *)native_read_rbp();
        } else {
                frame = (struct stack_frame *)regs->rbp;
                printk("#%d 0x%016llx\n", depth++, regs->rip);
        }

        while (depth < MAX_FRAMES && is_valid_stack_addr((unsigned char *)frame)) {
                printk("#%d 0x%016llx\n", depth++, frame->ret_addr);
                frame = frame->rbp;
        }
}