#ifndef EFISTUB_H
#define EFISTUB_H

#include <emerald/efi.h>

extern const efi_system_table_t *efi_system_table;

struct efi_boot_services {
        efi_table_hdr_t Hdr;
        
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
        efi_status_t (*LocateProtocol)(
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
};

#define PIXEL_RGB_RESERVED_8BIT_PER_COLOR	0
#define PIXEL_BGR_RESERVED_8BIT_PER_COLOR	1
#define PIXEL_BIT_MASK                   	2
#define PIXEL_BLT_ONLY                   	3
#define PIXEL_FORMAT_MAX                 	4

typedef struct {
	u32 RedMask;
	u32 GreenMask;
	u32 BlueMask;
	u32 ReservedMask;
} efi_pixel_bitmask_t;

typedef struct {
    u32 Version;
    u32 HorizontalResolution;
    u32 VerticalResolution;
    int PixelFormat;
    efi_pixel_bitmask_t PixelInformation;
    u32 PixelsPerScanLine;
} efi_graphics_output_mode_info_t;

typedef struct {
    u32 MaxMode;
    u32 Mode;
    efi_graphics_output_mode_info_t *Info;
    unsigned long SizeOfInfo;
    efi_phys_addr_t FrameBufferBase;
    unsigned long FrameBufferSize;
} efi_graphics_output_protocol_mode_t;

typedef struct efi_graphics_output_protocol {
    efi_status_t (*QueryMode)(
	struct efi_graphics_output_protocol *This,
        u32 ModeNumber,
        unsigned long *SizeOfInfo,
        efi_graphics_output_mode_info_t **Info
    );

    efi_status_t (*SetMode)(
        struct efi_graphics_output_protocol *This,
        u32 ModeNumber
    );

    void           *Blt;
    efi_graphics_output_protocol_mode_t *Mode;
} efi_graphics_output_protocol_t;

#endif // EFISTUB_H