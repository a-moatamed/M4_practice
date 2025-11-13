ENTRY(_reset);

MEMORY {
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024k
  SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 96k
};
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS {
  .vectors : { KEEP(*(.vectors)) } > FLASH
  .text : { *(.text*) } > FLASH
  .rodata : { *(.rodata*) } > FLASH
  .data : {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > SRAM AT > FLASH
  _sidata = LOADADDR(.data);
  .bss : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > SRAM
}
