#include <emerald/elf.h>
#include <emerald/string.h>
#include <emerald/compiler.h>

extern void *kernel_elf_start;

/* NOTE: This assumes the kernel is compiled as a 64-bit ELF */

/* Load a program into memory an ELF's Program Header Table */
static void load_pht(Elf64_Off phoff, Elf64_Half ph_entry_size, Elf64_Half pht_num_entries)
{
        for (Elf64_Half i = 0; i < pht_num_entries; i++) {
                struct elf64_phdr *phdr = kernel_elf_start + phoff + i * ph_entry_size;
                if (phdr->p_type == PT_LOAD)
                        memcpy((void *)phdr->p_paddr, kernel_elf_start + phdr->p_offset, phdr->p_filesz);
        }
}

static bool elf_valid(struct elf64_hdr *hdr)
{
        /* validate magic number and class */
        if (unlikely(hdr == NULL || memcmp(hdr->e_ident, ELFMAG, SELFMAG)))
                return false;   // not an ELF file
        if (unlikely(hdr->e_ident[EI_CLASS] != ELFCLASS64))
                return false;   // invalid class
        return (hdr->e_entry);
}

void *load_kernel(void)
{
        struct elf64_hdr *hdr = kernel_elf_start;
        if (likely(elf_valid(hdr))) {
                load_pht(hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum);
                return (void *)hdr->e_entry;
        }
        return NULL;
}