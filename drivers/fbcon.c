#include <emerald/fbcon.h>
#include <emerald/string.h>

#define EOF -1

void clear_framebuffer(struct screen_info *info)
{
        void *fb = (void *)info->lfb_base;
        memset(fb, 0, info->lfb_size);
}

// draw a pixel
void draw_pixel(struct screen_info *info, u64 x, u64 y, enum PxColor color)
{
        // Prevent drawing outside boundaries
        if (x >= info->lfb_width || y >= info->lfb_height)
                return;

        u32 *fb = (u32 *)info->lfb_base;
        fb[(y * info->lfb_ppsl) + x] = color;
}

// draw rectangle
void draw_filled_rect(struct screen_info *info, u64 x, u64 y, u64 w, u64 h, enum PxColor color) {
    // Check for overflow in coordinates
    if (x + w < x || y + h < y) return; 

    // Clip the boundaries so they never exceed the screen dimensions
    u64 end_x = (x + w > info->lfb_width)  ? info->lfb_width  : x + w;
    u64 end_y = (y + h > info->lfb_height) ? info->lfb_height : y + h;

    // Safety check for starting boundaries
    if (x >= info->lfb_width || y >= info->lfb_height) return;

    u32 *fb = (u32 *)info->lfb_base;
    for (u64 row = y; row < end_y; row++) {
        u64 row_offset = row * info->lfb_ppsl;
        for (u64 col = x; col < end_x; col++) {
            fb[row_offset + col] = color;
        }
    }
}

struct fbcon framebuffer_con;

void draw_char(char c, u64 x, u64 y, enum PxColor color)
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

void fbcon_clear()
{
        clear_framebuffer(&framebuffer_con.info);
}