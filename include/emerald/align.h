#ifndef EMERALD_ALIGN_H
#define EMERALD_ALIGN_H

/* round a value up to a mask */
#define __ALIGN_MASK(x, mask) 	(((x) + (mask)) & ~(mask))

/* round a value up to the next multiple of a */
#define ALIGN(x, a) 		__ALIGN_MASK((x), ((typeof(x))(a) - 1))

/* round a value down to the previous mutiple of a */
#define ALIGN_DOWN(x, a) 	ALIGN((x) - ((a) - 1), (a))

#define IS_ALIGNED(x, a) 	(((x) & ((typeof(x))(a) - 1)) == 0)

#endif