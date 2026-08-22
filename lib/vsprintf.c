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
        if (is_digit(*fmt)) {
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
	
	fmt += (*lm == LEN_LL || *lm == LEN_HH) ? 2 : 1;
	return fmt;
}

static void parse_base(char c, struct printk_spec *spec, enum length_mod *lm)
{
	switch (c) {
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
		spec->base = 10;
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
        /* advance past the '% */
        fmt = _parse_flags(++fmt, spec);
        fmt = _parse_width(fmt, args, spec);
	if (*fmt == '.')
		fmt = _parse_precision(++fmt, args, spec);
	fmt = decode_length(fmt, lm);
	
	parse_base(*fmt, spec, lm);	
	return fmt;
}

/* Single bounds-checked buffer write helper */
static inline void emit(char *buf, size_t size, size_t *written, char c)
{
        if (*written <= size - 1)
                buf[(*written)++] = c;
}

// write a string to a buffer
static inline void emit_str(char *buf, size_t size, size_t *written, const char *s)
{
        while (*s && *written <= size - 1)
                emit(buf, size, written, *s++);
}

int kvsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	struct printk_spec spec;
	enum length_mod lm;
	fmt = parse_fmt_spec(fmt, args, &spec, &lm);
}