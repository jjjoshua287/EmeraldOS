#include <emerald/types.h>
#include <emerald/vsprintf.h>

#define is_digit(c) (((c) >= '0') && ((c) <= '9'))

/* convert an ascii number to an integer up to 10 digits long */
static int skip_atoi(const char **s)
{
        int ret = 0;
        for (int i = 0; i < 10 && is_digit(**s); i++)
                ret = ret * 10 + (*(*s)++ - '0');
        return ret;
}

/* Reverse a string in-place. */
static void reverse_string(char *start, char *end)
{
        while (start < end) {
                char tmp = *start;
                *start++ = *end;
                *end-- = tmp;
        }
}

static const char *_parse_precision(const char *fmt, va_list args, struct printk_spec *spec)
{
        /* advance past the '.' */
        if (is_digit(*(++fmt))) {
                spec->precision = skip_atoi(fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->precision = va_arg(args, int);
        } else {
                spec->precision = -1;
        }
	return fmt;
}

static const char *_parse_width(const char *fmt, va_list args, struct printk_spec *spec)
{
        if (is_digit(*fmt)) {
                spec->width = skip_atoi(fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->width = va_arg(args, int);
                if (spec->width < 0) {
                        spec->width = -spec->width;
                        spec->flags |= FLAG_LEFT;
                }
        }
	return fmt;
}

static const char *_parse_flags(const char *fmt, struct printk_spec *spec)
{
        while (*fmt) {
                switch (*fmt) {
                case '-': 
                        spec->flags |= FLAG_LEFT;
                        break;
                case '+':
                        spec->flags |= FLAG_SIGN;
                        break;
                case ' ':
                        spec->flags |= FLAG_SPACE;
                        break;
                case '#':
                        spec->flags |= FLAG_SPECIAL;
                        break;
                case '0':
                        spec->flags |= FLAG_ZEROPAD;
                        break;
                default:
                        return fmt;
                }
                fmt++;
        }
	return fmt;
}

static const char *decode_length(const char *fmt, enum length_mod *lm)
{
	*lm = LEN_NONE;
	
	if (fmt[0] == 'h')
		*lm = (fmt[1] == 'h') ? LEN_HH : LEN_H;
	else if (fmt[0] == 'l')
		*lm = (fmt[1] == 'l') ? LEN_LL : LEN_L;
	else if (fmt[0] == 'z')
		*lm = LEN_Z;
	else if (fmt[0] == 't')
                *lm = LEN_T;
	fmt += (*lm == LEN_LL || *lm == LEN_HH) ? 2 : 1;
	return fmt;
}

static void parse_base(char c, struct printk_spec *spec, enum length_mod *lm)
{
	switch (c) {
	case 'd':
		spec->flags |= FLAG_SIGN;
		/* fallthrough */
	case 'i':
		spec->base = 10;
		break;
	case 'x':
		spec->flags |= FLAG_SMALL;
		/* fallthrough */
	case 'X':
	case 'p':
		spec->base = 16;
		break;
	case 'o':
		spec->base = 8;
		break;
	default:
		spec->base = 0;
		break;
	}
}

/**
 * _parse_fmt_spec: internal function to populate a printk_spec given a format string.
 * @fmt: Format string starting with '%'
 * @args: Variable argument list of arguments
 * @spec: Pointer to struct printk_spec.
 */
static const char *parse_fmt_spec(const char *fmt, va_list args, struct printk_spec *spec, 
				  enum length_mod *lm)
{
        fmt = _parse_flags(fmt, spec);
        fmt = _parse_width(fmt, args, spec);
	if (*fmt == '.')
		fmt = _parse_precision(fmt, args, spec);
	fmt = decode_length(fmt, lm);
	
	parse_base(*fmt, spec, lm);	
	return fmt;
}

static inline char *emit(char *buf, char *end, const char c)
{
        if (buf < end)
                *buf = c;
        return ++buf;
}

static inline char *emit_str(char *buf, char *end, const char *s)
{
        while (*s)
                emit(buf, end, *s++);
}

static char *number(char *buf, char *end, unsigned long long num, struct printk_spec spec) 
{}

static char *string(char *buf, char *end, const char *str, struct printk_spec spec) 
{}

static char *pointer(char *buf, char *end, const void *ptr, struct printk_spec spec)
{}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	char *str = buf;
        char *end = buf + size - 1; /* leave space for NULL terminator */

        while (*fmt && buf < end) {
                if (*fmt++ != '%') {
                        emit(str, end, *fmt);
			continue;
		}
		
		enum length_mod lm;
        	struct printk_spec spec = {0};
		fmt = parse_fmt_spec(fmt, args, &spec, &lm);
	}
}