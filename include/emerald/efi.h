#ifndef EMERALD_EFI_H
#define EMERALD_EFI_H

#include <emerald/types.h>

/*
 * Extensible Firmware Interface
 * Based on 'Extensible Firmware Interface Specification 1.10'
 */

typedef u64 RETURN_STATUS;

typedef unsigned long efi_status_t;
typedef u8 efi_bool_t;
typedef u16 efi_char16_t;   // UNICODE character
typedef void *efi_handle_t;

typedef u64 efi_phys_addr_t;
typedef u64 efi_virt_addr_t;

typedef enum {
        EfiReservedMemoryType,
        EfiLoaderCode,
        EfiLoaderData,
        /* More Types will be added as they're used */
} efi_memory_type;

typedef struct {
        u32             Type;
        efi_phys_addr_t PhysicalStart;
        efi_virt_addr_t VirtualStart;
        u64             NumberOfPages;
        u64             Attribute;
} efi_memory_descriptor;

#define EFI_MEMORY_DESCRIPTOR_VERSION 1

#define MAX_BIT 0x8000000000000000ULL

#define ENCODE_ERROR(StatusCode) ((RETURN_STATUS)(MAX_BIT | (StatusCode)))
#define RETURN_ERROR(StatusCode) (((s64)(RETURN_STATUS)(StatusCode)) < 0)

#define EFI_SUCCESS            0
#define EFI_LOAD_ERROR         ENCODE_ERROR(1)
#define EFI_INVALID_PARAMETER  ENCODE_ERROR(2)
#define EFI_UNSUPPORTED        ENCODE_ERROR(3)
#define EFI_BAD_BUFFER_SIZE    ENCODE_ERROR(4)
#define EFI_BUFFER_TOO_SMALL   ENCODE_ERROR(5)
#define EFI_NOT_READY          ENCODE_ERROR(6)
#define EFI_DEVICE_ERROR       ENCODE_ERROR(7)
#define EFI_WRITE_PROTECTED    ENCODE_ERROR(8)
#define EFI_OUT_OF_RESOURCES   ENCODE_ERROR(9)
#define EFI_NOT_FOUND          ENCODE_ERROR(14)
#define EFI_ACCESS_DENIED      ENCODE_ERROR(15)
#define EFI_TIMEOUT            ENCODE_ERROR(18)
#define EFI_ABORTED            ENCODE_ERROR(21)
#define EFI_SECURITY_VIOLATION ENCODE_ERROR(26)

#define EFI_ERROR(A) (RETURN_ERROR(A))

typedef struct {
        u32 Data1;
        u16 Data2;
        u16 Data3;
        u8  Data4[8];
} efi_guid_t;


// EFI table header
typedef struct {
        u64 Signature;
        u32 Revision;
        u32 HeaderSize;
        u32 CRC32;
        u32 Reserved;
} efi_table_hdr_t;

typedef struct efi_boot_services efi_boot_services_t;

/*
 * Types and defines for EFI ResetSystem
 */
#define EFI_RESET_COLD     0
#define EFI_RESET_WARM     1
#define EFI_RESET_SHUTDOWN 2

/*
 * EFI Runtime Services table
 */ 
#define EFI_RUNTIME_SERVICES_SIGNATURE ((u64)0x5652453544e5552ULL)
#define EFI_RUNTIME_SERVICES_REVISION  0x00010000

typedef struct {
	efi_table_hdr_t hdr;

        // Time Services
	void *GetTime;
	void *SetTime;
	void *GetWakeupTime;
	void *SetWakeupTime;

        // Virtual Memory Services (not implemented)
	void *SetVirtualAddressMap;
	void *ConvertPointer;

        // Variable Services
	void *GetVariable;
	void *GetNextVariable;
	void *SetVariable;

        // Miscellaneous Services
	void *GetNextHighMonoCount;
	void *ResetSystem;

        // Capsule/Variable Info (not implemented)
	void *UpdateCapsule;
	void *QueryCapsuleCaps;
	void *QueryVariableInfo;
} efi_runtime_services_t;

/*
 * EFI Configuration Table and GUID definitions
*/
#define NULL_GUID \
    { 0x00000000, 0x0000, 0x0000, \
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, \
        { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

typedef struct {
        efi_guid_t VendorGuid;
        void       *VendorTable;
} efi_cfg_table_t;

typedef void* efi_event_t;

typedef struct {
	u16 ScanCode;
	efi_char16_t UnicodeChar;
} efi_input_key_t;

struct efi_simple_text_input_protocol {
	void *Reset;
	efi_status_t (*read_keystroke)(
		struct efi_simple_text_input_protocol *This,
		efi_input_key_t *Key		
	);
	efi_event_t WaitForKey;
};

struct efi_simple_text_output_protocol {
        efi_status_t (*Reset)(
        	struct efi_simple_text_output_protocol *This,
                bool ExtendedVerification
        );
        efi_status_t (*OutputString)(
                struct efi_simple_text_output_protocol *This,
                efi_char16_t *String
        );
};

typedef struct efi_simple_text_input_protocol efi_simple_text_input_protocol_t;
typedef struct efi_simple_text_output_protocol efi_simple_text_output_protocol_t;

typedef struct efi_system_table { 
        efi_table_hdr_t                   Hdr;
        efi_char16_t                      *FirmwareVendor;
        u32                               FirmwareRevision;
        efi_handle_t                      ConsoleInHandle;
        struct efi_simple_text_input_protocol *ConIn;
        efi_handle_t                      ConsoleOutHandle;
        struct efi_simple_text_output_protocol *ConOut;
        efi_handle_t                      *StandardErrorHandle;
        void                              *StdErr;
        void                              *RuntimeServices;
        efi_boot_services_t               *BootServices;
        u64                               NumberOfTableEntries;
        efi_cfg_table_t                   *ConfigurationTable;
} efi_system_table_t;

// runtime framebuffer info obtained during EFI Phase
struct screen_info {
        u64 lfb_base;
        u32 lfb_width;
        u32 lfb_height;
        u32 lfb_ppsl;
};

// TODO: Name things better
extern struct screen_info info;
efi_status_t do_efi_things(struct efi_system_table *SystemTable);

void draw_pixel(struct screen_info *info, u64 x, u64 y, u32 color);

#endif // EMERALD_EFI_H