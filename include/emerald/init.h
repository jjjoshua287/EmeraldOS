#ifndef EMERALD_INIT_H
#define EMERALD_INIT_H

#define BOOT_INFO_FB_BASE 8

#ifndef __ASSEMBLER__
#include <emerald/compiler.h>
#include <emerald/runtime.h>

#define __HEAD __section(".head.text")

static_assert(offsetof(struct boot_info, info.lfb_base) == BOOT_INFO_FB_BASE,
    "update BOOT_INFO_FB_BASE (used by head_64.S)");

#else
#define __HEAD .section ".head.text","ax"
#endif

#endif