#ifndef ACPI_TABLES_H
#define ACPI_TABLES_H

#include "acpi_types.h"
#include <emerald/types.h>
#include <emerald/compiler_types.h>

/* Values for description table header signatures defined in this file */
#define ACPI_SIG_RSDP "RSD PTR" /* Root System Description Pointer */
#define ACPI_SIG_RSDT "RSDT"	/* Root System Description Table */
#define ACPI_SIG_XSDT "XSDT"	/* Extended System Description Table */
#define ACPI_SIG_MADT "APIC" 	/* Multiple APIC Description Table */

/* ACPI Tables defined in ACPI specification */

/** 
 * struct acpi_table_header: common header used by all excepts except RSDP and FACS 
 * @signature: 	  ASCII table signature
 * @length: 	  Length of table in bytes, including this header
 * @revision: 	  ACPI specification minor version number
 * @checksum: 	  To make sum of entire table == 0
 * @oem_id: 	  ASCII OEM Identification
 * @oem_table_id: ASCII OEM table Identification
 * @oem_revision: OEM revision number
 */
struct acpi_table_header {
	char signature[ACPI_NAMESEG_SIZE] ACPI_NONSTRING;
	u32  length;
	u8   revision;
	u8   checksum;
	char oem_id[ACPI_OEM_ID_SIZE] ACPI_NONSTRING;
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE] ACPI_NONSTRING;
	u32  oem_revision;
	char creator_id[ACPI_NAMESEG_SIZE] ACPI_NONSTRING;
	u32  creator_revision;
} __packed;

/*
 *
 * RSDP - Root System Description Pointer
 * 
 */

/* ACPI 2.0 RSDP */
struct acpi_table_rsdp {
	char signature[8]; 	/* contains "RSD PTR " */
	u8   checksum;		/* ACPI 1.0 checksum */
	char oem_id[ACPI_OEM_ID_SIZE];
	u8   revision;		/* Must be (0) for ACPI 1.0 or (2) for ACPI 2.0+ */
	u32  rsdt_physical_addr;
	u32  length;
	u64  xsdt_physical_addr;
	u8   extended_checksum;
	u8   reserved[3];	/* reserved; must be zero. */
} __packed;

/* ACPI 1.0 RSDP */
struct acpi_table_rsdp_common {
	char signature[8];
	u8 checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	u8 revision;
	u32 rsdt_physical_addr;
} __packed;

/*
 *
 * RSDT/XSDT - Root System Description Tables
 * 
 */

/* Root System Description Table */
struct acpi_table_rsdt {
	struct acpi_table_header header;
	u32 table_offset_entry[1]; /* array of pointers to ACPI tables */
} __packed;

/* Extendable System Description Table (64-bit successor to RSDT) */
struct acpi_table_xsdt {
	struct acpi_table_header header; /* array of pointers to ACPI tables */
	u64 table_offset_entry[1];
} __packed;

#define ACPI_RSDT_ENTRY_SIZE (sizeof(u32))
#define ACPI_XSDT_ENTRY_SZIE (sizeof(u64))

/* Generic subtable header (used in MADT, SRAT, etc.) */
struct acpi_subtable_header {
	u8 type;
	u8 length;
} __packed;

/**
 * struct acpi_table_madt - Multiple APIC Description Table (v3)
 * @header: Common ACPI table_header
 * @addr:   Physical address of local APIC (LAPIC)
 * @flags:  Flags, self explanatory
 */
struct acpi_table_madt {
	struct acpi_table_header header;
	u32 addr;
	u32 flags;
} __packed;

/* Masks for Flags field above */

#define ACPI_MADT_PCAT_COMPAT    (1) /* 00: System  also has dual 8259s */

/* Values for PCATCompat flag */

#define ACPI_MADT_DUAL_PIC	 1
#define ACPI_MADT_MULTIPLE_APIC	 0

/* Values for MADT subtable type in struct acpi_subtable_header 
 * 	NOTE: Only Type 0, 1, and 2 are implemented currently
 */
enum AcpiMadtType {
	ACPI_MADT_TYPE_LOCAL_APIC               = 0,
	ACPI_MADT_TYPE_IO_APIC                  = 1,
	ACPI_MADT_TYPE_INTERRUPT_OVERRIDE       = 2,
	ACPI_MADT_TYPE_NMI_SOURCE               = 3,
	ACPI_MADT_TYPE_LOCAL_APIC_NMI           = 4,
	ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE      = 5,
	ACPI_MADT_TYPE_IO_SAPIC                 = 6,
	ACPI_MADT_TYPE_LOCAL_SAPIC              = 7,
	ACPI_MADT_TYPE_INTERRUPT_SOURCE         = 8,
	ACPI_MADT_TYPE_LOCAL_X2APIC             = 9,
	ACPI_MADT_TYPE_LOCAL_X2APIC_NMI         = 10,
	ACPI_MADT_TYPE_GENERIC_INTERRUPT        = 11,
	ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR      = 12,
	ACPI_MADT_TYPE_GENERIC_MSI_FRAME        = 13,
	ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR    = 14,
	ACPI_MADT_TYPE_GENERIC_TRANSLATOR       = 15,
	ACPI_MADT_TYPE_MULTIPROC_WAKEUP         = 16,
	ACPI_MADT_TYPE_CORE_PIC                 = 17,
	ACPI_MADT_TYPE_LIO_PIC                  = 18,
	ACPI_MADT_TYPE_HT_PIC                   = 19,
	ACPI_MADT_TYPE_EIO_PIC                  = 20,
	ACPI_MADT_TYPE_MSI_PIC                  = 21,
	ACPI_MADT_TYPE_BIO_PIC                  = 22,
	ACPI_MADT_TYPE_LPC_PIC                  = 23,
	ACPI_MADT_TYPE_RINTC                    = 24,
	ACPI_MADT_TYPE_IMSIC                    = 25,
	ACPI_MADT_TYPE_APLIC                    = 26,
	ACPI_MADT_TYPE_PLIC                     = 27,
	ACPI_MADT_TYPE_GICV5_IRS                = 28,
	ACPI_MADT_TYPE_GICV5_ITS                = 29,
	ACPI_MADT_TYPE_GICV5_ITS_TRANSLATE      = 30,
	ACPI_MADT_TYPE_RESERVED                 = 31,   /* 31 to 0x7F are reserved */
	ACPI_MADT_TYPE_OEM_RESERVED             = 0x80  /* 0x80 to 0xFF are reserved for OEM use */
};



#endif