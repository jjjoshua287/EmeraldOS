#include <emerald/runtime.h>

void acpi_boot_init(void *sys_desc_ptr);

/* this will do more in the future, for now it's currently a wrapper for acpi_boot_init */

void setup_arch(struct boot_info *boot) {
        acpi_boot_init(boot->rsdp);
}