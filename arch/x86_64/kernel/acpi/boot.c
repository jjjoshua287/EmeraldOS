#include <acpi/acpi.h>

#include <asm/page.h>

#include <emerald/runtime.h>
#include <emerald/types.h>
#include <emerald/string.h>
#include <emerald/printk.h>

/* this is a generic function */
static bool __acpi_checksum(void *ptr, size_t len)
{
	u8 *bytes = (u8 *)ptr;
	u8 sum = 0;

    	for (size_t i = 0; i < len; i++)
		sum += bytes[i];
    	return sum == 0;
}

static bool acpi_validate_table(struct acpi_table_header *virt_hdr)
{
	return __acpi_checksum(virt_hdr, virt_hdr->length);
}

static bool validate_rsdp(struct acpi_table_rsdp *rsdp)
{
	/* the memcmp function is what caused the Page Fault */
	if (rsdp == NULL || memcmp(rsdp->signature, ACPI_SIG_RSDP, 8))
        	return false;

	size_t l = (rsdp->revision >= 2) ? rsdp->length : 20;
	return __acpi_checksum(rsdp, l);
}

#define sdt_entries(len, step) ((len) - sizeof((hdr))) / step
#define xsdt_entries(len) sdt_entries((len), ACPI_XSDT_ENTRY_SIZE >> 3)
#define rsdt_entries(len) sdt_entries((len), ACPI_RSDT_ENTRY_SIZE >> 3)

void acpi_boot_init(void *sys_desc_ptr)
{
        struct acpi_table_rsdp *rsdp = (struct acpi_table_rsdp *)sys_desc_ptr;
        if (!validate_rsdp(rsdp)) {
                printk("ACPI: Failed to initialize; Reason: Invalid RSDP\n");
		return;
	}

	phys_addr_t paddr;
	if (rsdp->revision >= 2 && rsdp->xsdt_physical_addr != 0)
		paddr = rsdp->xsdt_physical_addr;
	else
		paddr = (phys_addr_t)rsdp->rsdt_physical_addr;

	struct acpi_table_header *hdr = phys_to_virt(paddr);
	if (!acpi_validate_table(hdr)) {
		printk("ACPI: Failed to find System Descriptor Table\n");
		return;
	}
	
	const char *tbl_str = (rsdp->revision >= 2) ? "XSDT" : "RSDT";
	printk("ACPI: Successfully located %s\n", tbl_str);
}