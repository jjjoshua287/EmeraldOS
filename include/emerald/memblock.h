#ifndef EMERALD_MEMBLOCK_H
#define EMERALD_MEMBLOCK_H

#include <emerald/types.h>
#include <emerald/compiler.h>

/* the initial number of memblock regions, can be increased dynamically */
#define MEMBLOCK_INIT_REGIONS 128

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
extern bool memblock_debug;

#define memblock_dbg(fmt, ...) 				\
	do {						\
		if (memblock_debug)			\
			printk(fmt, ##__VA_ARGS__);	\
	} while (0)

/* flags for memblock alloc limits */
#define MEMBLOCK_ALLOC_ANYWHERE   (~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE 0	/* use memblock.curr_limit */

int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);

int __memblock_reserve(phys_addr_t base, phys_addr_t size, enum memblock_flags flags);

/**
 * memblock_reserve - add new reserved memblock region
 * @base: physical base address of the new region
 * @size: size of the new region
 * 
 * Add new memblock region [@base, @base + @size) to the "reserved" type.
 * See memblock_add_range() for more details.
 * 
 * Returns:
 * 0 on success, a negative number on failure.
 */
static __always_inline int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	return __memblock_reserve(base, size, 0);
}

/* add new memblock region reserved for kernel use */
static __always_inline int memblock_reserve_kern(phys_addr_t base, phys_addr_t size)
{
	return __memblock_reserve(base, size, MEMBLOCK_RSVD_KERN);
}

void memblock_dump_all(void);
void test_memblock_visual(void);

/**
 * for_each_memblock_type - iterator for memblock_type->regions[]
 * @i: loop variable, set to 0
 * @memblock_type: pointer to memblock_type to iterate over
 * @rgn: pointer to memblock_region
 */
#define for_each_memblock_type(i, memblock_type, rgn)		\
	for ((i) = 0, (rgn) = &memblock_type->regions[0];	\
		(i) < memblock_type->count;			\
		(i)++, (rgn) = &memblock_type->regions[(i)])

#endif