#include <emerald/efi.h>
#include <emerald/string.h>

#include "efistub.h"

efi_graphics_output_protocol_t *gop;
struct screen_info info;

// draw a pixel
void draw_pixel(struct screen_info *info, u64 x, u64 y, u32 color)
{
        // Prevent drawing outside boundaries
        if (x >= info->lfb_width || y >= info->lfb_height)
                return;

        u32 *fb = (u32 *)info->lfb_base;
        fb[(y * info->lfb_ppsl) + x] = color;
}

// draw rectangle
void draw_filled_rect(struct screen_info *info, u64 x, u64 y, u64 w, u64 h, u32 color) {
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

// Locate Graphics Output Protocol
efi_status_t locate_gop(efi_boot_services_t *gBS)
{
        // Locate the Graphics Output Protocol
        gop = NULL;
        efi_guid_t guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        efi_status_t status = gBS->LocateProtocol(&guid, NULL, (void **)&gop);
        return status;
}

efi_status_t setup_graphics_output_protocol(efi_system_table_t *SystemTable)
{
        efi_status_t status = locate_gop(SystemTable->BootServices);
        if (EFI_ERROR(status)) {
                // TODO: Handle the error
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Graphics Output Protocol: FAILURE");
        } else {
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Graphics Output Protocol: OK\n");
                
                info.lfb_base   = gop->Mode->FrameBufferBase;
                info.lfb_ppsl   = gop->Mode->Info->PixelsPerScanLine;
                info.lfb_width = gop->Mode->Info->HorizontalResolution;
                info.lfb_height  = gop->Mode->Info->VerticalResolution;
        }
        return status;
}

struct efi_memory_map mem;

efi_status_t get_memory_map(efi_boot_services_t *gBS)
{
        struct efi_memory_map mmap = { .size = 0, .memoryMap = NULL };
	efi_status_t status;

	gBS->GetMemoryMap(&mmap.size, NULL, &mmap.key, &mmap.descriptorSize, &mmap.version);
	mmap.size += 2 * mmap.descriptorSize;

	status = gBS->AllocatePool(EfiLoaderData, mmap.size, (void **)&mmap.memoryMap);
	if (EFI_ERROR(status))
		return status;

	status = gBS->GetMemoryMap(&mmap.size, mmap.memoryMap, &mmap.key, &mmap.descriptorSize, &mmap.version);
	if (EFI_ERROR(status))
		return status;

	memcpy(&mem, &mmap, sizeof(struct efi_memory_map));
	return EFI_SUCCESS;
}

#include <asm/gdt.h>

efi_status_t do_efi_things(efi_system_table_t *SystemTable)
{
        efi_status_t status = setup_graphics_output_protocol(SystemTable);
        if (status == EFI_SUCCESS)
                status = get_memory_map(SystemTable->BootServices);
        
        return status;
        // more things should be done in the future as kernel grows.
}