#ifndef _KERNEL_STDIO_H
#define _KERNEL_STDIO_H

#include <stdarg.h>
#include <stddef.h>

int kvscnprintf(char *buf, size_t size, const char *format, va_list args);
int snprintf(char *buf, size_t size, const char *format, ...);

#endif