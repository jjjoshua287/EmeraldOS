#ifndef ASM_E820_API_H
#define ASM_E820_API_H

#include <emerald/types.h>
#include <asm/e820/types.h>

/* Function prototypes for e820 functions called by the EmeraldOS Kernel */

const char *e820_type_name(enum e820_type type);
void e820_print_table(const struct e820_table *table);
void e820_insert_entry(struct e820_table *table, u64 addr, u64 size, enum e820_type type);
void e820_sanitize_table(struct e820_table *table);
int e820_memblock_setup(struct e820_table *table);

#endif