  .section .rdata,"dr"
  .global kernel_elf_start
  .global kernel_elf_end
  kernel_elf_start:
      .incbin "emeraldos.elf"
  kernel_elf_end: