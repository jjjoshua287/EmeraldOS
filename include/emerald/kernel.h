#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

/* 
 * This doesn't include everything right now, just things needed for printk.
 * They'll be added later.
*/

void draw_pixel(u64 x, u64 y, u32 color);

#endif