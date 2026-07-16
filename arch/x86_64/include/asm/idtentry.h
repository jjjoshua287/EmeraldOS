#ifndef X86_64_IDT_ENTRY_H
#define X86_64_IDT_ENTRY_H

#include <asm/segment.h>
#include <emerald/types.h>

/* helper here until IRQs are fully implemented */
#define FIRST_UNNAMED_STUB_VEC 32
#define DECLARE_IDTENTRY(name) void name(void)

/* IDT Entry prototypes */

/* These have no error codes */
DECLARE_IDTENTRY(asm_isr_divide_error);
DECLARE_IDTENTRY(asm_isr_debug);
DECLARE_IDTENTRY(asm_isr_nmi);
DECLARE_IDTENTRY(asm_isr_breakpoint);
DECLARE_IDTENTRY(asm_isr_overflow);
DECLARE_IDTENTRY(asm_isr_bound_range);
DECLARE_IDTENTRY(asm_isr_invalid_opcode);
DECLARE_IDTENTRY(asm_isr_device_not_available);

/* error code of 0 */
DECLARE_IDTENTRY(asm_isr_double_fault);

/* fault used in old hardware that has no error code */
DECLARE_IDTENTRY(asm_isr_coprocessor_segment);

/* These all have an error code */
DECLARE_IDTENTRY(asm_isr_invalid_tss);
DECLARE_IDTENTRY(asm_isr_segment_not_present);
DECLARE_IDTENTRY(asm_isr_stack_segment);
DECLARE_IDTENTRY(asm_isr_general_protection);
DECLARE_IDTENTRY(asm_isr_page_fault);

/* Control Protection Fault and Alignment Check have error codes, the rest do not */
DECLARE_IDTENTRY(asm_isr_x87_fpe);
DECLARE_IDTENTRY(asm_isr_alignment_check);
DECLARE_IDTENTRY(asm_isr_machine_check);
DECLARE_IDTENTRY(asm_isr_simd_fpe);
DECLARE_IDTENTRY(asm_isr_virtualization);
DECLARE_IDTENTRY(asm_isr_control_protection);

/* other ones */
DECLARE_IDTENTRY(asm_isr_hypervisor_injection);
DECLARE_IDTENTRY(asm_isr_vmm_comms_exception);
DECLARE_IDTENTRY(asm_isr_security_exception);

#endif