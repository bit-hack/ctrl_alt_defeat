.extern main

.section ".text","ax",@progbits
.globl _start
_start:
  nop
  li sp, 0x1ffc
  j main
