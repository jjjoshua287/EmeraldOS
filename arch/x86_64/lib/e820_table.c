#include <emerald/types.h>
#include <emerald/string.h>

#include <asm/e820/types.h>

/**
  * insert_e820_entry: Insert an entry into an e820 memory map in ascending order.
  * @table: ptr to struct e820_table
  * @addr:  physical address
  * @size:  size of entry
  * @type:  entry type
  * 
  * - The entries are sorted ascendingly by @addr with insertion sort.
  */
void insert_e820_entry(struct e820_table *table, u64 addr, u64 size, enum e820_type type)
{
	if (size == 0)
		return;

	int idx = table->nr_entries;
	struct e820_entry *ent = &table->entries[idx];
	while (addr < ent->addr && --idx >= 0)
		ent = &table->entries[idx];
	
	/* shift existing entries 1 to the right */
	if (ent->addr) {
		memmove(ent + 1, ent, (table->nr_entries - idx) * sizeof(struct e820_entry));
	}

	ent->addr = addr;
	ent->size = size;
	ent->type = type;
	table->nr_entries++;
};