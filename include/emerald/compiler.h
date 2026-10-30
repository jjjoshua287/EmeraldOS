#ifndef __EMERALD_COMPILER_H
#define __EMERALD_COMPILER_H

#include "compiler_types.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define unreachable() __builtin_unreachable()

#define READ_ONCE(x) \
({ \
	typeof(x) __x = *(volatile typeof(x) *)&(x); \
	__x; \
})

#define WRITE_ONCE(x, val) \
do { \
	*(volatile typeof(x) *)&(x) = (val); \
} while(0)

#endif