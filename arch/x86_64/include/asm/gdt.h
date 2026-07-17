#ifndef X86_64_GDT_H
#define X86_64_GDT_H

/* Assembly stub to reload segment selectors. */
extern void reload_segments(void);

void setup_gdt(void);

#endif /* X86_64_GDT_H */