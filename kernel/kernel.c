#include <emerald/efi.h>
#include <emerald/fbcon.h>
#include <emerald/printk.h>

#define RED    (0x00FF0000)
#define GREEN  (0x0000FF00)
#define BLUE   (0x000000FF)
#define YELLOW (0x00FFFF00)

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

// entry point into kernel, ExitBootServices should be called before entering this
int start_kernel()
{
        init_fbcon(&info);
        kernel_main();

        return 0;
}

efi_status_t efi_main(efi_handle_t ImageHandle, struct efi_system_table *SystemTable)
{
        //SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello Kernel!");
        do_efi_things(SystemTable);
        /* 
         * do_efi_things will eventually be moved into efistub.c and become the entry point of the EFI Phase.
         * more things here...
         * call ExitBootServices() 
         * pass a pointer to struct screen_info to kernel_main via a CPU Register (Assembly code here) 
         */
        
        start_kernel();
        return EFI_SUCCESS;
}