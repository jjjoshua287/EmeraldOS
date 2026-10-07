#ifndef EMERALD_RUNTIME_H
#define EMERALD_RUNTIME_H

#include <asm/e820/types.h>

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

/** struct boot_info - boot information for the kernel passed by the EFI stub
 * @info: GOP framebuffer info
 * @mem:  hardware memory map info
 */
struct boot_info {
        void *rsdp;
        struct screen_info info;
        struct e820_table mem;
};

extern struct boot_info boot;

#endif /* EMERALD_RUNTIME_H */