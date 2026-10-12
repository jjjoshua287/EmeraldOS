#include <emerald/types.h>
#include <emerald/limits.h>
#include <emerald/minmax.h>
#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/memblock.h>

#include <asm/bug.h>

static struct memblock_region memblock_mem_init_regions[MEMBLOCK_INIT_REGIONS];
static struct memblock_region memblock_rerserved_init_regions[MEMBLOCK_INIT_REGIONS];

struct memblock memblock = {
	.memory.regions = memblock_mem_init_regions,
	.memory.max = MEMBLOCK_INIT_REGIONS,
	.memory.name = "memory",
	

	.reserved.regions = memblock_rerserved_init_regions,
	.reserved.max = MEMBLOCK_INIT_REGIONS,
	.reserved.name = "reserved",

	.curr_limit = MEMBLOCK_ALLOC_ANYWHERE,
};

bool memblock_debug = true;

static inline const char *memblock_type_name(struct memblock_type *type)
{
	if (type == &memblock.memory)
		return "memory";
	else if (type == &memblock.reserved)
		return "reserved";
	else
		return "unknown";
}

/* adjust *@size so that @base + *@size doesn't overflow. returns the new size */
static inline phys_addr_t memblock_cap_size(phys_addr_t base, phys_addr_t *size)
{
	return *size = min(*size, (phys_addr_t)ULLONG_MAX - base);
}

/**
 * memblock_insert_region - insert new memblock region
 * @type: memblock type to insert into
 * @idx: index for insertion point
 * @base: physical base address of the new region
 * @size: size of the new region
 * @flags: flags of the new region
 * 
 * Insert new memblock region [@base, @base + @size) into @type at @idx.
 * @type must already have extra space for the new region
 */
static void memblock_insert_region(struct memblock_type *type, int idx, phys_addr_t base, 
			   phys_addr_t size, enum memblock_flags flags)
{
	struct memblock_region *rgn = &type->regions[idx];

	BUG_ON(type->count >= type->max);
	memmove(rgn + 1, rgn, (type->count - idx) * sizeof(*rgn));
	rgn->base = base;
	rgn->size = size;
	rgn->flags = flags;

	type->count++;
	type->total_size += size;
}

/**
 * memblock_add_range - add new memblock region
 * @type: memblock type to add new region to
 * @base: physical base address of the new region
 * @size: size of the new region
 * @flags: flags of the new region
 * 
 * Add new memblock region [@base, @base + @size) into @type.
 * all neighbouring compatible regions in @type are merged after the addition.
 * 
 * Returns:
 * 0 on success, a negative number on failure.
 */
int memblock_add_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size, 
		       enum memblock_flags flags)
{
	int idx;
	bool insert = false;
	struct memblock_region *rgn;
	phys_addr_t end = base + memblock_cap_size(base, &size);

	if (!size)
		return 0;

	/* special case for empty array */
	if (type->regions[0].size == 0) {
		WARN_ON(type->count != 0 || type->total_size);
		type->regions[0].base = base;
		type->regions[0].size = size;
		type->regions[0].flags = flags;
		type->total_size = size;
		type->count = 1;
		return 0;
	}

repeat:
	/*
	 * The following is executed twice. The first time with %false @insert
	 * to count the number of regions needed for the range and the second
	 * time with %true @insert to actually insert them.
	 */
	int nr_new = 0;

	for_each_memblock_type(idx, type, rgn) {
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rgn->base + rgn->size;
		if (rbase >= end)
			break;
		if (rend <= base)
			continue;
		/*
		 * @rgn overlaps. If it separates the lower portion of the new
		 * area, insert that portion
		 */
		if (rbase > base) {
			WARN_ON(flags != rgn->flags);
			nr_new++;
			if (insert)
				memblock_insert_region(type, idx++, base, rbase - base, flags);
		}
		/* the area before @rend has been resolved. */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert)
			memblock_insert_region(type, idx, base, end - base, flags);
	}

	if (!nr_new)
		return 0;

	if (!insert) {
		/* resize the array if we don't have enough space */
		if (type->count + nr_new > type->max) {
			/* warn for debugging until I actually handle the error */
			WARN();
			return -1;
		}

		insert = true;
		goto repeat;
	} else {
		// merge the regions
		return 0;
	}
}

/**
 * memblock_add - add new memblock region
 * @base: physical base address of the new region
 * @size: size of the new region
 * 
 * Add new memblock region [@base, @base + @size) to the "memory" type.
 * See memblock_add_range() for more details.
 * 
 * Returns:
 * 0 on success, a negative number on failure.
 */
int memblock_add(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *type = &memblock.memory;
	phys_addr_t end = base + size - 1;
	
	memblock_dbg("memblock_add: [0x%016llx-0x%016llx]\n", base, end);

	return memblock_add_range(type, base, size, 0);
}

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
int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *type = &memblock.reserved;
	phys_addr_t end = base + size - 1;

	memblock_dbg("memblock_reserve: [0x%016llx-0x%016llx]\n", base, end);

	return memblock_add_range(type, base, size, 0);
}