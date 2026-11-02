#include <asm/pgtable.h>

/* bits supported by the hardware */
pteval_t __supported_pte_mask = ~0;
/* bits allowed in normal kernel mappings */
pteval_t __default_kernel_pte_mask = ~0;