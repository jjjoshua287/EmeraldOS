#ifndef X86_64_STACK_H
#define X86_64_STACK_H

#define KERNEL_STACK_SIZE (1024 * 32)

#ifndef __ASSEMBLER__
extern unsigned char kernel_stack[KERNEL_STACK_SIZE];
#endif
#endif