#include <emerald/runtime.h>

void setup_gdt(void);
void setup_idt(void);
void acpi_boot_init(void *sys_desc_ptr);

void setup_arch() {
        setup_gdt();
        setup_idt();
        acpi_boot_init(boot.rsdp);
}