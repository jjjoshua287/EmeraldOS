#include <emerald/efi.h>
#include <emerald/string.h>
#include <emerald/runtime.h>

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
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Graphics Output Protocol: OK\n");
                
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

static inline void fill_boot_info()
{
        boot.info = scr_info;
        boot.mem = hw_mem;
}

extern void start_64(void);

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

        boot.rsdp = rsdp;
        fill_boot_info();
                
        status = handle_exit(ImageHandle, SystemTable);
        if (EFI_ERROR(status))
                return status;

        /* Only RuntimeServices exist now. */
        start_64();
        return EFI_SUCCESS;
}