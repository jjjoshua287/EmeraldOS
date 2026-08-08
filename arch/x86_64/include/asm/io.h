#ifndef ASM_X86_64_IO_H
#define ASM_X86_64_IO_H

/* inline assembly for writing to hardware ports */

static inline void outb(unsigned char value, unsigned short port)
{
    asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
}

#endif