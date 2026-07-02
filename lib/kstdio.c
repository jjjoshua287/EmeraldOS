// kernel space formatted standard input output (stdio) operations.

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static void reverse_string(char *start, char *end)
{
        while (start < end) {
                char tmp = *start;
                *start++ = *end;
                *end-- = tmp;
        }
}

// convert an unsigned integer into a string.
static char *utoa(uint64_t n, char *out_buf, int radix)
{
        int idx = 0;
        const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

        do {
                out_buf[idx++] = digits[n % radix];
        } while ((n /= radix) != 0);

        reverse_string(out_buf, out_buf + idx - 1);
        out_buf[idx] = '\0';

        return out_buf;
}

// signed int wrapper for utoa. converts an integer into a string.
static char *itoa(int n, char *out_buf, int radix)
{
        int idx = 0;
        uint64_t val = n;
        if (n < 0 && radix == 10) {
                out_buf[idx++] = '-';
                val = (uint64_t)(-n);
        }

        return utoa(val, out_buf + idx, radix);
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

static inline int specifier_has_prefix(char spec)
{
        return (spec == 'x' || spec == 'X' || spec == 'p');
}

static inline void add_prefix(const char *prefix, char *buffer, size_t size, size_t *written)
{
        while (*prefix && *written <= size - 1)
                buffer[(*written)++] = *prefix++;
}

/**
 * format_specifier() - convert 1 specifier to a string
 *      - this has no bounds checking
 * 
 * @spec: the specifier
 * @args: valist to args
 * @str:  buffer for string
 * 
 * Return: the specifier as a null-terminated string
 */
static char *format_specifier(char spec, va_list *args, char *str)
{
        switch (spec) {
        case 'd':
        case 'i':
                return itoa(va_arg(*args, int), str, 10);

        case 'u':
                return utoa(va_arg(*args, unsigned int), str, 10);

        case 'x':
        case 'X':
                return utoa(va_arg(*args, unsigned int), str, 16);

        case 'p':
                return utoa((uintptr_t)va_arg(*args, void *), str, 16);

        case 's':
                return va_arg(*args, char *);

        case 'c':
                str[0] = (char)va_arg(*args, int);
                str[1] = '\0';
                return str;

        default:
                str[0] = spec;
                str[1] = '\0';
                return str;
        }
}

// note: this currently doesn't honor submodifiers such as %5d.
int kvscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
        size_t written = 0;
        char str[65];

        while (*fmt && written <= size - 1) {
                if (*fmt != '%') {
                        emit(buf, size, &written, *fmt++);
                        continue;
                }

                char spec = *++fmt;
                if (specifier_has_prefix(spec))
                        emit_str(buf, size, &written, "0x");

                char *s = format_specifier(spec, &args, str);
                emit_str(buf, size, &written, s);
                fmt++;
        }

        buf[written] = '\0';
        return written;
}

int snprintf(char *buf, size_t size, const char *format, ...) 
{
        va_list args;
        va_start(args, format);
        int ret = kvscnprintf(buf, size, format, args);
        va_end(args);
        return ret;
}