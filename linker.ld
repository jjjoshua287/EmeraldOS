ENTRY(start_64)

KERNEL_LMA = 1M;
KERNEL_VMA = 0xFFFFFFFF80000000;
KERNEL_STACK_SIZE = 0x4000; /* 16 KB */

SECTIONS
{
    . = KERNEL_LMA;
    __kernel_start = .;

    .head.text : {
        *(.head.text)
    }

    /* Higher half VMA here */
    . += KERNEL_VMA;
    

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) { 
        *(.text .text.*)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata .rodata.*)
    }

    .data ALIGN(4K): AT(ADDR(.data) - KERNEL_VMA) {
        *(.data .data.*) 
    }
  
    .bss (NOLOAD) : ALIGN(4K) {
        . = ALIGN(8);
        __bss_start = .;

        *(.bss .bss.*)
        *(COMMON)
        
        . = ALIGN(8);
        __bss_stop = .;
    }

    .stack (NOLOAD) : AT(ADDR(.stack) - KERNEL_VMA) ALIGN(16) {
        kernel_stack_bottom = .;
        . += KERNEL_STACK_SIZE;
        . = ALIGN(16);
        kernel_stack_top = .;
    }

    . = ALIGN(4K);
    __kernel_end = . - KERNEL_VMA;
}