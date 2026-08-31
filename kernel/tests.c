#ifdef CFG_TEST

#include <stddef.h>
#include <emerald/printk.h>

void test_printk_visual() {
    printk("=== BEGIN PRINTK VISUAL TESTS ===\n");

    /* 1. Basic Types & Escaping */
    printk("[BASIC] Str: '%s', Char: '%c', Pct: '%%'\n", "kernel", 'K');

    /* 2. Boundary Values & 64-bit Ints */
    printk("[LIMITS] i32 min/max: %d, %d\n", (int)-2147483648, 2147483647);
    printk("[LIMITS] u32 max: %u\n", 4294967295U);
    printk("[LIMITS] i64 min/max: %lld, %lld\n", -9223372036854775807LL - 1, 9223372036854775807LL);
    printk("[LIMITS] u64 max: %llu\n", 18446744073709551615ULL);

    /* 3. Hex, Octal, and '#' Alternate Form */
    printk("[BASE] Hex lower/upper: %x, %X\n", 0xdeadbeef, 0xdeadbeef);
    printk("[BASE] Alternate hex: %#x, %#X\n", 0xcafe, 0xcafe);
    printk("[BASE] Alternate zero (no prefix): %#x, %#o\n", 0, 0);

    /* 4. Flags: Signs, Zero Padding, Left Alignment */
    printk("[FLAGS] Force sign (+): %+d, %+d\n", 42, -42);
    printk("[FLAGS] Space sign ( ): '% d', '% d'\n", 42, -42);
    printk("[FLAGS] + overrides space: '%+ d'\n", 42);
    printk("[FLAGS] Zero pad vs Left align: [%08x] [%-8d] [%-08d]\n", 0x1a3, 42, 42);

    /* 5. Precision & Truncation */
    printk("[PREC] Int zero with .0 (should be blank): '%.0d'\n", 0);
    printk("[PREC] Int padding (.5): '%.5d'\n", 42);
    printk("[PREC] String truncation (.4): '%.4s'\n", "LongKernelString");

    /* 6. Dynamic Width & Precision (*) */
    printk("[DYNAMIC] Width 8: [%*d], Left: [%*d]\n", 8, 123, -8, 123);
    printk("[DYNAMIC] Precision: [%.*s]\n", 3, "TruncateMe");
    printk("[DYNAMIC] Width+Prec: [%*.*s]\n", 8, 4, "KernelEngine");

    /* 7. Pointers */
    printk("[PTR] Regular: %p, Null: %p\n", (void*)0xffff800000000000ULL, NULL);

    printk("=== END PRINTK VISUAL TESTS ===\n");
}

void run_dev_tests()
{
    test_printk_visual();
}

#else

void run_dev_tests() {};

#endif