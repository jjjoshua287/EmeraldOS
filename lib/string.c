#include <emerald/string.h>

size_t strlen(char *s)
{
    size_t i = 0;
    while (*s++ != '\0')
        i++;
    return i;
}

void *memset(void *dest, int val, size_t n)
{
        for (int i = 0; i < n; i++)
                *((unsigned char*)dest + i) = val;
        return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
        for (int i = 0; i < n; i++)
                *((unsigned char*)dest + i) = *((unsigned char*)src + i);
        return dest;
}