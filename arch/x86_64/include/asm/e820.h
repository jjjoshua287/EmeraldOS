#ifndef ASM_X86_E820_H
#define ASM_X86_E820_H

#include <emerald/types.h>

/* These are the E820 Types known to the kernel */
enum e820_type {
	E820_TYPE_RAM = 1,
	E820_TYPE_RESERVED = 2,
	E820_TYPE_ACPI = 3,
	E820_TYPE_NVS = 4,
	E820_TYPE_UNUSABLE = 5,
	E820_TYPE_PMEM = 7,
};

/* 
 * A single E820 map entry, describing a memory range of [addr...addr+size-1],
 * of 'type' memory type.
 */
struct e820_entry {
	u64 		addr;
	u64 		size;
	enum e820_type 	type;
};

#define E820_MAX_ENTRIES 128

/* The whole array of E820 entries */
struct e820_table {
	u32 nr_entries;
	struct e820_entry entries[E820_MAX_ENTRIES];
};

#endif