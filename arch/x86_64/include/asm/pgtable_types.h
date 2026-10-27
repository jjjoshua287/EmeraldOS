#ifndef ASM_X86_PGTABLE_TYPES_H
#define ASM_X86_PGTABLE_TYPES_H

#include <asm/pgtable64_types.h>

#define _PAGE_BIT_PRESENT	0	/* is Present */
#define _PAGE_BIT_RW		1	/* writable */
#define _PAGE_BIT_USER		2	/* userspace addressable */
#define _PAGE_BIT_PWT		3	/* page write-through */
#define _PAGE_BIT_PCD		4	/* page cache disabled */
#define _PAGE_BIT_ACCESSED	5	/* was accessed (set by CPU) */
#define _PAGE_BIT_DIRTY		6	/* was written to (set by CPU) */
#define _PAGE_BIT_PS		7	/* 2 MB Huge Pages */
#define _PAGE_BIT_PAT		7	/* On 4 KB Pages */
#define _PAGE_BIT_GLOBAL	8	/* Global TLB entry */

#define _PAGE_BIT_AVL0		9	/* Available for kernel */
#define _PAGE_BIT_AVL1		10
#define _PAGE_BIT_AVL2		11

#define _PAGE_BIT_PAT_LARGE	12	/* On 2 MB or 1 GB Pages */

#define _PAGE_BIT_AVL3		52	/* Available for kernel */
#define _PAGE_BIT_AVL4		53
#define _PAGE_BIT_AVL5		54
#define _PAGE_BIT_AVL6		55
#define _PAGE_BIT_AVL7		56
#define _PAGE_BIT_AVL8		57
#define _PAGE_BIT_AVL9		58

#define _PAGE_BIT_PROTKEY0	59	/* Protection Key: bit 1/4 */
#define _PAGE_BIT_PROTKEY1	60	/* Protection Key: bit 2/4 */
#define _PAGE_BIT_PROTKEY2	61	/* Protection Key: bit 3/4 */
#define _PAGE_BIT_PROTKEY3	62	/* Protection Key: bit 4/4 */

#define _PAGE_BIT_NX		63	/* No execute */

/* macro to ensure correct bitshifts */
#ifndef __ASSEMBLER__
#define _TYPE(T, X) ((T)(X))
#else
#define _TYPE(T, X) X
#endif

#define _BIT_PTE(bit) (_TYPE(pteval_t, 1) << bit)

#define _PAGE_PRESENT	_BIT_PTE(_PAGE_BIT_PRESENT)
#define _PAGE_RW	_BIT_PTE(_PAGE_BIT_RW)
#define _PAGE_USER	_BIT_PTE(_PAGE_BIT_USER)
#define _PAGE_PWT	_BIT_PTE(_PAGE_BIT_PWT)
#define _PAGE_PCD	_BIT_PTE(_PAGE_BIT_PCD)
#define _PAGE_ACCESSED	_BIT_PTE(_PAGE_BIT_ACCESSED)
#define _PAGE_DIRTY	_BIT_PTE(_PAGE_BIT_DIRTY)
#define _PAGE_PS	_BIT_PTE(_PAGE_BIT_PS)
#define _PAGE_GLOBAL	_BIT_PTE(_PAGE_BIT_GLOBAL)

#define _PAGE_PAT	_BIT_PTE(_PAGE_BIT_PAT)
#define _PAGE_PAT_LARGE	_BIT_PTE(_PAGE_BIT_PAT_LARGE)

#define _PAGE_AVL0	_BIT_PTE(_PAGE_BIT_AVL0)
#define _PAGE_AVL1	_BIT_PTE(_PAGE_BIT_AVL1)
#define _PAGE_AVL2	_BIT_PTE(_PAGE_BIT_AVL2)
#define _PAGE_AVL3	_BIT_PTE(_PAGE_BIT_AVL3)
#define _PAGE_AVL4	_BIT_PTE(_PAGE_BIT_AVL4)
#define _PAGE_AVL5	_BIT_PTE(_PAGE_BIT_AVL5)
#define _PAGE_AVL6	_BIT_PTE(_PAGE_BIT_AVL6)
#define _PAGE_AVL7	_BIT_PTE(_PAGE_BIT_AVL7)
#define _PAGE_AVL8	_BIT_PTE(_PAGE_BIT_AVL8)
#define _PAGE_AVL9	_BIT_PTE(_PAGE_BIT_AVL9)

#define _PAGE_PROTKEY0	_TYPE(pteval_t, 0)
#define _PAGE_PROTKEY1	_TYPE(pteval_t, 0)
#define _PAGE_PROTKEY2	_TYPE(pteval_t, 0)
#define _PAGE_PROTKEY3	_TYPE(pteval_t, 0)

#define _PAGE_PROTKEY_MASK (_PAGE_PROTKEY0 | _PAGE_PROTKEY1 | \
			    _PAGE_PROTKEY2 | _PAGE_PROTKEY3)

#define _PAGE_NX	_BIT_PTE(_PAGE_BIT_NX)

#define _PAGE_NOCACHE (_PAGE_PCD | _PAGE_PWT)

/* boot-time tables: no NX, they must work before EFER.NXE is set */
#define __EARLY_PAGE_KERNEL 	(_PAGE_PRESENT | _PAGE_RW)
#define __EARLY_PAGE_KERNEL_BIG	(__EARLY_PAGE_KERNEL | _PAGE_PS)
#define __EARLY_PAGE_KERNEL_IO	(__EARLY_PAGE_KERNEL_BIG | _PAGE_NOCACHE | _PAGE_ACCESSED)

/* shortened names for page bits */
#define __PP _PAGE_PRESENT
#define __RW _PAGE_RW
#define _USR _PAGE_USER
#define ___A _PAGE_ACCESSED
#define ___D _PAGE_DIRTY
#define ___G _PAGE_GLOBAL
#define __NX _PAGE_NX

#define __NC _PAGE_NOCACHE
#define __PS _PAGE_PS

#ifndef __ASSEMBLER__
typedef struct { pteval_t pgprot; } pgprot_t;
#endif

#define pgprot_val(x) ((x).pgprot)
#define __pgprot(x) ((pgprot_t){ x })
#define __pg(x) __pgprot(x)

#define PAGE_NONE 		__pg(   0 |    0 |    0 | ___A |    0 |    0 |    0 | ___G)
#define PAGE_SHARED		__pg(__PP | __RW | _USR | ___A | __NX |    0 |    0 |    0)
#define PAGE_SHARED_EXEC	__pg(__PP | __RW | _USR | ___A |    0 |    0 |    0 |    0)
#define PAGE_COPY_NOEXEC	__pg(__PP |    0 | _USR | ___A | __NX |    0 |    0 |    0)
#define PAGE_COPY_EXEC		__pg(__PP |    0 | _USR | ___A |    0 |    0 |    0 |    0)
#define PAGE_READONLY		__pg(__PP |    0 | _USR | ___A | __NX |    0 |    0 |    0)
#define PAGE_READONLY_EXEC	__pg(__PP |    0 | _USR | ___A |    0 |    0 |    0 |    0)

#define PAGE_KERNEL_RO		__pg(__PP |    0 |    0 | ___A | __NX |    0 |    0 | ___G)
#define PAGE_KERNEL_ROX		__pg(__PP |    0 |    0 | ___A |    0 |    0 |    0 | ___G)
#define PAGE_KERNEL		__pg(__PP | __RW |    0 | ___A | __NX | ___D |    0 | ___G)
#define PAGE_KERNEL_EXEC	__pg(__PP | __RW |    0 | ___A |    0 | ___D |    0 | ___G)
#define PAGE_KERNEL_NOCACHE	__pg(__PP | __RW |    0 | ___A | __NX | ___D |    0 | ___G | __NC)
#define PAGE_KERNEL_LARGE	__pg(__PP | __RW |    0 | ___A | __NX | ___D | __PS | ___G)
#define PAGE_KERNEL_LARGE_EXEC	__pg(__PP | __RW |    0 | ___A |    0 | ___D | __PS | ___G)

#define PAGE_KERNEL_IO 		PAGE_KERNEL
#define PAGE_KERNEL_IO_NOCACHE	PAGE_KERNEL_NOCACHE

#endif	/* ASM_X86_PGTABLE_TYPES_H */