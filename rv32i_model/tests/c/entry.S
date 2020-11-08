.extern main

.section ".text","ax",@progbits
.globl _start
_start:
  la sp, 0xffffffff
  j main
