#include <asm/stack.h>
#include <emerald/compiler.h>

unsigned char kernel_stack[KERNEL_STACK_SIZE] __aligned(16);