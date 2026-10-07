VERSION = 0.0.2
GIT_HASH = $(shell git rev-parse --short HEAD)

# Configuration
ARCH = x86_64
ARCH_DIR = arch/$(ARCH)
TRIPLE = x86_64-elf
CROSS_CC = /opt/cross/bin

# Directories
BUILD = build
ESP = esp/EFI/BOOT

# Sub-makefiles could define rules; keep `all` as the default goal.
.DEFAULT_GOAL := all

# Shared Compiler Flags
# -MMD -MP emit .d files so editing a header rebuilds what includes it.
CFLAGS = -std=gnu23 -ffreestanding -fno-omit-frame-pointer -mno-red-zone \
		 -isystem $(CURDIR)/include -isystem $(CURDIR)/$(ARCH_DIR)/include -MMD -MP -g

# --- EFI Phase (stub: clang, MS ABI, PE/COFF) ---
EFI_CC = clang
EFI_LINKER = lld-link

EFI_CFLAGS := -target x86_64-pc-win32 \
			  -fshort-wchar \
			  -Wno-msvc-not-found
EFI_CFLAGS += $(CFLAGS)

EFI_LDFLAGS = /subsystem:efi_application \
			  /entry:efi_main \
			  /nodefaultlib
EFI_TARGET = BOOTX64.EFI
EFI_BUILD = $(BUILD)/efi

EFI_SRCS := lib/string.c $(ARCH_DIR)/lib/e820_table.c
include drivers/efi/Makefile

EFI_OBJS := $(addprefix $(EFI_BUILD)/,$(patsubst %.S,%.o,$(EFI_SRCS:.c=.o)))

# --- ELF Kernel (cross gcc, SysV ABI) ---
KERNEL_CC = $(CROSS_CC)/$(TRIPLE)-gcc
KERNEL_LINKER = $(CROSS_CC)/$(TRIPLE)-ld

KERNEL_TARGET = emeraldos.elf
KERNEL_BUILD = $(BUILD)/kernel

KERNEL_CFLAGS := $(CFLAGS)
KERNEL_CFLAGS += -DCFG_INIT_FBCON_EARLY
KERNEL_CFLAGS += -mgeneral-regs-only -fno-stack-protector
KERNEL_CFLAGS += -mcmodel=kernel -fno-pic

KERNEL_LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

# Initialize global lists to populate via sub-makefiles
SRCS :=

include $(ARCH_DIR)/Makefile
include kernel/Makefile
include lib/Makefile
include drivers/Makefile

# compute kernel object files from the collected sources
KERNEL_OBJS := $(addprefix $(KERNEL_BUILD)/,$(patsubst %.S,%.o,$(SRCS:.c=.o)))

# --- Rules ---
.PHONY: all clean run

all: $(EFI_TARGET)

# Final image: stub objects plus the embedded kernel blob.
$(EFI_TARGET): $(EFI_OBJS)
	$(EFI_LINKER) $(EFI_LDFLAGS) /out:$@ $^

# The blob object embeds emeraldos.elf via .incbin. Make can't see that
# dependency on its own, so state it here. This is what forces the kernel
# to be built (and rebuilt) before the stub is linked.
$(EFI_BUILD)/drivers/efi/kernel_blob.o: $(KERNEL_TARGET)
$(EFI_BUILD)/drivers/efi/kernel_blob.o: EFI_CFLAGS += -I$(CURDIR)

$(KERNEL_TARGET): $(KERNEL_OBJS) linker.ld
	$(KERNEL_LINKER) $(KERNEL_LDFLAGS) -o $@ $(KERNEL_OBJS)

# Stub objects: clang, MS ABI
$(EFI_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(EFI_CC) $(EFI_CFLAGS) -c $< -o $@

$(EFI_BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(EFI_CC) $(EFI_CFLAGS) -c $< -o $@

# Kernel objects: cross gcc, SysV ABI
$(KERNEL_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CFLAGS) -c $< -o $@

$(KERNEL_BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CFLAGS) -c $< -o $@

esp: all
	mkdir -p $(ESP)
	cp $(EFI_TARGET) $(ESP)/

run: esp
	qemu-system-x86_64	\
		-bios /usr/share/edk2/ovmf/OVMF_CODE.fd	\
		-drive format=raw,file=fat:rw:esp/	\
		-vga std -net none

debug: esp
	qemu-system-x86_64	\
		-bios /usr/share/edk2/ovmf/OVMF_CODE.fd	\
		-drive format=raw,file=fat:rw:esp/	\
		-vga std -net none -d int,cpu_reset -no-reboot -D qemu.log

clean:
	rm -rf $(BUILD) $(KERNEL_TARGET) $(EFI_TARGET)

-include $(EFI_OBJS:.o=.d) $(KERNEL_OBJS:.o=.d)