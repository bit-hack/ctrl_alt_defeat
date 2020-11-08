OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x00001000;
  .text : { *(.text) }
  . = ALIGN(0x4);
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = 0x00001fff;
  _end = .;
}
