#ifndef _LIB_FONTS_FONT_H
#define _LIB_FONTS_FONT_H

#include <emerald/compiler_types.h>
#include <emerald/font.h>

#define FONT_EXTRA_WORDS 4

struct font_data {
        unsigned int extra[FONT_EXTRA_WORDS];
        unsigned char data[];
} __packed;

// Built-in fonts
#define VGA8x16_IDX 0

#endif