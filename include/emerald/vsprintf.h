#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stdarg.h>
#include <stddef.h>

#include <emerald/compiler_types.h>

enum flags {
	FLAG_SIGN    = 1 << 0,	/* unsigned/signed */
	FLAG_LEFT    = 1 << 1,	/* left jusified */
	FLAG_PLUS    = 1 << 2,	/* show sign */
	FLAG_SPACE   = 1 << 3,	/* show space */
	FLAG_ZEROPAD = 1 << 4,	/* pad with zero */
	FLAG_SMALL   = 1 << 5,	/* use lowercase hex */
	FLAG_SPECIAL = 1 << 6,	/* prefix hex with "0x", octal with "0 "*/
};

enum format_type {
	FORMAT_TYPE_NONE,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_SIZE_T,
	FORMAT_TYPE_PTR_DIFF,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PCT_CHAR,
};

struct printk_spec {
	unsigned char  flags;
	unsigned char  base;		/* number base: 8, 10, or 16 only */
	short          precision;	/* number of digits/chars */
	int            width;		/* width of output field */
} __packed;
static_assert(sizeof(struct printk_spec) == 8);

#define FIELD_WIDTH_MAX 2048
#define PRECISION_MAX  	((1 << 15) - 1)

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int scnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* VSPRINTF_H */