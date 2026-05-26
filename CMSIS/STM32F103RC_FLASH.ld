MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
  RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 48K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } >FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM.exidx :
  {
    PROVIDE(__exidx_start = .);
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    PROVIDE(__exidx_end = .);
  } >FLASH

  .preinit_array :
  {
    PROVIDE(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE(__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE(__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE(__fini_array_end = .);
  } >FLASH

  _etext = .;

  .data : AT(_etext)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } >RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } >RAM

  PROVIDE(end = _ebss);
  PROVIDE(_end = _ebss);
}
