#ifndef EMERALD_INIT_H
#define EMERALD_INIT_H

#define BOOT_INFO_FB_BASE 8
#define BOOT_INFO_SIZE 3120

#ifndef __ASSEMBLER__
#include <emerald/compiler.h>
#include <emerald/runtime.h>

#define __HEAD 		__section(".head.text")
#define __HEAD_DATA 	__section(".head.data")

static_assert(sizeof(struct boot_info) == BOOT_INFO_SIZE,
	"update BOOT_INFO_SIZE (used by head_64.S)");

static_assert(offsetof(struct boot_info, info.lfb_base) == BOOT_INFO_FB_BASE,
    "update BOOT_INFO_FB_BASE (used by head_64.S)");

#else
#define __HEAD 		.section ".head.text", "ax"
#define __HEAD_DATA 	.section ".head.data", "aw", @progbits
#endif

#endif