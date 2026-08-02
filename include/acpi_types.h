#ifndef ACPI_TYPES_H
#define ACPI_TYPES_H

/*
 * ACPI Specification constatns
*/
#define ACPI_NAMESEG_SIZE       4     /* Fixed by ACPI spec */

/* Sizes for ACPI table headers */
#define ACPI_OEM_ID_SIZE        6
#define ACPI_OEM_TABLE_ID_SIZE  8

#ifndef ACPI_NONSTRING
#define ACPI_NONSTRING  /* String with no NUL terminator */

#endif