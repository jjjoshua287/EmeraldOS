#include <asm/ptrace.h>
#include <asm/processor.h>

#include <emerald/printk.h>
#include <emerald/compiler.h>

#ifdef CFG_NO_FRAME_POINTER
void __dump_stack(struct pt_regs *regs)
{

}

#else 

extern unsigned char kernel_stack_bottom[];
extern unsigned char KERNEL_STACK_SIZE[];

const unsigned long kernel_stack_size = (unsigned long)KERNEL_STACK_SIZE;

static inline bool is_valid_stack_addr(unsigned char *rbp)
{
        return (rbp >= kernel_stack_bottom && rbp <= (kernel_stack_bottom + kernel_stack_size - sizeof(struct stack_frame)));
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
#endif /* !CFG_NO_FRAME_POINTER */