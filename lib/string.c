#include <emerald/string.h>

size_t strlen(const char *s)
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

int memcmp(const void *s1, const void *s2, size_t n)
{
        for (int i = 0; i < n; i++) {
                unsigned char b1 = *(unsigned char *)(s1 + i);
                unsigned char b2 = *(unsigned char *)(s2 + i);
                
                if (b1 < b2)
                        return -1;
                else if (b1 > b2)
                        return 1;
        }
        return 0;
}