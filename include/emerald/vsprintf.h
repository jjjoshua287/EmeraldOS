#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stdarg.h>
#include <stddef.h>

enum flags {
	FLAG_SIGN    = 1 << 0,
	FLAG_LEFT    = 1 << 1,
	FLAG_PLUS    = 1 << 2,
	FLAG_SPACE   = 1 << 3,
	FLAG_ZEROPAD = 1 << 4,
	FLAG_SMALL   = 1 << 5,
	FLAG_SPECIAL = 1 << 6,
};

enum length_modifier {
	LEN_DEFAULT = 0,
	LEN_SHORT   = 1,
	LEN_LONG    = 2,
	LEN_LLONG   = 3,
	LEN_SIZE_T  = 4,
};

struct printk_spec {
	unsigned char  flags;
	unsigned char  base;		/* number base: 8, 10, or 16 only */
	short          precision;	/* number of digits/chars */
	int            width;		/* width of output field */
};

#define FIELD_WIDTH_MAX 2048
#define PRECISION_MAX ((1 << 15) - 1)

#define PRINTK_SPEC_INIT { 	\
	.base = 10,				\
	.precision = -1,		\
}

int kvsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);
int kvscnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int kscnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* VSPRINTF_H */