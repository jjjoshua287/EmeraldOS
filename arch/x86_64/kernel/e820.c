#include <asm/e820.h>
#include <emerald/efi.h>
#include <emerald/runtime.h>
#include <emerald/string.h>

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
	int idx = table->nr_entries;
	struct e820_entry *ent = &table->entries[idx];
	while (addr < ent->addr && --idx >= 0)
		ent = &table->entries[idx];
	
	/* shift existing entries 1 to the right */
	if (ent->addr != NULL) {
		memmove(ent + 1, ent, (table->nr_entries - idx) * sizeof(struct e820_entry));
	}

	ent->addr = addr;
	ent->size = size;
	ent->type = type;
	table->nr_entries++;
};

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

/* Convert a memory map obtained from UEFI's GetMemoryMap to an E820 Memory Map */
struct e820_table convert_efi_mmap(struct hw_memory_map *mmap)
{
	struct e820_table table = {0};
	u64 uefi_entries = mmap->size / mmap->descriptorSize;
	table.nr_entries += (uefi_entries < E820_MAX_ENTRIES) ? uefi_entries : E820_MAX_ENTRIES;

	efi_memory_descriptor *map = mmap->memoryMap;
	for (u64 i = 0; i < uefi_entries && i < E820_MAX_ENTRIES; i++) {
		struct e820_entry *entry = &table.entries[i];
		efi_memory_descriptor *desc = (u8*)map + i * mmap->descriptorSize;
		enum e820_type type;
		u64 size = desc->NumberOfPages * EFI_PAGE_SIZE;
		
		switch (desc->Type) {
		case EfiLoaderCode:
		case EfiLoaderData:
		case EfiBootServicesCode:
		case EfiBootServicesData:
		case EfiConventionalMemory:
			if (desc->Attribute & EFI_MEMORY_WB)
				type = E820_TYPE_RAM;
			else
				type = E820_TYPE_RESERVED;
			break;
		case EfiACPIReclaimMemory:
			type = E820_TYPE_ACPI;
			break;
		case EfiACPIMemoryNVS:
			type = E820_TYPE_NVS;
			break;
		case EfiPersistentMemory:
			type = E820_TYPE_PMEM;
			break;
		case EfiUnusableMemory:
			type = E820_TYPE_UNUSABLE;
			break;
		default:
			type = E820_TYPE_RESERVED;
			break;
		}

		insert_e820_entry(&table, desc->PhysicalStart, size, type);
	}

	sanitize_e820_table(&table);
	return table;
}