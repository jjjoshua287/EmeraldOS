#include <emerald/container_of.h>

#include "fonts.h"

static const struct font_desc *fonts[] = {
        &font_vga_8x16,
};

const struct font_data *to_font_data_struct(font_data_t *fd)
{
        return container_of_const(fd, struct font_data, data[0]);
}

const struct font_desc *get_default_font()
{
        // this is technically a stub due to only 1 font, will be expanded later
        return fonts[0];
}

#define num_fonts 1