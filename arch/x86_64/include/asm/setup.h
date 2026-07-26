#ifndef X86_64_SETUP_H
#define X86_64_SETUP_H

/* Assembly stub to reload segment selectors. */
extern void reload_segments(void);

void setup_gdt(void);
void setup_idt(void);

#endif /* X86_64_SETUP_H */