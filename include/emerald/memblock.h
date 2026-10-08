#ifndef EMERALD_MEMBLOCK_H
#define EMERALD_MEMBLOCK_H

#include <emerald/types.h>

/** 
  * enum memblock_flags - various flags for memblock regions
  * @MEMBLOCK_NONE: no special request
  * @MEMBLOCK_NOMAP: don't add to kernel mapping; treat this region as reserved
  * @MEMBLOCK_DRIVER_MANAGED: memory region a driver detects and adds. This region 
  * might not be identified as System RAM in the firmware's memory map.
  * @MEMBLOCK_RSVD_KERN: memory region that is reserved for kernel use. All memblock
  * allocations set this flag.
  */
enum memblock_flags {
	MEMBLOCK_NONE 		= 0x0,	/* no special request */
	MEMBLOCK_NOMAP 		= 0x1,	/* don't add to kernel mapping */
	MEMBLOCK_DRIVER_MANAGED	= 0x2,	/* detected by a driver */
	MEMBLOCK_RSVD_KERN	= 0x4,	/* reserved for kernel use */
};

/**
 * struct memblock_region - represents a memory region
 * @base: start address
 * @size: size of memory region
 * @flags: attributes of memory region
 */
struct memblock_region {
    phys_addr_t base;
    phys_addr_t size;
    enum memblock_flags flags;
};

/**
 * struct memblock_type - a collection of memory regions of a certain type
 * @count: number of regions
 * @max: size of the allocated array
 * @total_size: size of all regions
 * @regions: dynamic array of regions
 * @name: symbolic name of memory type
 */
struct memblock_type {
	unsigned long count;
	unsigned long max;
	phys_addr_t total_size;
	struct memblock_region *regions;
	const char *name;
};

/**
 * struct memblock - metadata for memblock allocator.
 * @curr_limit: physical address of current allocation limit
 * @memory: usable memory regions
 * @reserved: reserved memory regions
 */
struct memblock {
	phys_addr_t curr_limit;
	struct memblock_type memory;
	struct memblock_type reserved;
};

extern struct memblock memblock;

/* constants for memblock API */

/* flags for memblock alloc limits */
#define MEMBLOCK_ALLOC_ANYWHERE   (~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE 0	/* use memblock.curr_limit */

/* the initial number of memblock regions, can be increased dynamically */
#define MEMBLOCK_INIT_SECTIONS 128

#endif