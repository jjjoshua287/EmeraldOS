#ifndef FBCON_H
#define FBCON_H

#include <emerald/types.h>
#include <emerald/font.h>
#include <emerald/efi.h>

struct fbcon {
    int cursorX;
    int cursorY;
    struct screen_info info;
    const struct font_desc *font;
};
extern struct fbcon framebuffer_con;

/* Initalize framebuffer console driver with the info obtained in EFI Phase */
static inline void init_fbcon(struct screen_info *info)
{
        const struct font_desc *font = get_default_font();
        framebuffer_con.font = get_default_font();
        framebuffer_con.info = *info;
        framebuffer_con.cursorX = font->width;
        framebuffer_con.cursorY = font->height * 4;
}

void draw_char(char c, u64 x, u64 y, u32 color);
void fbcon_write(char c);

#endif