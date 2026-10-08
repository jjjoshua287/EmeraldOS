#include <emerald/string.h>

size_t strlen(const char *s)
{
    	size_t i = 0;
    	while (*s++)
        	i++;
    	return i;
}

size_t strnlen(const char *s, size_t maxlen)
{
	size_t i = 0;
        while (i < maxlen && *s++)
		i++;
	return i;
}


void *memset(void *dest, int val, size_t n)
{
        unsigned char *p = (unsigned char *)dest;
        unsigned char c = (unsigned char)val;

        while (n--)
                *p++ = c;

        return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
        for (size_t i = 0; i < n; i++)
                *((unsigned char*)dest + i) = *((unsigned char*)src + i);
        return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
        if (dest < src) {
                // copy forward
                for (size_t i = 0; i < n; i++)
                        *((unsigned char *)dest + i) = *((unsigned char *)src + i);
        } else {
                // copy backward
                for (size_t i = n; i > 0; i--)
                        *((unsigned char *)dest + (i - 1)) = *((unsigned char *)src + (i - 1));
        }
        return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
        const unsigned char *p1 = (unsigned char *)s1; 
        const unsigned char *p2 = (unsigned char *)s2;
        for (size_t i = 0; i < n; i++) {
                if (p1[i] < p2[i])
                        return -1;
                else if (p1[i] > p2[i])
                        return 1;
        }
        return 0;
}
