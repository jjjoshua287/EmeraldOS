#include <emerald/elf.h>
#include <emerald/string.h>
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
                
                efi_status_t status = gBS->AllocatePages(
                        AllocateAddress, EfiLoaderData,
                        EFI_SIZE_TO_PAGES(phdr->p_memsz),
                        (efi_phys_addr_t *)&phdr->p_paddr
                );
                if (EFI_ERROR(status))
                        return status;

                memcpy((void *)phdr->p_paddr, kernel_elf_start + phdr->p_offset, phdr->p_filesz);
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