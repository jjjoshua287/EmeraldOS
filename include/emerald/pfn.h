#ifndef EMERALD_PFN_H
#define EMERALD_PFN_H

/*
 * Macros for doing things with Page Frame Numbers (PFN)
 */

#include <asm/page.h>
#include <emerald/types.h>

#define PFN_ALIGN(x) 	(((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_SIZE)
#define PFN_UP(x)	(((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)

#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x)	((unsigned long)((x) >> PAGE_SHIFT))

#endif