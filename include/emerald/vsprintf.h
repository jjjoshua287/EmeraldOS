#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stddef.h>
#include <stdarg.h>

/**
 * struct printk_info: format string for printk
 * @prec: precision
 * @width: width
 * @spec: format letter
 * @is_long_double: L/ll flag
 * @is_short: h flag
 * @is_long: l flag
 * @alt: # flag
 * @space: Space flag
 * @left: - flag
 * @showsign: + flag
 * @is_char: hh flag
 * @pad: padding character
 */
struct printk_info {
        int prec;
        int width;
        wchar_t spec;
        unsigned int is_long_double:1;
        unsigned int is_short:1;
        unsigned int is_long:1;
        unsigned int alt:1;
        unsigned int space:1;
        unsigned int left:1;
        unsigned int showsign:1;
        unsigned int is_char:1;
        wchar_t pad;
};

#define PRINTF_MAX_WIDTH 2048

int kvsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);
int kvscnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int kscnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* VSPRINTF_H */