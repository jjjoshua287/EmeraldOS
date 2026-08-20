#include <emerald/types.h>
#include <emerald/vsprintf.h>

#define is_digit(c) (((c) >= '0') && ((c) <= '9'))

static void reverse_string(char *start, char *end)
{
        while (start < end) {
                char tmp = *start;
                *start++ = *end;
                *end-- = tmp;
        }
}

/* convert an ascii number to an integer up to 10 digits long */
static int atoi(const char *s)
{
        int ret = 0;
        for (int i = 0; i < 10 && *s; i++)
                ret = ret * 10 + (*s++ - '0');
        return ret;
}

static void _parse_precision(const char *fmt, va_list args, struct printk_spec *spec)
{
        if (is_digit(*fmt)) {
                spec->precision = atoi(fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->precision = va_arg(args, int);
        }
}

static void _parse_width(const char *fmt, va_list args, struct printk_spec *spec)
{
        if (is_digit(*fmt)) {
                spec->width = atoi(fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->width = va_arg(args, int);
                if (spec->width < 0) {
                        spec->width = -spec->width;
                        spec->flags |= FLAG_LEFT;
                }
        }
}

static void _parse_flags(const char *fmt, struct printk_spec *spec)
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
                        return;
                }
                fmt++;
        }
}

/* this function populates a struct printk_spec */
int _parse_fmt_spec(const char *fmt, va_list args, struct printk_spec *spec)
{
        /* advance past the '% */
        _parse_flags(++fmt, spec);
        _parse_width(fmt, args, spec);
	if (*fmt == '.')
		_parse_precision(++fmt, args, spec);
}