#include <asm/e820/types.h>
#include <emerald/types.h>
#include <emerald/runtime.h>
#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/compiler.h>
#include <emerald/memblock.h>

static struct e820_table e820_table_init;
struct e820_table *e820_table = &e820_table_init;

const char *e820_type_name(enum e820_type type)
{
	switch (type) {
	case E820_TYPE_RAM:
		return "System RAM";
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

void e820_print_table()
{
	u64 end_prev = 0;

	for (u32 i = 0; i < e820_table->nr_entries; i++) {
		const struct e820_entry *entry = &e820_table->entries[i];
		u64 start = entry->addr;
		u64 end = entry->addr + entry->size;

		/* Out of order E820 maps shouldn't happen */
		if (unlikely(start < end_prev))
			printk("Out of order E820 entry!\n");

		if (entry->addr > end_prev)
			printk("e820: [gap 0x%016llx-0x%016llx]\n", end_prev, start - 1);

	    	printk("e820: [mem 0x%016llx-0x%016llx] %s\n", start, end - 1, e820_type_name(entry->type));
		
		end_prev = entry->addr + entry->size;
	}
}

/* Merge adjacent regions */
void e820_sanitize_table() 
{
	u32 w = 0;	/* index of the last kept entry */
	/* If we only have 1 entry, don't sort it. */
	if (e820_table->nr_entries < 2)
		return;

	for (u32 r = 1; r < e820_table->nr_entries; r++) {
		struct e820_entry *last = &e820_table->entries[w];
		struct e820_entry *curr = &e820_table->entries[r];
		u64 last_end = last->addr + last->size;

		/* Check if last and curr should be merged */
		if (curr->type == last->type && curr->addr <= last_end) {
			/* last and curr are adjacent or overlapping */
			u64 curr_end = curr->addr + curr->size;
			if (curr_end > last_end)
				last->size = curr_end - last->addr;
		} else {
			e820_table->entries[++w] = *curr;
		}
	}

	size_t size = (e820_table->nr_entries - (w + 1)) * sizeof(struct e820_entry);
	memset(&e820_table->entries[w + 1], 0, size);
	e820_table->nr_entries = w + 1;
}

void e820_memory_setup(struct e820_table *table)
{
	/* The e820 in struct boot_params lives in the EFI Stub, so make a copy
	 * for the kernel so we don't need to pass it between functions.
	 */
	memcpy(e820_table, table, sizeof(struct e820_table));
	e820_sanitize_table();

	printk("Firmware-provided physical RAM map:\n");
	e820_print_table();
}

/* handover the e820 memory map to memblock */
int e820_memblock_setup()
{
	int ret = 0;

	for (u32 i = 0; i < e820_table->nr_entries; i++) {
		struct e820_entry *entry = &e820_table->entries[i];
		if (entry->type == E820_TYPE_RAM)
			ret |= memblock_add(entry->addr, entry->size);
		else if (entry->type != E820_TYPE_UNUSABLE)
			ret |= memblock_reserve(entry->addr, entry->size);
	}

	/* throw away partial pages */
	memblock_trim_memory(PAGE_SIZE);

	memblock_dump_all();
	return ret;
}