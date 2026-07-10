#include <emerald/runtime.h>
#include <emerald/fbcon.h>
#include <emerald/printk.h>

int kernel_main()
{
        int (*self_ptr)() = kernel_main;

        printk("Integer: %d\n", 42);
        printk("Unsigned Integer: %u\n", (unsigned int)37);
        printk("Pointer: %p\n", (void *)self_ptr);
        printk("Hexadecimal: %x\n", 0xdeadbeef);
        printk("String: %s\n", "The cake is a lie.");
        printk("char: %c\n", 'A');
        printk("default: %%\n");

        while (1);
        return 0;
}

#include <asm/gdt.h>
#include <asm/segment.h>

// entry point into kernel, ExitBootServices should be called before entering this
int start_kernel(void)
{
        setup_gdt();
        init_fbcon(&info);
        kernel_main();
        return 0;
}