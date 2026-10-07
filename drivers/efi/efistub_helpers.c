#include <asm/e820/types.h>

#include <emerald/efi.h>
#include <emerald/elf.h>
#include <emerald/string.h>
#include <emerald/runtime.h>
#include <emerald/compiler.h>

#include "efistub.h"

extern const unsigned char kernel_elf_start[];

/* NOTE: This assumes the kernel is compiled as a 64-bit ELF */

/* Load a program into memory an ELF's Program Header Table */
static efi_status_t load_pht(efi_boot_services_t *gBS, Elf64_Off phoff, 
                        Elf64_Half ph_entry_size, Elf64_Half pht_num_entries)
{
        for (Elf64_Half i = 0; i < pht_num_entries; i++) {
                struct elf64_phdr *phdr = (struct elf64_phdr *)(kernel_elf_start + phoff + i * ph_entry_size);
                if (phdr->p_type != PT_LOAD)
                        continue;

                efi_phys_addr_t addr = phdr->p_paddr;
                efi_status_t status = gBS->AllocatePages(
                        AllocateAddress, EfiLoaderData,
                        EFI_SIZE_TO_PAGES(phdr->p_memsz), &addr);
                if (EFI_ERROR(status))
                        return status;

                memcpy((void *)phdr->p_paddr, kernel_elf_start + phdr->p_offset, phdr->p_filesz);
                memset((void *)phdr->p_paddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
        }
        return EFI_SUCCESS;
}

static bool elf_valid(struct elf64_hdr *hdr)
{
        /* validate magic number and class */
        if (unlikely(hdr == NULL || memcmp(hdr->e_ident, ELFMAG, SELFMAG)))
                return false;   // not an ELF file
        if (unlikely(hdr->e_ident[EI_CLASS] != ELFCLASS64))
                return false;   // invalid class
        return true;
}

void *load_kernel(struct efi_boot_services *gBS)
{
        struct elf64_hdr *hdr = (struct elf64_hdr *)kernel_elf_start;
        if (likely(elf_valid(hdr))) {
                if (EFI_ERROR(load_pht(gBS, hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum)))
                        return NULL;
                return (void *)hdr->e_entry;
        }
        return NULL;
}

void insert_e820_entry(struct e820_table *table, u64 addr, u64 size, enum e820_type type);

/* Convert a memory map obtained from UEFI's GetMemoryMap to an E820 Memory Map */
void efi_mmap_to_e820(const struct hw_memory_map *mmap, struct e820_table *out)
{
	memset(out, 0, sizeof(struct e820_table));
	
	u64 uefi_entries = mmap->size / mmap->descriptorSize;
	u64 num_entries = (uefi_entries < E820_MAX_ENTRIES) ? uefi_entries : E820_MAX_ENTRIES;

	u8 *map = (u8 *)mmap->memoryMap;
	for (u64 i = 0; i < num_entries; i++) {
		struct e820_entry *entry = &out->entries[i];
		efi_memory_descriptor *desc = (efi_memory_descriptor *)(map + i * mmap->descriptorSize);
		enum e820_type type;
		u64 size = (u64)desc->NumberOfPages * EFI_PAGE_SIZE;
		
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

		insert_e820_entry(out, desc->PhysicalStart, size, type);
	}
}