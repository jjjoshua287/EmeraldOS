#ifndef EMERALD_ELF_H
#define EMERALD_ELF_H

#include <emerald/types.h>

/* This header follows the: Tool Interface Standard (TIS) 
 * Executable and Linking Format (ELF) Specification v1.2
 */

/* 32-bit ELF base types */
typedef u32 Elf32_Addr;
typedef u16 Elf32_Half;
typedef u32 Elf32_Off;
typedef u32 Elf32_Sword;
typedef u32 Elf32_Word;

/* Symbol Table Indexes */
#define STN_UNDEF 0

/* Symbol Binding (ELF32_ST_BIND) */
#define STB_LOCAL 	0
#define STB_GLOBAL 	1
#define STB_WEAK 	2
#define STB_LOPROC 	13
#define STB_HIPROC 	15

/* Symbol types */
#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_LOPROC 	13
#define STT_HIPROC 	15

/* used to manipulate st_info */
#define ELF_ST_BIND(x)	 ((x) >> 4)
#define ELF_ST_TYPE(x) 	 ((x) & 0xF)
#define ELF32_ST_BIND(x) ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x) ELF_ST_TYPE(x)

/* used in relocation */
#define ELF32_R_SYM(x)  ((i) >> 8)
#define ELF32_R_TYPE(x) ((unsigned char)(i))

typedef struct elf32_rel {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct elf32_rela {
 	Elf32_Addr r_offset;
 	Elf32_Word r_info;
 	Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct elf32_sym {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

/* These constants represent the different elf file types */
#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4
#define ET_LOPROC 	0xff00
#define ET_HIPROC	0xffff

/* Values for e_version field */
#define EV_NONE		0
#define EV_CURRENT	1

#define EI_NIDENT 16

typedef struct elf32_hdr {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;	/* entry point */
	Elf32_Off e_phoff;	/* Program Header table offset */
	Elf32_Off e_shoff;	/* Section header table offset */
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

/* Segment Types (p_type) */
#define PT_NULL 	0
#define PT_LOAD 	1
#define PT_DYNAMIC 	2
#define PT_INTERP 	3
#define PT_NOTE 	4
#define PT_SHLIB 	5
#define PT_PHDR 	6
#define PT_LOPROC 	0x70000000
#define PT_HIPROC 	0x7fffffff

/* Segment Permissions */
#define PF_R 	0x4
#define PF_W	0x2
#define PF_X	0x1

typedef struct elf32_phdr {
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

/* sh_type */
#define SHT_NULL 	0
#define SHT_PROGBITS 	1
#define SHT_SYMTAB 	2
#define SHT_STRTAB 	3
#define SHT_RELA 	4
#define SHT_HASH 	5
#define SHT_DYNAMIC 	6
#define SHT_NOTE 	7
#define SHT_NOBITS 	8
#define SHT_REL 	9
#define SHT_SHLIB 	10
#define SHT_DYNSYM 	11
#define SHT_LOPROC 	0x70000000
#define SHT_HIPROC 	0x7fffffff
#define SHT_LOUSER 	0x80000000
#define SHT_HIUSER 	0xffffffff

/* sh_flags */
#define SHF_WRITE 	0x1
#define SHF_ALLOC 	0x2
#define SHF_EXECINSTR 	0x4
#define SHF_MASKPROC 	0xf0000000

/* Special Section Indexes */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_HIRESERVE	0xffff

typedef struct elf32_shdr {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

/* e_ident[] identification indexes */
#define EI_MAG0 	0
#define EI_MAG1 	1
#define EI_MAG2 	2
#define EI_MAG3 	3
#define EI_CLASS 	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_PAD		7

/* Magic Number '0x7fELF' */
#define ELFMAG0	0x7f
#define ELFMAG1	'E'
#define ELFMAG2	'L'
#define ELFMAG3	'F'
#define ELFMAG	"\177ELF"

/* EI_CLASS */
#define ELFCLASSNONE	0
#define ELFCLASS32	1
#define ELFCLASS64	2

/* EI_DATA */
#define ELFDATANONE	0
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#endif // EMERALD_ELF_H