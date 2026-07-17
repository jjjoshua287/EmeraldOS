#ifndef EMERALD_RUNTIME_H
#define EMERALD_RUNTIME_H

#include <emerald/types.h>
#include <emerald/efi.h>

/* 
 * Runtime structures for use post-ExitBootServices()
 * NOTE: These structures are filled in during the EFI Phase,
 */

// runtime framebuffer info obtained during EFI Phase
struct screen_info {
        u64 lfb_base;
        u32 lfb_width;
        u32 lfb_height;
        u32 lfb_ppsl;
        size_t lfb_size;
};

// runtime memory map info obtained during EFI Phase
struct hw_memory_map {
        u64 size;
        efi_memory_descriptor *memoryMap;
        u64 key;
        u64 descriptorSize;
        u32 version;
};

extern struct screen_info info;
extern struct hw_memory_map mem;

int start_kernel(void);

#endif // EMERALD_RUNTIME_H