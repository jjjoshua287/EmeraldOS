#include <stdarg.h>
#include <emerald/kstdio.h>

// temporary import until a driver struct with function ptrs implemented.
#include <emerald/fbcon.h>

void printk(const char *restrict fmt, ...)
{
        char kbuf[512];
        va_list args;

        va_start(args, fmt);
        kvscnprintf(kbuf, sizeof(kbuf), fmt, args);
        va_end(args);

        char *ptr = kbuf;
        while (*ptr)
                fbcon_write(*ptr++);
}