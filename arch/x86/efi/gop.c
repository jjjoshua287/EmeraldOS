#include "efi.h"
#include "gop.h"

static efi_gop_t *gop;

// Locate Graphics Output Protocol
EFI_STATUS locate_gop(efi_boot_services_t *gBS)
{
        // Locate the Graphics Output Protocol
        efi_guid_t guid = EFI_GOP_GUID;
        EFI_STATUS status = gBS->LocateProtocol(&guid, NULL, (void **)&gop);
        return status;
}

// draw a pixel
void draw_pixel(u64 x, u64 y, u32 color)
{
        u32 *fb = (u32 *)gop->Mode->FrameBufferBase;
        u64 pitch = gop->Mode->Info->PixelsPerScanLine;
        fb[(y * pitch) + x] = color;
}

// draw rectangle
void draw_filled_rect(u64 x, u64 y, u64 w, u64 h, u32 color)
{
    u32 *fb = (u32 *)gop->Mode->FrameBufferBase;
    u64 pitch = gop->Mode->Info->PixelsPerScanLine;
    for (u64 row = y; row < y + h; row++)
        for (u64 col = x; col < x + w; col++)
            fb[row * pitch + col] = color;
}

void draw_smpte_bars(efi_gop_t *gop, u32 start_y)
{
        u32 w = gop->Mode->Info->ResolutionX;
        u32 h = gop->Mode->Info->ResolutionY - start_y;
        u32 bar_w = w / 7;

        // top section — 67% of height
        u32 top_h = (h * 2) / 3;
        u32 top_colors[7] = {
                0xC0C0C0, // grey
                0xFFFF00, // yellow
                0x00FFFF, // cyan
                0x00FF00, // green
                0xFF00FF, // magenta
                0xFF0000, // red
                0x0000FF  // blue
        };
        for (int i = 0; i < 7; i++)
                draw_filled_rect(i * bar_w, start_y, bar_w, top_h, top_colors[i]);

        // middle strip — small
        u32 mid_y = start_y + top_h;
        u32 mid_h = h / 12;
        u32 mid_colors[7] = {
                0x0000FF, 0x000000, 0xFF00FF, 0x000000,
                0x00FFFF, 0x000000, 0xC0C0C0
        };
        for (int i = 0; i < 7; i++)
            draw_filled_rect(i * bar_w, mid_y, bar_w, mid_h, mid_colors[i]);

        // bottom section — remainder
        u32 bot_y = mid_y + mid_h;
        u32 bot_h = (start_y + h) - bot_y;
        u32 bot_colors[7] = {
                0x0D2B45, 0xFFFFFF, 0x1A0A2E,
                0x000000, 0x000000, 0x000000, 0xC0C0C0
        };
        for (int i = 0; i < 7; i++)
            draw_filled_rect(i * bar_w, bot_y, bar_w, bot_h, bot_colors[i]);
}