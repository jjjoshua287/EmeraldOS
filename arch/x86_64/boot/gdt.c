#include <asm/setup.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/segment.h>

#define IST_STACK_SIZE 4096

static struct desc_struct gdt[GDT_ENTRIES] __aligned(16);

static struct x86_hw_tss tss;
static u8 df_ist_stack[IST_STACK_SIZE] 	__aligned(16);
static u8 nmi_ist_stack[IST_STACK_SIZE]	__aligned(16);
static u8 db_ist_stack[IST_STACK_SIZE]	__aligned(16);
static u8 mc_ist_stack[IST_STACK_SIZE]	__aligned(16);
/* TODO: Make IST Stack for #VC (VMM Communication Exc.) */

/* Initalize the TSS as an entry of the GDT */
void tss_init(void)
{
	/* TODO: rsp0 once usermode exists */
	memset(&tss, 0, sizeof(tss));
	tss.ist[IST_INDEX_DF] = (u64)(df_ist_stack + sizeof(df_ist_stack));
	tss.ist[IST_INDEX_NMI] = (u64)(nmi_ist_stack + sizeof(nmi_ist_stack));
	tss.ist[IST_INDEX_DB] = (u64)(db_ist_stack + sizeof(db_ist_stack));
	tss.ist[IST_INDEX_MC] = (u64)(mc_ist_stack + sizeof(mc_ist_stack));
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