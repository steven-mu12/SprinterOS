/*
 ******************************************************************************
 * @file      sprinter.ld
 * @author    Steven Mu
 * @summary   SprinterOS kernel linker script
 ******************************************************************************
 */

ENTRY(Reset_Handler)

INCLUDE memmap.ld

_dtcm_start      = ORIGIN(DTCM);
_dtcm_end        = ORIGIN(DTCM) + LENGTH(DTCM);
_userspace_start = ORIGIN(USERSPACE);
_userspace_end   = ORIGIN(USERSPACE) + LENGTH(USERSPACE);
_os_load_addr    = ORIGIN(KERNEL_IMG);
_os_load_end     = ORIGIN(KERNEL_IMG) + LENGTH(KERNEL_IMG);

_kernel_stack_size = 0x10000;
_estack            = _dtcm_end;

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >KERNEL_IMG

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } >KERNEL_IMG

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >KERNEL_IMG

  /* zeroed by Reset_Handler, so it is not part of the image */
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >DTCM

  /* however big the above is, user heap starts at _end */
  . = ALIGN(8);
  PROVIDE ( end  = . );
  PROVIDE ( _end = . );

  /DISCARD/ :
  {
    *(.ARM.exidx*)
    *(.eh_frame)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
