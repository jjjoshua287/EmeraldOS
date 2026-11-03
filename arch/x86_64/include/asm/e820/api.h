#ifndef ASM_E820_API_H
#define ASM_E820_API_H

#include <emerald/types.h>
#include <asm/e820/types.h>

extern struct e820_table *e820_table;

/* Function prototypes for e820 functions called by the EmeraldOS Kernel */
bool e820_mapped_any(u64 start, u64 end, enum e820_type type);

const char *e820_type_name(enum e820_type type);
void e820_print_table(void);
void e820_sanitize_table(void);
void e820_insert_entry(struct e820_table *table, u64 addr, u64 size, enum e820_type type);
void e820_memory_setup(struct e820_table *table);
int e820_memblock_setup(void);

unsigned long e820_max_pfn(void);
unsigned long e820_max_low_pfn(void);

#endif