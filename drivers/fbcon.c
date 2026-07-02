#include <emerald/fbcon.h>

#define EOF -1
#define WHITE 0x00FFFFFF
#define BLACK 0x0

struct fbcon framebuffer_con;

void draw_char(char c, u64 x, u64 y, u32 color)
{
        const u8 *glyph = &font_vga_8x16.data[(u8)c * font_vga_8x16.height];
        for (u32 row = 0; row < font_vga_8x16.height; row++) {
                u8 bits = glyph[row];
                for (u32 col = 0; col < font_vga_8x16.width; col++) {
                        if (bits & (0x80 >> col))
                                draw_pixel(&framebuffer_con.info, x + col, y + row, color);
                }
        }
}

void fbcon_write(char c)
{
        switch (c) {
                case '\0':
                        break;

                case '\n':
                        framebuffer_con.cursorX = framebuffer_con.font->width;
                        framebuffer_con.cursorY += framebuffer_con.font->height;
                        break;

                case '\t':
                        framebuffer_con.cursorX += (framebuffer_con.font->width * 8);
                        break;

                case '\b':
                        draw_char(' ', framebuffer_con.cursorX -= framebuffer_con.font->width, framebuffer_con.cursorY, BLACK);
                        break;

                case '\r':
                        framebuffer_con.cursorX = framebuffer_con.font->width;
                        break;

                case '\f':
                        // this is a form feed sequence, but there's currently no form to feed.
                        framebuffer_con.cursorY += framebuffer_con.font->height;
                        break;

                case '\v':
                        framebuffer_con.cursorY += (framebuffer_con.font->height * 8);
                        break;

                default:
                        draw_char(c, framebuffer_con.cursorX, framebuffer_con.cursorY, WHITE);
                        framebuffer_con.cursorX += framebuffer_con.font->width;
                        break;
        }
}