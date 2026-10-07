#include <asm/e820/types.h>
#include <emerald/runtime.h>
#include <emerald/string.h>
#include <emerald/printk.h>

const char *e820_type_name(enum e820_type type)
{
	switch (type) {
	case E820_TYPE_RAM:
		return "Usable";
	case E820_TYPE_RESERVED:
		return "Reserved";
	case E820_TYPE_ACPI:
		return "ACPI data";
	case E820_TYPE_NVS:
		return "ACPI NVS";
	case E820_TYPE_UNUSABLE:
		return "Unusable";
	case E820_TYPE_PMEM:
		return "Persistant";
	default:
		return "A riddle wrapped in a mystery inside an enigma";
	}
}

void print_e820_table(const struct e820_table *table)
{
	for (u32 i = 0; i < table->nr_entries; i++) {
		const struct e820_entry *ent = &table->entries[i];
	    	printk("e820: [mem %016llx-%016llx] %s\n", 
			ent->addr,  ent->addr + ent->size - 1, e820_type_name(ent->type));
	}
}

/* Merge adjacent regions */
void sanitize_e820_table(struct e820_table *table) {
	struct e820_entry *last = &table->entries[0];
	for (int i = 1; i < table->nr_entries; i++) {
		struct e820_entry *curr = &table->entries[i];
		if (curr->type != last->type) {
			last = curr;
			continue;
		}

		last->size += curr->size;
		last = curr;

		/* we can't shift entries left if we're at the last entry */
		if ((i + 1) == table->nr_entries)
			memset(curr, 0, sizeof(struct e820_entry));
		else 
			memmove(curr, curr + 1, (table->nr_entries - i) * sizeof(struct e820_entry));
		table->nr_entries--;
	}
}