ENTRY(start_64)

PHDRS
{
    text   PT_LOAD FLAGS(5);    /* R+X */
    rodata PT_LOAD FLAGS(4);    /* R   */
    data   PT_LOAD FLAGS(6);    /* R+W */
}

SECTIONS
{
    . = 1M;
    __kernel_start = .;
    
    .text : { 
        *(.text .text.*) 
    } :text

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    } :rodata

    .data : ALIGN(4K) {
        *(.data .data.*) 
    } :data
  
    .bss : ALIGN(8) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_stop = .;
    }

    . = ALIGN(4K);
    __kernel_end = .;
}