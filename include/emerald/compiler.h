#ifndef __EMERALD_COMPILER_H
#define __EMERALD_COMPILER_H

#include "compiler_types.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define unreachable() __builtin_unreachable()

#endif