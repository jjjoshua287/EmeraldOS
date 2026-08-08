#include <acpi/acpi.h>

#include <emerald/panic.h>
#include <emerald/runtime.h>
#include <emerald/types.h>
#include <emerald/string.h>

/* this is a generic function */
static bool __acpi_checksum(void *ptr, size_t len)
{
	u8 *bytes = (u8 *)ptr;
	u8 sum = 0;

    	for (size_t i = 0; i < len; i++)
		sum += bytes[i];
    	return sum == 0;
}

#define acpi_validate_table(hdr) __acpi_checksum((hdr), (hdr)->length)

static bool validate_rsdp(struct acpi_table_rsdp *rsdp)
{
	if (rsdp == NULL || memcmp(rsdp->signature, ACPI_SIG_RSDP, 8))
        	return false;

	size_t l = (rsdp->revision >= 2) ? rsdp->length : 20;
	return __acpi_checksum(rsdp, l);
}

#define sdt_entries(len, step) ((len) - sizeof((hdr))) / step
#define xsdt_entries(len) sdt_entries((len), ACPI_XSDT_ENTRY_SIZE >> 3)
#define rsdt_entries(len) sdt_entries((len), ACPI_RSDT_ENTRY_SIZE >> 3)

/* The kernel panics if acpi fails to init for easier debugging purposes. 
 * In the future, it will disable acpi instead of panicking.
 */
void acpi_boot_init()
{
        struct acpi_table_rsdp *rsdp = (struct acpi_table_rsdp *)&boot.rsdp;
        if (validate_rsdp(rsdp))
                panic("Failure to initialize acpi, RSDP is invalid", NULL);
 
        struct acpi_table_header *hdr;
	if (rsdp->revision >= 2 && rsdp->xsdt_physical_addr != 0)
		hdr = (struct acpi_table_header *)rsdp->xsdt_physical_addr;
	else
		hdr = (struct acpi_table_header *)(uintptr_t)rsdp->rsdt_physical_addr;
	if (!acpi_validate_table(hdr))
		panic("RSDT/XSDT checksum failed", NULL);
}