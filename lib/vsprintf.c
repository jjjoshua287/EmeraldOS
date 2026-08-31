#include <stdarg.h>
#include <emerald/ctype.h>
#include <emerald/string.h>
#include <emerald/vsprintf.h>

/* Reverse a string in-place. */
static void reverse_string(char *start, char *end)
{
        while (start < end) {
                char tmp = *start;
                *start++ = *end;
                *end-- = tmp;
        }
}

/* convert an ascii number to an integer up to 10 digits long */
static int skip_atoi(const char **s)
{
        int ret = 0;
        for (int i = 0; i < 10 && isdigit(**s); i++)
                ret = ret * 10 + (*(*s)++ - '0');
        return ret;
}

// convert an unsigned integer into a string, returning strlen()
static int utoa(unsigned long long num, char *out_buf, int base, int flags)
{
        int i = 0;
        const char *digits = (flags & FLAG_SMALL) ? "0123456789abcdef"
                                                  : "0123456789ABCDEF";
        do {
                out_buf[i++] = digits[num % base];
        } while ((num /= base) != 0);

        reverse_string(out_buf, out_buf + i - 1);
        out_buf[i] = '\0';

        return i;
}


static const char *_parse_precision(const char *fmt, va_list *ap, struct printk_spec *spec)
{
        /* advance past the '.' */
        fmt++;
        if (isdigit(*fmt)) {
                spec->precision = skip_atoi(&fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->precision = va_arg(*ap, int);
        } else {
                /* a single '.' was provided, */
                spec->precision = 0;
                return fmt;
        }

        /* Clamp precision to PRECISON_MAX to prevent overflow */
        if (spec->precision > PRECISION_MAX)
                spec->precision = PRECISION_MAX;
	return fmt;
}

static const char *_parse_width(const char *fmt, va_list *ap, struct printk_spec *spec)
{
        if (isdigit(*fmt)) {
                spec->width = skip_atoi(&fmt);
        } else if (*fmt == '*') {
                fmt++;
                spec->width = va_arg(*ap, int);
                if (spec->width < 0) {
                        spec->width = -spec->width;
                        spec->flags |= FLAG_LEFT;
                }
        }

        if (spec->width > FIELD_WIDTH_MAX)
                spec->width = FIELD_WIDTH_MAX;
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
                        spec->flags |= FLAG_PLUS;
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

/* update the format type if needed */
static enum format_type check_spec_type(const char conv)
{
        switch (conv) {
        case 'c':
                return FORMAT_TYPE_CHAR;
        case 's':
                return FORMAT_TYPE_STR;
        case 'p':
                return FORMAT_TYPE_PTR;
        case '%':
                return FORMAT_TYPE_PCT_CHAR;
        case 'f':
        case 'n':
                return FORMAT_TYPE_INVALID;
        default:
                return FORMAT_TYPE_NONE;
        }
}

/* Decode the Length modifer and set FORMAT_TYPE */
static const char *decode_length(const char *fmt, enum format_type *type)
{
        switch (fmt[0]) {
        case 'h':
                *type = (fmt[1] == 'h') ? FORMAT_TYPE_CHAR : FORMAT_TYPE_SHORT;
                break;
        case 'l':
                *type = (fmt[1] == 'l') ? FORMAT_TYPE_LONG_LONG : FORMAT_TYPE_LONG;
                break;
        case 'z':
                *type = FORMAT_TYPE_SIZE_T;
                break;
        case 't':
                *type = FORMAT_TYPE_PTR_DIFF;
                break;
        default:
                *type = check_spec_type(*fmt);
                return fmt;
        }

        if (*type == FORMAT_TYPE_CHAR || *type == FORMAT_TYPE_LONG_LONG)
                return fmt + 2;
        else
                return fmt + 1;
}

static void parse_base(char c, struct printk_spec *spec, enum format_type *type)
{
	switch (c) {
	case 'd':
        case 'i':
		spec->flags |= FLAG_SIGN;
                /* fallthrough */
        case 'u':
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
static const char *parse_fmt_spec(const char *fmt, va_list *ap, struct printk_spec *spec, 
				  enum format_type *type)
{
        fmt = _parse_flags(fmt, spec);
        fmt = _parse_width(fmt, ap, spec);
	if (*fmt == '.')
		fmt = _parse_precision(fmt, ap, spec);
        else
                spec->precision = -1;
	fmt = decode_length(fmt, type);
	
	parse_base(*fmt, spec, type);	
	return fmt;
}

static inline char *emit(char *buf, char *end, const char c)
{
        if (buf < end)
                *buf = c;
        return ++buf;
}

static inline char *emit_n(char *buf, char *end, const char *string, int n)
{
        for (int i = 0; i < n; i++)
                buf = emit(buf, end, string[i]); 
        return buf;
}

static inline char *emit_padding(char *buf, char *end, const char pad, int num)
{
        for (; num > 0; num--)
                buf = emit(buf, end, pad);
        return buf;
}

static char *number(char *buf, char *end, unsigned long long num, struct printk_spec spec) 
{
        char sign = '\0';
        char pad = ' ';
        char tmp[21];
        bool needs_pfx = false;
        int width = spec.width;
        int precision = spec.precision;

        /* Convert num to signed if it's negative when signed */
        if (spec.flags & FLAG_SIGN) {
                if ((signed long long)num < 0) {
                        sign = '-';
                        num = -(signed long long)num;
                        width--;
                } else if (spec.flags & FLAG_PLUS) {
                        sign = '+';
                        width--;
                } else if (spec.flags & FLAG_SPACE) {
                        sign = ' ';
                        width--;
                }
        }

        /* reserve width for prefix if necessary */
        if (spec.flags & FLAG_SPECIAL) {
                needs_pfx = num && ((spec.base == 8) || (spec.base == 16));
                if (spec.base == 8) {
                        needs_pfx = (!precision); 
                }
                if (needs_pfx)
                        width -= (spec.base == 16) ? 2 : 1;
        }

        /* calculate the number of digits to print */
        int len = utoa(num, tmp, spec.base, spec.flags);
        if (num == 0 && precision == 0)
                len = 0;
        if (spec.precision <= 0) {
                precision = len;
                if (spec.flags & FLAG_ZEROPAD && !(spec.flags & FLAG_LEFT))
                        pad = '0';
        } else if (len > precision) {
                precision = len;
        }
        width -= precision;

        /* emit leading padding if any */
        if (!(spec.flags & (FLAG_LEFT)))
                buf = emit_padding(buf, end, pad, width);
        if (sign)
                buf = emit(buf, end, sign);
        if (needs_pfx) {
                if (spec.base == 16)
                        buf = emit_n(buf, end, "0x", 2);
                else
                        buf = emit(buf, end, '0');
        }

        /* emit numbers after the prefix */
        if (precision > len)
                buf = emit_padding(buf, end, '0', precision - len);
        if (spec.precision)
                buf = emit_n(buf, end, tmp, len);
        
        /* emit extra padding if left-aligned */
        if (spec.flags & FLAG_LEFT)
                buf = emit_padding(buf, end, ' ', width);
        return buf;
}

/**
 * string: internal formatting helper for strings
 * @buf: string buffer
 * @end: end of string buffer
 * @str: string to format
 * @spec: printk specification passed by value
 * 
 * Return: updated string buffer
 */
static char *string(char *buf, char *end, const char *str, struct printk_spec spec) 
{
        int len;
        const char *s = (str == NULL) ? "(null)" : str;
        if (spec.precision >= 0)
                len = spec.precision;
        else
                len = strlen(s);

        if (!(spec.flags & FLAG_LEFT))
                buf = emit_padding(buf, end, ' ', spec.width - len);
        buf = emit_n(buf, end, s, len);
        if (spec.flags & FLAG_LEFT)
                buf = emit_padding(buf, end, ' ', spec.width - len);
        return buf;
}

static char *pointer(char *buf, char *end, const void *ptr, struct printk_spec spec)
{
        if (ptr == NULL)
                buf = string(buf, end, "(null)", spec);
        else
                buf = number(buf, end, (unsigned long long)ptr, spec);
        return buf;
}
/**
 * convert_num_spec: Turn a 1/2/4-byte value into a 64-bit one for printing.
 * Truncate the number as necessary and deal with signedness
 * 
 */
static unsigned long long convert_num_spec(unsigned int val, size_t size, struct printk_spec spec)
{
        unsigned int shift = 32 - size * 8;
        
        val <<= shift;
        if (!(spec.flags & FLAG_SIGN))
                return val >> shift;
        return (int)val >> shift;
}

/**
 * vsnprintf - Format a string and place it into a buffer
 * @buf: The character buffer to write data to.
 * @size: The size of the buffer, including the null terminator ('\0')
 * @fmt: the format string
 * @args: Arguments for the format string
 * 
 * `%f` and `%n` are unsupported
 * `%p*` are currently unsupported, but will be expanded on as the need arises.
 * 
 * Return: The number of the characters that would've been written,
 * excluding the null terminator.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
        /* keep a local copy so we can return an unmodified ptr to start of buf */
        char *str = buf;
        char *end = buf + size;
        unsigned long long num = 0;

        while (*fmt) {
                num = 0;
                if (*fmt != '%') {
                        str = emit(str, end, *fmt++);
			continue;
		}
                /* advance past the '%' */
                const char *start = fmt++;
		enum format_type type;
        	struct printk_spec spec = {0};

		fmt = parse_fmt_spec(fmt, &args, &spec, &type);
                fmt++;

                switch (type) {
                case FORMAT_TYPE_NONE:
                        if (!spec.base) {
                                str = emit_n(str, end, start, fmt - start);
                                continue;
                        }
                        num = convert_num_spec(va_arg(args, unsigned int), sizeof(int), spec);
                        break;
                case FORMAT_TYPE_CHAR:
                        if (spec.base) {
                                num = convert_num_spec(va_arg(args, unsigned int), sizeof(char), spec);
                                break;
                        }
                        
                        char c = (char)va_arg(args, unsigned int);
                        if (!(spec.flags & FLAG_LEFT))
                                str = emit_padding(str, end, ' ', spec.width - 1);
                        str = emit(str, end, c);
                        if (spec.flags & FLAG_LEFT)
                                str = emit_padding(str, end, ' ', spec.width - 1);
                        continue;
                case FORMAT_TYPE_SHORT:
                        num = convert_num_spec(va_arg(args, unsigned int), sizeof(short), spec);
                        break;
                case FORMAT_TYPE_LONG:
                        if (sizeof(long) == sizeof(int))
                                num = convert_num_spec((unsigned int)va_arg(args, unsigned long), sizeof(long), spec);
                        else
                                num = va_arg(args, unsigned long);
                        break;
                case FORMAT_TYPE_LONG_LONG:
                        num = va_arg(args, unsigned long long);
                        break;
                case FORMAT_TYPE_SIZE_T:
                        num = va_arg(args, size_t);
                        break;
                case FORMAT_TYPE_PTR_DIFF:
                        num = va_arg(args, ptrdiff_t);
                        break;
                case FORMAT_TYPE_PTR: {
                        const void *ptr = va_arg(args, void *);
                        str = pointer(str, end, ptr, spec);
                        continue;
                }
                case FORMAT_TYPE_STR: {
                        char *s = va_arg(args, char *);
                        str = string(str, end, s, spec);
                        continue;
                }
                case FORMAT_TYPE_PCT_CHAR:
                        str = emit(str, end, '%');
                        continue;
                case FORMAT_TYPE_INVALID:
                        str = emit_n(str, end, start, fmt - start);
                        continue;
                default:
                        /* something went wrong if we get here */
                        goto out;
                }
                str = number(str, end, num, spec);
	}
out:
        if (size > 0) {
                if (str < end)
                        *str = '\0';
                else
                        *(end - 1) = '\0';
        }
        return str - buf;
}

/**
 * vscnprintf - Format a string and place it into a buffer
 * @buf: The character buffer to write data to.
 * @size: The size of the buffer, including the null terminator ('\0')
 * @fmt: The format string
 * @args: Arguments for the format string
 * 
 * Return: The number of characters written into @buf.
 *      - If @size == 0, the return is 0
 * 
 * See the vsnprintf() documentation for more info
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
        if (!size)
                return 0;
        int i = vsnprintf(buf, size, fmt, args);
        if (i < size)
                return i;

        return size - 1;
}

/**
 * snprintf - Format a string and place it into a buffer
 * @buf: The character buffer to write data to.
 * @size: The size of the buffer, including the null terminator ('\0')
 * @fmt: The format string
 * @...: Arguments for the format string
 * 
 * Return: The number of the characters that would've been written,
 * excluding the null terminator.
 * 
 * See the vsnprintf() documentation for more info
 */
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int i = vsnprintf(buf, size, fmt, args);
        va_end(args);

        return i;
}

/**
 * scnprintf - Format a string and place it into a buffer
 * @buf: The character buffer to write data to.
 * @size: The size of the buffer, including the null terminator ('\0')
 * @fmt: The format string
 * @...: Arguments for the format string
 * 
 * Return: The number of characters written into @buf.
 *      - If @size == 0, the return is 0
 * 
 * See the vsnprintf() documentation for more info
 */
int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int i = vscnprintf(buf, size, fmt, args);
        va_end(args);
        
        return i;
}