#include <asm/ptrace.h>
#include <asm/except_pf.h>
#include <asm/processor.h>

#include <emerald/fbcon.h>
#include <emerald/vsprintf.h>
#include <emerald/printk.h>
#include <emerald/panic.h>

static void print_fault_reason(unsigned long error_code, bool user)
{
        printk("#PF: %s-privileged %s from %s code\n",
		(error_code & X86_PF_USER) ? "user" : "supervisor",
		(error_code & X86_PF_INSTR) ? "instruction fetch" :
		(error_code & X86_PF_WRITE) ? "write access" : "read access",
		(user) ? "user" : "kernel");
	
	printk("#PF: error_code(0x%04lx) - %s\n", error_code,
		(!(error_code & X86_PF_PROT)) ? "not-persent page" :
		(error_code & X86_PF_RSVD) ? "reserved bit violation" :
		(error_code & X86_PF_PK) ? "protection keys violation" : 
						"permissions violation");
}

void handle_page_fault(struct pt_regs *regs)
{
        unsigned long cr2 = native_read_cr2();
        /* 
         * this would do alot more things, but there currently isn't any page mapper,
         * so treat every exception as fatal.
         */

	fbcon_clear();

	char buf[64];
	snprintf(buf, sizeof(buf), "Unable to handle page fault for address: 0x%016llx", cr2); 
        print_fault_reason(regs->error_code, user_mode(regs));
	panic(buf, regs);
}