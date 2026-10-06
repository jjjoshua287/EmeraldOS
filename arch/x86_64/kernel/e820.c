#include <asm/e820.h>
#include <emerald/efi.h>
#include <emerald/runtime.h>

struct e820_table convert_efi_mmap(struct hw_memory_map *mmap)
{
        struct e820_table table;
        u64 uefi_entries = mmap->size / mmap->descriptorSize;
        table.nr_entries += (uefi_entries < E820_MAX_ENTRIES) ? uefi_entries : E820_MAX_ENTRIES;
        
        efi_memory_descriptor *map = mmap->memoryMap;
        for (u64 i = 0; i < uefi_entries && i < E820_MAX_ENTRIES; i++) {
                struct e820_entry *entry = &table.entries[i];
                efi_memory_descriptor *desc = (u8*)map + i * mmap->descriptorSize;
                entry->addr = desc->PhysicalStart;
                entry->size = desc->NumberOfPages * EFI_PAGE_SIZE;
                
                switch (desc->Type) {
                case EfiLoaderCode:
                case EfiLoaderData:
                case EfiBootServicesCode:
                case EfiBootServicesData:
                case EfiConventionalMemory:
                        if (desc->Attribute & EFI_MEMORY_WB)
                                entry->type = E820_TYPE_RAM;
                        else
                                entry->type = E820_TYPE_RESERVED;
                        break;
                case EfiACPIReclaimMemory:
                        entry->type = E820_TYPE_ACPI;
                        break;
                case EfiACPIMemoryNVS:
                        entry->type = E820_TYPE_NVS;
                        break;
                case EfiPersistentMemory:
                        entry->type = E820_TYPE_PMEM;
                        break;
                case EfiUnusableMemory:
                        entry->type = E820_TYPE_UNUSABLE;
                        break;
                default:
                        entry->type = E820_TYPE_RESERVED;
                        break;
                }
        }
        return table;
}