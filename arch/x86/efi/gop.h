// Header file containing structures relevant to the Graphics Output Protocol
#ifndef GOP_H
#define GOP_H

#include "types.h"
#include "efi.h"

#define EFI_GOP_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

// Pixel Format
typedef enum {
    RGB_Pixel_Reserved_8Bit,
    BGR_Pixel_Reserved_8Bit,
    PixelBitMask,
    PixelBitOnly,
    PixelFormatMax
} efi_gop_pixel_format_t;

typedef struct {
    u32                    Version;
    u32                    ResolutionX;
    u32                    ResolutionY;
    efi_gop_pixel_format_t PixelFormat;
    u32                    PixelInformation[4];
    u32                    PixelsPerScanLine;
} efi_gop_mode_info_t;

typedef struct {
    u32                        MaxMode;
    u32                        Mode;
    efi_gop_mode_info_t *Info;
    u64                        SizeOfInfo;
    u64                        FrameBufferBase;
    u64                        FrameBufferSize;
} efi_gop_mode_t;

// EFI Graphics Output Protocol struct
typedef struct efi_gop_t {
    EFI_STATUS (*QueryMode)(
        struct efi_gop_t *This,
        u32 ModeNumber,
        u64 *SizeOfInfo,
        efi_gop_mode_info_t **Info
    );

    EFI_STATUS (*SetMode)(
        struct efi_gop_t *This,
        u32 ModeNumber
    );

    void           *Blt;
    efi_gop_mode_t *Mode;
} efi_gop_t;

// Prototypes
EFI_STATUS locate_gop(efi_boot_services_t *gBS);
void draw_pixel(u64 x, u64 y, u32 color);
void draw_filled_rect(u64 x, u64 y, u64 w, u64 h, u32 color);
void draw_smpte_bars(efi_gop_t *gop, u32 start_y);

#endif // GOP_H