#ifndef EFI_H
#define EFI_H

#include "types.h"
#include "efi_types.h"

typedef u16 CHAR16;
typedef u64 EFI_STATUS;
typedef void* EFI_HANDLE;

typedef struct {
        u32 Data1;
        u16 Data2;
        u16 Data3;
        u8  Data4[8];
} efi_guid_t;

typedef struct {
        u64 Signature;
        u32 Revision;
        u32 HeaderSize;
        u32 CRC32;
        u32 Reserved;
} efi_table_header_t;

typedef struct efi_simple_text_output_protocol_t {
        EFI_STATUS (*Reset)(
                struct efi_simple_text_output_protocol_t *This,
                bool ExtendedVerification
        );
        EFI_STATUS (*OutputString)(
                struct efi_simple_text_output_protocol_t *This,
                CHAR16 *String
        );
} efi_simple_text_output_protocol_t;

typedef struct {
        efi_table_header_t Hdr;
        
        // Task Priority Services
        void *RaiseTPL;
        void *RestoreTPL;
        
        // Memory Services
        void *AllocatePages;
        void *FreePages;
        void *GetMemoryMap;
        void *AllocatePool;
        void *FreePool;

        // Event and Timer Services
        void *CreateEvent;
        void *SetTimer;
        void *WaitForEvent;
        void *SignalEvent;
        void *CloseEvent;
        void *CheckEvent;

        // Protocol Handler Services
        void *InstallProtocolInterface;
        void *ReinstallProtocolInterface;
        void *UninstallProtocolInterface;
        void *HandleProtocol;
        void *Reserved;
        void *RegisterProtocolNotify;
        void *LocateHandle;
        void *LocateDevicePath;
        void *InstallConfigurationTable;

        // Image Services
        void *LoadImage;
        void *StartImage;
        void *Exit;
        void *UnloadImage;
        void *ExitBootServices;

        // Misc. Services
        void *GetNextMonotonicCount;
        void *Stall;
        void *SetWatchdogTimer;

        // DriverSupport Services
        void *ConnectController;
        void *DisconnectController;

        // Open and Close Protocol Services
        void *OpenProtocol;
        void *CloseProtocol;
        void *OpenProtocolInformation;

        //Library Services
        void *ProtocolsPerHandle;
        void *LocateHandleBuffer;
        EFI_STATUS (*LocateProtocol)(
                efi_guid_t *Protocol,
                void       *Registration,
                void       **Interface
        );
        void *InstallMultipleProtocolInterfaces;
        void *UninstallMultipleProtocolInterfaces;
        
        // 32-bit CRC Services
        void *CalculateCRC32;
        
        // Miscellaneous Services 2
        void *CopyMem;
        void *SetMem;
        void *CreateEventEx;

} efi_boot_services_t;

typedef struct {
        // GUID
        efi_guid_t VendorGuid;
        void       *VendorTable;
} efi_cfg_table_t;

typedef struct {
        efi_table_header_t                Hdr;
        CHAR16                            *FirmwareVendor;
        u32                               FirmwareRevision;
        EFI_HANDLE                        ConsoleInHandle;
        void                              *ConIn;
        EFI_HANDLE                        ConsoleOutHandle;
        efi_simple_text_output_protocol_t *ConOut;
        EFI_HANDLE                        *StandardErrorHandle;
        void                              *StdErr;
        void                              *RuntimeServices;
        efi_boot_services_t               *BootServices;
        u64                               NumberOfTableEntries;
        efi_cfg_table_t                   *ConfigurationTable;
} efi_system_table_t;

#endif