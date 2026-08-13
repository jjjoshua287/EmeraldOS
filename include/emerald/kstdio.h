#ifndef _KERNEL_STDIO_H
#define _KERNEL_STDIO_H

#include <stdarg.h>
#include <stddef.h>

/* NOTE: This header will be deprecated in v0.3 in favor of vsprintf.h which will house the mostly complete implementation */

int kvscnprintf(char *buf, size_t size, const char *format, va_list args);
int snprintf(char *buf, size_t size, const char *format, ...);

#endif