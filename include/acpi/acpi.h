#ifndef EFI_ACPI_H
#define EFI_ACPI_H

/* 
    GUIDs used for entries in the EFI system table.
    
    These GUIDs point to the ACPI tables defined in the ACPI specifications.
	- These GUIDs are defined in the UEFI 2.0 specification.
 */

#define ACPI_TABLE_GUID \
  { 0xeb9d2d30, 0x2d88, 0x11d3, \
	{0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
 
/* GUID for ACPI 2.0 Table */
#define EFI_ACPI_TABLE_GUID \
  { 0x8868e871, 0xe4f1, 0x11d3, \
	{0xbc, 0x22, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 } }
 
#define ACPI_10_TABLE_GUID  	ACPI_TABLE_GUID
#define EFI_ACPI_20_TABLE_GUID  EFI_ACPI_TABLE_GUID

#endif  /* EFI_ACPI_H */