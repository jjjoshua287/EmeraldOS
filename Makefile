CC     = clang
LINKER = lld-link

CFLAGS = -target x86_64-pc-win32 \
         -std=c23 \
         -ffreestanding \
         -fshort-wchar \
         -mno-red-zone \
         -I $(shell pwd)/include/ \
         -c

LDFLAGS = /subsystem:efi_application \
          /entry:efi_main \
          /nodefaultlib

TARGET = BOOTX64.EFI
ESP    = esp/EFI/BOOT

# Initialize global lists to populate via sub-makefiles
SRCS :=
SUBDIRS = kernel lib drivers

include kernel/Makefile
include lib/Makefile
include drivers/Makefile

OBJS := $(SRCS:.c=.o)

all: $(TARGET) $(ESP)/$(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) /out:$(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ESP)/$(TARGET): $(TARGET)
	mkdir -p $(ESP)
	cp $(TARGET) $(ESP)/$(TARGET)

run: all
	qemu-system-x86_64 \
  		-bios /usr/share/edk2/ovmf/OVMF_CODE.fd \
  		-drive format=raw,file=fat:rw:esp/ \
  		-vga std \
  		-net none

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf esp/

.PHONY: all run clean $(SUBDIRS)