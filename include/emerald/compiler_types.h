#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#ifndef __same_type
#define __same_type(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
#endif

#define __packed __attribute((__packed__))
#define __aligned(x) __attribute((__aligned__(x)))

#endif