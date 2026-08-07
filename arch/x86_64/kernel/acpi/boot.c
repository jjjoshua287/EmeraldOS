#include <acpi/acpi.h>

#include <emerald/panic.h>
#include <emerald/runtime.h>
#include <emerald/types.h>
#include <emerald/string.h>

static bool checksum_valid(void *ptr, size_t len)
{
	u8 *bytes = (u8 *)ptr;
	u8 sum = 0;

    	for (size_t i = 0; i < len; i++)
		sum += bytes[i];
    	return sum == 0;
}

static bool validate_rsdp(struct acpi_table_rsdp *rsdp)
{
	if (rsdp == NULL || memcmp(rsdp->signature, ACPI_SIG_RSDP, 8))
        	return false;

	size_t l = (rsdp->revision >= 2) ? rsdp->length : 20;
	return checksum_valid(rsdp, l);
}

#define xsdt_entries(len, hdr) ((len) - sizeof((hdr))) / 8

void acpi_boot_init(struct boot_info *boot)
{
        struct acpi_table_rsdp *rsdp = (struct acpi_table_rsdp *)boot->rsdp;
        if (!validate_rsdp(rsdp))
                panic("Failure to initalize acpi, RSDP is invalid", NULL);
}