#include <asm/gdt.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/segment.h>

static struct desc_struct gdt[GDT_ENTRIES] __aligned(16);
static struct x86_hw_tss tss;
static u8 ist1_stack[4096] __aligned(16);	/* for #DF, will be done later */

/* Initalize the TSS as an entry of the GDT */
void tss_init(void)
{
	memset(&tss, 0, sizeof(tss));
	tss.ist[0] = (u64)(ist1_stack + sizeof(ist1_stack));
	tss.iomap_base = sizeof(struct x86_hw_tss);
	set_tss_desc(gdt, &tss);
}

/* Initalize all the entries of the GDT */
static void gdt_init(void)
{
	struct desc_struct descs[GDT_ENTRIES];
	memset(&gdt[GDT_ENTRY_NULL], 0, sizeof(struct desc_struct));
	
	descs[GDT_ENTRY_KERNEL_CS] = (struct desc_struct)GDT_ENTRY_INIT(DESC_CODE64, 0, 0xFFFFF);
	descs[GDT_ENTRY_KERNEL_DS] = (struct desc_struct)GDT_ENTRY_INIT(DESC_DATA64, 0, 0xFFFFF);
	
	descs[GDT_ENTRY_USER_CS] = (struct desc_struct)GDT_ENTRY_INIT(DESC_CODE64 | DESC_USER, 0, 0xFFFFF);
	descs[GDT_ENTRY_USER_DS] = (struct desc_struct)GDT_ENTRY_INIT(DESC_DATA64 | DESC_USER, 0, 0xFFFFF);

	/* Skip null entry, as it's already been written to GDT */
	for (int i = 1; i < GDT_ENTRY_TSS; i++)
		write_gdt_entry(gdt, i, &descs[i], 0);
	tss_init();
}

/* Setup the GDT */
void setup_gdt(void)
{
        /* Disable Interrupts */
        asm volatile("cli");

	gdt_init();

	struct desc_ptr gdtr = {
		.size = sizeof(gdt) - 1,
		.addr = (u64)gdt,
	};
	load_gdt(&gdtr);
        reload_segments();
        int tss_seg = __TSS_SEL;
	load_tr(tss_seg);
}