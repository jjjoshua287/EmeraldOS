#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/* calculate the length of a string excluding the null terminator */
size_t strlen(char *s);

/* set N bytes of dest to val */
void *memset(void *dest, int val, size_t n);

/* copy N bytes of src to dest */
void *memcpy(void *dest, const void *src, size_t n);

#endif