#ifndef X86_64_SEGMENT_H
#define X86_64_SEGMENT_H

/* 
 * The current layout of the GDT for EmeraldOS:
 *
 * 0 - null descriptor
 * 1 - kernel code segment
 * 2 - kernel data segment
 * 3 - user code segment
 * 4 - user data segment
 * 5 - TSS (takes 2 slots) 
*/

#define GDT_ENTRY_NULL      0
#define GDT_ENTRY_KERNEL_CS 1
#define GDT_ENTRY_KERNEL_DS 2
#define GDT_ENTRY_USER_CS   3
#define GDT_ENTRY_USER_DS   4

/* 64-bit TSS requires 2 entries */
#define GDT_ENTRY_TSS       5

/* Number of entries in the GDT Table */
#define GDT_ENTRIES         7
#define GDT_SIZE            (GDT_ENTRIES * 8)

/* 
 * Segment Selector values corresponding to the above entries
 * 
 * Note, selectors should have a correct RPL,
 * indicated by a bitwise OR for user-space selectors
 */ 
#define __KERNEL_CS         (GDT_ENTRY_KERNEL_CS << 3)
#define __KERNEL_DS         (GDT_ENTRY_KERNEL_DS << 3)
#define __USER_CS           ((GDT_ENTRY_USER_CS << 3) | 3)
#define __USER_DS           ((GDT_ENTRY_USER_DS << 3) | 3)
#define __TSS_SEL           (GDT_ENTRY_TSS << 3)

/* Number of entries in Interrupt Descriptor Table */
#define IDT_ENTRIES 256

/* Interrupt Stack Table slots */
#define IST_INDEX_DF    0
#define IST_INDEX_NMI   1
#define IST_INDEX_DB    2
#define IST_INDEX_MC    3
#define IST_INDEX_VC    4

#endif /* X86_64_SEGMENT_H */