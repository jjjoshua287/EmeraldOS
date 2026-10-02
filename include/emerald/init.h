#ifndef EMERALD_INIT_H
#define EMERALD_INIT_H

#ifndef __ASSEMBLER__
#include <emerald/compiler.h>

#define __HEAD __section(".head.text")

#else
#define __HEAD .section ".head.text","ax"
#endif

#endif