#ifndef FONT_H
#define FONT_H 

typedef const unsigned char font_data_t;

// Return the raw font data.
static inline const unsigned char *font_data_buf(font_data_t *fd)
{
        return (const unsigned char *)fd;
}

// Font descriptor
struct font_desc {
        int idx;
        const char *name;
        unsigned int width, height;
        unsigned int charcount;
        font_data_t *data;
        int pref;
};

// Get the default font
extern const struct font_desc *get_default_font();

// Built-in fonts

extern const struct font_desc font_vga_8x16;

#endif // FONT_H