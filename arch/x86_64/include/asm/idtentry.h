#ifndef X86_64_IDT_ENTRY_H
#define X86_64_IDT_ENTRY_H

#define DECLARE_IDTENTRY(name) void name(void)

/* IDT Entry prototypes */

/* These have no error codes */
DECLARE_IDTENTRY(asm_exc_divide_error);
DECLARE_IDTENTRY(asm_exc_debug);
DECLARE_IDTENTRY(asm_exc_nmi);
DECLARE_IDTENTRY(asm_exc_breakpoint);
DECLARE_IDTENTRY(asm_exc_overflow);
DECLARE_IDTENTRY(asm_exc_bound_range);
DECLARE_IDTENTRY(asm_exc_invalid_opcode);
DECLARE_IDTENTRY(asm_exc_device_not_available);

/* error code of 0 */
DECLARE_IDTENTRY(asm_exc_double_fault);

/* fault used in old hardware that has no error code */
DECLARE_IDTENTRY(asm_exc_coprocessor_segment);

/* These all have an error code */
DECLARE_IDTENTRY(asm_exc_invalid_tss);
DECLARE_IDTENTRY(asm_exc_segment_not_present);
DECLARE_IDTENTRY(asm_exc_stack_segment);
DECLARE_IDTENTRY(asm_exc_general_protection);
DECLARE_IDTENTRY(asm_exc_page_fault);

/* Control Protection Fault and Alignment Check have error codes, the rest do not */
DECLARE_IDTENTRY(asm_exc_x87_fpe);
DECLARE_IDTENTRY(asm_exc_alignment_check);
DECLARE_IDTENTRY(asm_exc_machine_check);
DECLARE_IDTENTRY(asm_exc_simd_fpe);
DECLARE_IDTENTRY(asm_exc_virtualization);
DECLARE_IDTENTRY(asm_exc_control_protection);

/* other ones */
DECLARE_IDTENTRY(asm_exc_hypervisor_injection);
DECLARE_IDTENTRY(asm_exc_vmm_comms_exception);
DECLARE_IDTENTRY(asm_exc_security_exception);

/* Intel Reserved Vectors */
DECLARE_IDTENTRY(asm_exc_reserved_vector_15);
DECLARE_IDTENTRY(asm_exc_reserved_vector_22);
DECLARE_IDTENTRY(asm_exc_reserved_vector_23);
DECLARE_IDTENTRY(asm_exc_reserved_vector_24);
DECLARE_IDTENTRY(asm_exc_reserved_vector_25);
DECLARE_IDTENTRY(asm_exc_reserved_vector_26);
DECLARE_IDTENTRY(asm_exc_reserved_vector_27);
DECLARE_IDTENTRY(asm_exc_reserved_vector_31);

#endif // X86_64_IDT_ENTRY_H