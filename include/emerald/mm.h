#ifndef EMERALD_MEMORY_MANAGEMENT_H
#define EMERALD_MEMORY_MANAGEMENT_H

#include <asm/page.h>

/* 
 * convert between pages and MB.
 * 20 is the shift for 1MB (2^20 = 1MB) 
 */
#define PAGES_TO_MB(pages)  ((pages >> (20 - PAGE_SHIFT)))
#define MB_TO_PAGES(mb)     ((mb) << (20 - PAGE_SHIFT))

/* align a pointer to the next page boundary */
#define PAGE_ALIGN(addr) ALIGN((addr), PAGE_SIZE)

/* align a pointer to the previous page boundary */
#define PAGE_ALIGN_DOWN(addr) ALIGN_DOWN((addr), PAGE_SIZE)

/* test whether an address (unsigned long or pointer) is aligned to PAGE_SIZE */
#define PAGE_ALIGNED(addr) IS_ALIGNED((unsigned long)(addr), PAGE_SIZE)

#endif	/* EMERALD_MEMORY_MANAGEMENT_H */