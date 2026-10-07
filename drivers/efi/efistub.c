#include <emerald/efi.h>
#include <emerald/string.h>
#include <emerald/runtime.h>
#include <emerald/compiler.h>

#include "efistub.h"

static inline bool guidcmp(efi_guid_t a, efi_guid_t b)
{
        return memcmp(&a, &b, sizeof(efi_guid_t));
}

/**
 * get_efi_cfg_table - Locate pointer to the configuration table a efi_guid corresponds to. 
 * 
 * @SystemTable: Pointer to the EFI System Table
 * @guid: table GUID
 * 
 * Return: A pointer to the corresponding configuration table, or NULL if not found
 */
void *get_efi_cfg_table(efi_system_table_t *SystemTable, efi_guid_t guid)
{
        for (int i = 0; i < SystemTable->NumberOfTableEntries; i++) {
                if (guidcmp(SystemTable->ConfigurationTable->VendorGuid, guid) == 0)
                        return SystemTable->ConfigurationTable->VendorTable;
                SystemTable->ConfigurationTable->VendorTable++;
        }
        return NULL;
}

efi_graphics_output_protocol_t *gop;
struct screen_info scr_info;

// Locate Graphics Output Protocol
static efi_status_t locate_gop(efi_boot_services_t *gBS)
{
        // Locate the Graphics Output Protocol
        gop = NULL;
        efi_guid_t guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        efi_status_t status = gBS->LocateProtocol(&guid, NULL, (void **)&gop);
        return status;
}

static efi_status_t setup_graphics_output_protocol(efi_system_table_t *SystemTable)
{
        efi_status_t status = locate_gop(SystemTable->BootServices);
        if (EFI_ERROR(status)) {
                // TODO: Handle the error
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Graphics Output Protocol: FAILURE");
        } else {
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Graphics Output Protocol: OK\n\r");
                
                scr_info.lfb_base   = gop->Mode->FrameBufferBase;
                scr_info.lfb_ppsl   = gop->Mode->Info->PixelsPerScanLine;
                scr_info.lfb_width  = gop->Mode->Info->HorizontalResolution;
                scr_info.lfb_height = gop->Mode->Info->VerticalResolution;
                scr_info.lfb_size   = gop->Mode->FrameBufferSize;
        }
        return status;
}

struct hw_memory_map hw_mem;

static efi_status_t get_memory_map(efi_boot_services_t *gBS)
{
        struct hw_memory_map mmap = { .size = 0, .memoryMap = NULL };
	efi_status_t status;

	gBS->GetMemoryMap(&mmap.size, NULL, &mmap.key, &mmap.descriptorSize, &mmap.version);
	mmap.size += 2 * mmap.descriptorSize;

	status = gBS->AllocatePool(EfiLoaderData, mmap.size, (void **)&mmap.memoryMap);
	if (EFI_ERROR(status))
		return status;

	status = gBS->GetMemoryMap(&mmap.size, mmap.memoryMap, &mmap.key, &mmap.descriptorSize, &mmap.version);

	memcpy(&hw_mem, &mmap, sizeof(struct hw_memory_map));
	return status;
}

// Handle ExitBootServices()
static efi_status_t handle_exit(efi_handle_t ImageHandle, efi_system_table_t *SystemTable)
{
        efi_status_t status = SystemTable->BootServices->ExitBootServices(ImageHandle, hw_mem.key);
        if (status == EFI_SUCCESS)
                return status;

        status = get_memory_map(SystemTable->BootServices);
        if (EFI_ERROR(status))
                return status;
        
        status = SystemTable->BootServices->ExitBootServices(ImageHandle, hw_mem.key);
        return status;
}

struct boot_info boot;

/* Kernel ELF expects System V ABI, not MS ABI like UEFI does. */
typedef void (*kernel_entry_t)(struct boot_info *boot) __attribute__((sysv_abi));

/* Function prototypes from efistub_helpers.c */
void *load_kernel(struct efi_boot_services *gBS);
void efi_mmap_to_e820(const struct hw_memory_map *mmap, struct e820_table *out);
int efi_utoa(unsigned long long num, efi_char16_t *out_buf, int base);

static void efi_puthex(efi_system_table_t *SystemTable, unsigned long long v)
{
        efi_char16_t tmp[65];
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (efi_char16_t *)u"0x");
        efi_utoa(v, tmp, 16);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, tmp);
}

static void print_fb_info(efi_system_table_t *SystemTable) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"fb_base=");
        efi_puthex(SystemTable, scr_info.lfb_base);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\n\rfb_size=");
        efi_puthex(SystemTable, scr_info.lfb_size);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\n\r");
}

efi_status_t efi_main(efi_handle_t ImageHandle, efi_system_table_t *SystemTable)
{
        efi_status_t status = setup_graphics_output_protocol(SystemTable);
        if (EFI_ERROR(status))
                return status;

        status = get_memory_map(SystemTable->BootServices);
        if (EFI_ERROR(status))
                return status;

        /* We check if rsdp is NULL in case the ACPI 2.0 Table isn't supported.
         * This doesn't guarentee the RSDP is valid however. The kernel
         * still needs to ensure it is valid.
         */
        void *rsdp = get_efi_cfg_table(SystemTable, (efi_guid_t)EFI_ACPI_20_TABLE_GUID);
        if (rsdp == NULL)
                rsdp = get_efi_cfg_table(SystemTable, (efi_guid_t)ACPI_10_TABLE_GUID);

        print_fb_info(SystemTable);

        kernel_entry_t entry = (kernel_entry_t)load_kernel(SystemTable->BootServices);

        if (likely(entry != NULL)) {
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Loaded Kernel");
                status = handle_exit(ImageHandle, SystemTable);
                if (EFI_ERROR(status))
                        return status;

                /* Give the Kernel the struct boot_info it expects and call its entry point. */
                boot.rsdp = rsdp;
                boot.info = scr_info;
                efi_mmap_to_e820(&hw_mem, &boot.mem);
                entry(&boot);
        } else {
                /* we failed to load the kernel */
                return (efi_status_t)EFI_LOAD_ERROR;
        }

        unreachable();
}