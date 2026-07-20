#include <asm/desc_defs.h>
#include <asm/desc.h>
#include <asm/exceptions.h>
#include <asm/segment.h>
#include <asm/idtentry.h>

#include <emerald/types.h>

enum {
        DPL0 = 0x0,
        DPL3 = 0x3
};

#define DEFAULT_STACK 0

#define G(_vector, _addr, _ist, _type, _dpl, _segment)  \
        {                                               \
                .vector    = _vector,                   \
                .segment   = _segment,                  \
                .addr      = _addr,                     \
                .bits.ist  = _ist,                      \
                .bits.type = _type,                     \
                .bits.dpl  = _dpl,                      \
                .bits.p    = 1,                         \
        }

/* Standard Interrupt Gate */
#define INTG(_vector, _addr)                            \
        G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL0, __KERNEL_CS)

/* System Interrupt Gate */
#define SYSG(_vector, _addr)                            \
        G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL3, __KERNEL_CS)

/* Interrupt Gate with Interrupt Stack, The _ist index is the index in the
 * tss.ist[] array. The descriptor needs it to start at 1.
 */
#define ISTG(_vector, _addr, _ist)                      \
        G(_vector, _addr, _ist + 1, GATE_INTERRUPT, DPL0, __KERNEL_CS)

#define IDT_TABLE_SIZE (IDT_ENTRIES * sizeof(gate_desc))

static const struct idt_data def_idt[] = {
        INTG(EXC_DE,            asm_exc_division_error),
        ISTG(EXC_DB,            asm_exc_debug, IST_INDEX_DB),
        ISTG(EXC_NMI,           asm_exc_nmi, IST_INDEX_NMI),
        SYSG(EXC_BP,            asm_exc_breakpoint),
        SYSG(EXC_OF,            asm_exc_overflow),
        INTG(EXC_BR,            asm_exc_bound_range_exceeded),
        INTG(EXC_UD,            asm_exc_invalid_opcode),
        INTG(EXC_NM,            asm_exc_device_not_available),
        ISTG(EXC_DF,            asm_exc_double_fault, IST_INDEX_DF),
        INTG(EXC_OLD_MP,        asm_exc_coproc_segment_overrun),
        INTG(EXC_TS,            asm_exc_invalid_tss),
        INTG(EXC_NP,            asm_exc_segment_not_present),
        INTG(EXC_SS,            asm_exc_stack_segment_fault),
        INTG(EXC_GP,            asm_exc_general_protection_fault),
        INTG(EXC_PF,            asm_exc_page_fault),
        INTG(EXC_SPURIOUS,      asm_exc_reserved_vector_15),
        INTG(EXC_MF,            asm_exc_x87_float_point_exception),
        INTG(EXC_AC,            asm_exc_alignment_check),
        ISTG(EXC_MC,            asm_exc_machine_check, IST_INDEX_MC),
        INTG(EXC_XM,            asm_exc_simd_float_point_exception),
        INTG(EXC_VE,            asm_exc_virtualization_exception),
        INTG(EXC_CP,            asm_exc_control_protection_exception),
        INTG(22,                asm_exc_reserved_vector_22),
        INTG(23,                asm_exc_reserved_vector_23),
        INTG(24,                asm_exc_reserved_vector_24),
        INTG(25,                asm_exc_reserved_vector_25),
        INTG(26,                asm_exc_reserved_vector_26),
        INTG(27,                asm_exc_reserved_vector_27),
        INTG(EXC_HV,            asm_exc_hypervisor_injection_exception),
        INTG(EXC_VC,            asm_exc_vmm_communication_exception),
        INTG(EXC_SX,            asm_exc_security_exception),
};

gate_desc idt_table[IDT_ENTRIES];

struct desc_ptr idt_desc = {
        .size = IDT_TABLE_SIZE - 1,
        .addr = (u64)idt_table
};

void invalidate_idt(void)
{
    static const struct desc_ptr idt = {
        .size = 0,
        .addr = 0
    };
    load_idt(&idt);
}