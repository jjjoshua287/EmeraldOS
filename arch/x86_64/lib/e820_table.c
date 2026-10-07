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
	if (size == 0 || table->nr_entries >= E820_MAX_ENTRIES)
		return;

	u32 idx = table->nr_entries;
	while (idx > 0 && table->entries[idx - 1].addr > addr) {
		table->entries[idx] = table->entries[idx - 1];
		idx--;
	}

	table->entries[idx].addr = addr;
	table->entries[idx].size = size;
	table->entries[idx].type = type;
	table->nr_entries++;
};