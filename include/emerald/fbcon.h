#ifndef FBCON_H
#define FBCON_H

#include <emerald/runtime.h>
#include <emerald/types.h>
#include <emerald/font.h>

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

enum PxColor {
        BLACK  = 0x0,
        WHITE  = 0x00FFFFFF,
        RED    = 0x00FF0000,
        GREEN  = 0x0000FF00,
        BLUE   = 0x000000FF,
        YELLOW = 0x00FFFF00,
};

/* raw function that may be used in further implementation */
void draw_char(char c, u64 x, u64 y, enum PxColor color);

/* these are the intended functions to be called when importing this header */
void fbcon_write(char c);
void fbcon_clear();

#endif