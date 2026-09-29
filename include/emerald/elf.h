#ifndef EMERALD_ELF_H
#define EMERALD_ELF_H

#include <emerald/types.h>

/* Layout per Xinuos gABI, ELF Object File Format v4.3 */   

/* 32-bit ELF base types */
typedef u32 Elf32_Addr;
typedef u32 Elf32_Off;
typedef u16 Elf32_Half;
typedef u32 Elf32_Word;
typedef s32 Elf32_Sword;

/* 64-bit ELF base types */
typedef u64 Elf64_Addr;
typedef u64 Elf64_Off;
typedef u16 Elf64_Half;
typedef u32 Elf64_Word;
typedef s32 Elf64_Sword;
typedef u64 Elf64_Xword;
typedef s64 Elf64_Sxword;

/* Relative Relocation table entries */
typedef Elf32_Word  Elf32_Relr;
typedef Elf64_Xword Elf64_Relr;

/* Symbol Table Indexes */
#define STN_UNDEF 0

/* Symbol Binding (ELF32_ST_BIND) */
#define STB_LOCAL 	0
#define STB_GLOBAL 	1
#define STB_WEAK 	2
#define STB_LOOS	10
#define STB_HIOS	12
#define STB_LOPROC 	13
#define STB_HIPROC 	15

/* Symbol types */
#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_COMMON	5
#define STT_TLS		6
#define STT_LOOS	10
#define STT_HIOS	12
#define STT_LOPROC 	13
#define STT_HIPROC 	15

/* Symbol Visibility */
#define STV_DEFAULT 	0
#define STV_INTERNAL 	1
#define STV_HIDDEN 	2
#define STV_PROTECTED 	3
#define STV_EXPORTED 	4
#define STV_SINGLETON 	5
#define STV_ELIMINATE 	6

/* used to manipulate st_info */
#define ELF_ST_BIND(x)	 	((x) >> 4)
#define ELF_ST_TYPE(x) 	 	((x) & 0xF)
#define ELF_ST_VISIBILITY(x) 	((x) & 0x7)

#define ELF32_ST_BIND(x) 	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x) 	ELF_ST_TYPE(x)
#define ELF32_ST_VISIBILITY(x) 	ELF_ST_VISIBILITY(x)
#define ELF64_ST_BIND(x) 	ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x) 	ELF_ST_TYPE(x)
#define ELF64_ST_VISIBILITY(x) 	ELF_ST_VISIBILITY(x)

/* used in relocation */
#define ELF32_R_SYM(x)  ((x) >> 8)
#define ELF32_R_TYPE(x) ((unsigned char)(x))

#define ELF64_R_SYM(x) ((x) >> 32)
#define ELF64_R_TYPE(x) ((x) & 0xffffffffL)

typedef struct elf32_rel {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct elf64_rel {
	Elf64_Addr r_offset;	/* Location at which to apply the action */
	Elf64_Xword r_info;	/* Index and type of relocation */
} Elf64_Rel;

typedef struct elf32_rela {
 	Elf32_Addr r_offset;
 	Elf32_Word r_info;
 	Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct elf64_rela {
 	Elf64_Addr r_offset;	/* Location at which to apply the action */
 	Elf64_Xword r_info;	/* Index and type of relocation */
 	Elf64_Sxword r_addend;	/* Constant addend used to compute value */
} Elf64_Rela;

typedef struct elf32_sym {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct elf64_sym {
	Elf64_Word st_name;	/* Symbol name, index in string tbl */
	unsigned char st_info;	/* Type and binding attributes */
	unsigned char st_other;	/* Associated symbol's visibility */
	Elf64_Half st_shndx;	/* Associated section index */
	Elf64_Addr st_value;	/* Associated symbol's value */
	Elf64_Xword st_size;	/* Associated symbol size */
} Elf64_Sym;

/* These constants represent the different elf file types */
#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4
#define ET_LOOS		0xfe00
#define ET_HIOS		0xfeff
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
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
	unsigned char e_ident[EI_NIDENT]; /* ELF Magic Number */
	Elf64_Half e_type;
	Elf64_Half e_machine;	/* Target Architecture */
	Elf64_Word e_version;
	Elf64_Addr e_entry;	/* entry point virtual address */
	Elf64_Off e_phoff;	/* Program Header Table file offset */
	Elf64_Off e_shoff;	/* Section Header Table file offset */
	Elf64_Word e_flags;
	Elf64_Half e_ehsize;
	Elf64_Half e_phentsize;	/* Size of Program Header Table entries */
	Elf64_Half e_phnum;	/* Number of entries in Program Header Table */
	Elf64_Half e_shentsize;	/* Size of Section Header Table entries */
	Elf64_Half e_shnum;	/* Number of entries in Section Header Table */
	Elf64_Half e_shstrndx;
} Elf64_Ehdr;

/* Segment Types (p_type) */
#define PT_NULL 	0
#define PT_LOAD 	1
#define PT_DYNAMIC 	2
#define PT_INTERP 	3
#define PT_NOTE 	4
#define PT_SHLIB 	5
#define PT_PHDR 	6
#define PT_TLS		7
#define PT_LOOS		0x60000000
#define PT_HIOS		0x6fffffff
#define PT_LOPROC 	0x70000000
#define PT_HIPROC 	0x7fffffff

/* Segment Permissions */
#define PF_R 	0x4
#define PF_W	0x2
#define PF_X	0x1

/* Segment Flag bits, p_flags */
#define PF_MASKOS   0x0ff00000
#define PF_MASKPROC 0xf0000000

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

typedef struct {
	Elf64_Word p_type;	/* Type of segment */
	Elf64_Word p_flags;	/**/
	Elf64_Off p_offset;	/* Segment file offset */
	Elf64_Addr p_vaddr;	/* Segment virtual address */
	Elf64_Addr p_paddr;	/* Segment physical address */
	Elf64_Xword p_filesz;	/* Segment size in file */
	Elf64_Xword p_memsz;	/* Segment size in memory */
	Elf64_Xword p_align;	/* Segment alignment in file and memory */
} Elf64_Phdr;

/* sh_type */
#define SHT_NULL 		0
#define SHT_PROGBITS 		1
#define SHT_SYMTAB 		2
#define SHT_STRTAB 		3
#define SHT_RELA 		4
#define SHT_HASH 		5
#define SHT_DYNAMIC 		6
#define SHT_NOTE 		7
#define SHT_NOBITS 		8
#define SHT_REL 		9
#define SHT_SHLIB 		10
#define SHT_DYNSYM 		11
#define SHT_INIT_ARRAY 		14
#define SHT_FINI_ARRAY 		15
#define SHT_PREINIT_ARRAY 	16
#define SHT_GROUP 		17
#define SHT_SYMTAB_SHNDX 	18
#define SHT_RELR 		19
#define SHT_LOOS 		0x60000000
#define SHT_HIOS 		0x6fffffff
#define SHT_LOPROC 		0x70000000
#define SHT_HIPROC 		0x7fffffff
#define SHT_LOUSER 		0x80000000
#define SHT_HIUSER 		0xffffffff

/* sh_flags */
#define SHF_WRITE 		0x1
#define SHF_ALLOC 		0x2
#define SHF_EXECINSTR 		0x4
#define SHF_MERGE 		0x10
#define SHF_STRINGS 		0x20
#define SHF_INFO_LINK 		0x40
#define SHF_LINK_ORDER 		0x80
#define SHF_OS_NONCONFORMING 	0x100
#define SHF_GROUP 		0x200
#define SHF_TLS 		0x400
#define SHF_COMPRESSED 		0x800
#define SHF_MASKOS 		0x0ff00000
#define SHF_MASKPROC 		0xf0000000

/* Special Section Indexes */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_LOOS	0xff20
#define SHN_HIOS	0xff3f
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_XINDEX	0xffff
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

typedef struct elf64_shdr {
	Elf64_Word sh_name;		/* Section name; idx into string table */
	Elf64_Word sh_type;		/* Section type */
	Elf64_Xword sh_flags;		/* Section flags */
	Elf64_Addr sh_addr;		/* Section virtual address at execution */
	Elf64_Off sh_offset;		/* Section file offset */
	Elf64_Xword sh_size;		/* Size of section in bytes */
	Elf64_Word sh_link;		/* Index of another section */
	Elf64_Word sh_info;		/* Additional Section info */
	Elf64_Xword sh_addralign;	/* Section alignment */
	Elf64_Xword sh_entsize;		/* Entry size if section holds table */
} Elf64_Shdr;

/* e_ident[] identification indexes */
#define EI_MAG0 	0
#define EI_MAG1 	1
#define EI_MAG2 	2
#define EI_MAG3 	3
#define EI_CLASS 	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_OSABI	7
#define EI_ABIVERSION	8
#define EI_PAD		9
#define EI_NIDENT	16

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