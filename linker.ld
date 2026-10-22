ENTRY(start_64)

KERNEL_LMA = 1M;
KERNEL_VMA = 0xFFFFFFFF80000000;
KERNEL_STACK_SIZE = 0x4000; /* 16 KB */

SECTIONS
{
	. = KERNEL_LMA;
    	__kernel_start = .;

    	.head.text : {
    	    KEEP(*(.head.text))
    	}

    	.head.data : {
    	    *(.head.data)
    	}

    	/* Higher half VMA here */
    	. = KERNEL_VMA + .;
    	__kernel_virt_start = .;

    	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) { 
    	    *(.text .text.*)
    	}

    	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) {
    	    *(.rodata .rodata.*)
    	}

    	.data ALIGN(4K): AT(ADDR(.data) - KERNEL_VMA) {
    	    *(.data .data.*) 
    	}
  
	. = ALIGN(4K);
    	.bss (NOLOAD) : AT(ADDR(.bss) - KERNEL_VMA) {
    	    . = ALIGN(8);
    	    __bss_start = .;

        	*(.bss .bss.*)
        	*(COMMON)
        
        	. = ALIGN(8);
        	__bss_stop = .;
    	}
	
	. = ALIGN(16);
    	.stack (NOLOAD) : AT(ADDR(.stack) - KERNEL_VMA) {
        	kernel_stack_bottom = .;
        	. += KERNEL_STACK_SIZE;
        	kernel_stack_top = .;
    	}

    	. = ALIGN(4K);
    	__kernel_virt_end = .;
    
    	__kernel_end = . - KERNEL_VMA;
}

/* high-half aliases of low-linked head data, for use from C */
level2_kernel_pgt_va = level2_kernel_pgt + KERNEL_VMA;