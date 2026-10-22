#include <emerald/types.h>
#include <emerald/limits.h>
#include <emerald/errno.h>
#include <emerald/minmax.h>
#include <emerald/align.h>
#include <emerald/string.h>
#include <emerald/printk.h>
#include <emerald/memblock.h>

#include <asm/bug.h>
#include <asm/pages.h>

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
 * memblock_double_array - double the size of a memblock_region array
 * @type: memblock_type of the regions array being doubled
 * @start_addr: start of memory range to avoid overlap with
 * @size: size of memory range to avoid overlap with
 * 
 * Returns:
 * 0 on success, -errno on failure
 */
static int memblock_double_array(struct memblock_type *type, 
				 phys_addr_t start_addr,
				 phys_addr_t size)
{
	return -ENOSYS; 	/* Not implemented yet */
}

/**
 * memblock_merge_regions - merge neighboring compatible regions
 * @type: memblock type to scan
 * @start_rgn: start scanning from (@start_rgn - 1)
 * @end_rgn: end scanning at (@end_rgn - 1)
 * 
 * Scan @type and merge neighboring compatible regions in [@start_rgn - 1, @end_rgn)
 */
static void memblock_merge_regions(struct memblock_type *type, 
				   unsigned long start_rgn, 
				   unsigned long end_rgn)
{
	int i = 0;
	if (start_rgn)
		i = start_rgn - 1;
	end_rgn = min(end_rgn, type->count - 1);

	while (i < end_rgn) {
		struct memblock_region *this = &type->regions[i];
		struct memblock_region *next = &type->regions[i + 1];
		if ((this->base + this->size != next->base) || 
		    (this->flags != next->flags)) {
			i++;
			BUG_ON(this->base + this->size > next->base);
			continue;
		}
		this->size += next->size;
		
		/* shift entries back by one */
		size_t bytes = (type->count - (i + 2)) * sizeof(struct memblock_region);
		memmove(next, next + 1, bytes);
		type->count--;
		end_rgn--;
	}
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
 * 0 on success, -errno on failure.
 */
int memblock_add_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size, 
		       enum memblock_flags flags)
{
	int idx, nr_new;
	int start_rgn = -1, end_rgn = 0;
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
	nr_new = 0;

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
			if (insert) {
				if (start_rgn == -1)
					start_rgn = idx;
				end_rgn = idx + 1;
				memblock_insert_region(type, idx++, base, rbase - base, flags);
			}
		}
		/* the area before @rend has been resolved. */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert) {
			if (start_rgn == -1)
				start_rgn = idx;
			end_rgn = idx + 1;
			memblock_insert_region(type, idx, base, end - base, flags);
		}
	}

	if (!nr_new)
		return 0;

	if (!insert) {
		/* resize the array if we don't have enough space */
		while (type->count + nr_new > type->max) {
			if (memblock_double_array(type, base, size) < 0)
				return -ENOMEM;
		}
		insert = true;
		goto repeat;
	} else {
		memblock_merge_regions(type, start_rgn, end_rgn);
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
 * 0 on success, -errno on failure.
 */
int memblock_add(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *type = &memblock.memory;
	phys_addr_t end = base + size - 1;
	
	memblock_dbg("memblock_add: [0x%016llx-0x%016llx]\n", base, end);

	return memblock_add_range(type, base, size, 0);
}

int __memblock_reserve(phys_addr_t base, phys_addr_t size, enum memblock_flags flags)
{
	struct memblock_type *type = &memblock.reserved;
	phys_addr_t end = base + size - 1;

	memblock_dbg("memblock_reserve: [0x%016llx-0x%016llx] flags = %x\n", 
		     base, end, flags);

	return memblock_add_range(type, base, size, flags);
}

/**
 * memblock_discard - remove a memory region from a memory type
 * @type: memory type of the victim region
 * @idx: index of the victim region
 * 
 * removes the region at type->regions[idx] and shifts elements down
 */
static void memblock_discard(struct memblock_type *type, int idx)
{
	struct memblock_region *rgn = &type->regions[idx];
	type->total_size -= rgn->size;
	memset(rgn, 0, sizeof(*rgn));

	if (type->count == 0)
		return;
	
	BUG_ON(type->count >= type->max);

	if (idx != type->count - 1)
		memmove(rgn, rgn + 1, (type->count - (idx + 1)) * sizeof(*rgn));
	type->count--;
}

/**
 * memblock_split_region - make a hole in a memblock region
 * @type: memblock type
 * @idx: index of victim region
 * @base: start address of range
 * @end: end address of range
 *
 * splits the region into: 
 * 	- [rbase, @base) at @idx
 * 	- [@end, rend) at @idx + 1
 * 
 * Returns:
 * 0 on success, -errno on failure
 */
static int memblock_split_region(struct memblock_type *type, int idx, 
				 phys_addr_t base, phys_addr_t end)
{
	struct memblock_region *rgn = &type->regions[idx];
        phys_addr_t rend = rgn->base + rgn->size;
	phys_addr_t rsize = rgn->size;

        if (type->count + 1 > type->max) {
                if (memblock_double_array(type, base, end - base) < 0)
                        return -ENOMEM;
                rgn = &type->regions[idx];
        }

        /* Shrink lower fragment and insert high fragment */
        rgn->size = base - rgn->base;
	type->total_size -= (rsize - rgn->size);
	memblock_insert_region(type, idx + 1, end, rend - end, rgn->flags);
        return 0;
}

/**
 * memblock_remove_range - remove new memblock region
 * @type: memblock type to remove region from
 * @base: physical base address of the victim region
 * @size: size of the victim region
 * 
 * Removes a memblock region [@base, @base + @size) from @type.
 * 
 * Returns:
 * 0 on success, -errno on failure.
 */
int memblock_remove_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size)
{
	int idx;
	struct memblock_region *rgn;
	phys_addr_t end = base + memblock_cap_size(base, &size);

	if (!size || type->regions[0].size == 0)
		return 0;
	
	for_each_memblock_type(idx, type, rgn) {
		phys_addr_t rbase = rgn->base;
		phys_addr_t rsize = rgn->size;
		phys_addr_t rend = rgn->base + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;

		/* @rgn is guarenteed to overlap */

		/* split @rgn if the range is inside @rgn */
		if (rbase < base && rend > end) {
			return memblock_split_region(type, idx, base, end);
		}

		/* range overlaps the right section */
		if (rbase < base) {
			rgn->size = base - rbase;
		} else if (rend <= end) {
			/* entire region inside range */
			memblock_discard(type, idx--);
			continue;
		} else {
			/* range overlaps the higher section */
			rgn->base = end;
			rgn->size = rend - end;
		}
		type->total_size -= (rsize - rgn->size);
	}
	return 0;
}

/**
 * memblock_remove - remove a memory region
 * @base: physical base address of the victim region
 * @size: size of the new region
 * 
 * Remove memblock region [@base, @base + @size) from the "memory" type.
 * See memblock_remove_range() for more details.
 * 
 * Returns:
 * 0 on success, -errno on failure.
 */
int memblock_remove(phys_addr_t base, phys_addr_t size)
{
	struct memblock_type *type = &memblock.memory;
	phys_addr_t end = base + size - 1;
	
	memblock_dbg("memblock_remove: [0x%016llx-0x%016llx]\n", base, end);

	return memblock_remove_range(type, base, size);
}

/**
 * memblock_find_free_area - find a contiguous area of free memory
 * @size: size of the area
 * @align: alignment boundary of the area
 * @start: start address
 * @end: end address
 * 
 * finds a contiguous area of memory in the 'memory' type that isn't reserved
 * and is aligned to @align within [start, end).
 * 
 * Returns:
 * A physical address of the found area on success, 0 on failure.
 */
static phys_addr_t memblock_find_free_area(phys_addr_t size, phys_addr_t align,
				   phys_addr_t start, phys_addr_t end)
{
	int idx;
	struct memblock_region *rgn;
	struct memblock_region *rsvd_rgn = &memblock.reserved.regions[0];
	struct memblock_type *mem = &memblock.memory;
	struct memblock_type *rsvd = &memblock.reserved;

	for_each_memblock_type(idx, mem, rgn) {
		if (idx + 1 >= rsvd->count)
			rsvd_rgn = &rsvd->regions[idx];

		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rgn->base + rgn->size;

		/* we can't use this region if it's reserved */
		if (rbase >= rsvd_rgn->base && rend <= (rsvd_rgn->base + rsvd_rgn->size))
			continue;

		if (rbase >= end)
			break;
		if (rend <= start)
			continue;
		
		if (rgn->size >= size && IS_ALIGNED(rbase, align))
			return rbase;
	}

	/* we failed to find an area that meets the requirements */
	return 0;
}

void *memblock_phys_alloc(phys_addr_t size, phys_addr_t align,
			  phys_addr_t start, phys_addr_t end)
{
	if (end == MEMBLOCK_ALLOC_ACCESSIBLE)
		end = memblock.curr_limit;

	phys_addr_t base = memblock_find_free_area(size, align, start, end);
	if (base == 0)
		return NULL;
	if (memblock_reserve_kern(base, size) < 0)
		return NULL;
	return phys_to_virt(base);
}

static void memblock_dump(struct memblock_type *type)
{
	int i = 0;
	struct memblock_region *rgn;
	printk("%s.count = 0x%lx\n", type->name, type->count);

	for_each_memblock_type(i, type, rgn) {
		phys_addr_t base = rgn->base;
		phys_addr_t size = rgn->size;
		phys_addr_t end = rgn->base + size - 1;
		
		printk("%s[%#x]\t[0x%016llx-0x%016llx], 0x%016llx bytes flags: %#x\n",
			type->name, i, base, end, size, rgn->flags);
	}
}

static void __memblock_dump_all(void)
{
	printk("Memblock Configuration:\n");
	printk("Current allocation limit: 0x%016llx\n", memblock.curr_limit);
	printk(" memory size = %016llx    reserved memory size: %016llx\n",
		memblock.memory.total_size, memblock.reserved.total_size);
	
	memblock_dump(&memblock.memory);
	memblock_dump(&memblock.reserved);
}

void memblock_dump_all(void)
{
	if (memblock_debug)
		__memblock_dump_all();
}

void test_memblock_visual()
{
	if (!memblock_debug)
		return;

        /* Base allocation: [0x1000 - 0xAFF], size 0xA000 */
        memblock_add(0x1000, 0xa000);
        
        /* 1. Disjoint Left: No Change */
        memblock_remove(0x0, 0x0800);
        
        /* 2. Adjacent Touch Right: No change */
        memblock_remove(0xb000, 0x1000);
        
        /* 3. Internal Split: Add gap [0x5000, 0x7000) -> leaves 2 rgns */
        memblock_remove(0x5000, 0x2000);
        
        /* 4. Overlap Head: Remove [0x0800, 0x2000) -> shrinks R0 base to 0x2000 */
        memblock_remove(0x0800, 0x1800);
        
        /* 5. Trim Tail: Remove [0x4000, 0x4800) -> shrinks R0 to [0x2000, 0x4000) */
        memblock_remove(0x4000, 0x0800);
        
        /* 6. Multiple: Remove [0x3800, 0x8000) -> trims R0 tail and R1 head across gap */
        memblock_remove(0x3800, 0x4800);
        
        /* 7. Entire Region: Remove [0x1800, 0x3C00) -> deletes R0 completely */
        memblock_remove(0x1800, 0x2400);

        /* 8. Exact Match: Remove remaining [0x9000, 0xB000) -> empties table */
        memblock_remove(0x8000, 0x3000);

        memblock_dump_all();
}