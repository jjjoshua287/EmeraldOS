#ifndef ASM_E820_API_H
#define ASM_E820_API_H

#include <emerald/types.h>
#include <emerald/runtime.h>

#include <asm/e820/types.h>

/* Function prototypes for e820 functions called by the EmeraldOS Kernel */

const char *e820_type_name(enum e820_type type);
void print_e820_table(const struct e820_table *table);
void insert_e820_entry(struct e820_table *table, u64 addr, u64 size, enum e820_type type);
void sanitize_e820_table(struct e820_table *table);
#endif