#include <stdint.h>
#include <emerald/vsprintf.h>

#define is_digit(c) (((c) >= '0') && ((c) <= '9'))

static void reverse_string(char *start, char *end)
{
        while (start < end) {
                char tmp = *start;
                *start++ = *end;
                *end-- = tmp;
        }
}

static void parse_printk_length_modifiers(const char *mod, struct printk_info *info)
{

}

static void parse_printk_width(const char *c, struct printk_info *info, va_list ap)
{
        if (c == '*')
                info->width = va_arg(ap, int);
        else if (!is_digit(c))
                info->width = -1;  /* unused */
        else
        /* This will handle strings later once I am free again */
                info->width = c - 48;
}

static const char *parse_printk_flags(const char *flags, struct printk_info *info)
{
        char c;
        while (c = *flags++) {
                switch (c) {
                case '-':
                        info->left = 1;
                        break;
                case '+':
                        info->showsign = 1;
                        break;
                case ' ':
                        info->space = !info->showsign;
                        break;
                case '#':
                        info->alt = 1;
                        break;
                case '0':
                        info->pad = '0';
                        break;
                default:
                        if (info->left)
                                info->pad = ' ';
                        return;
                }
        }
}

static void parse_printk_info(const char *fmt, struct printk_info *out_info)
{
        if (out_info == NULL)
                return;
}
