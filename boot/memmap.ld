/* Memories definition */
MEMORY
{
  FLASH       (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCM        (xrw) : ORIGIN = 0x20000000, LENGTH = 128K    /* DTCM, kernelspace after bootloader */
  USERSPACE   (xrw) : ORIGIN = 0x20020000, LENGTH = 368K    /* SRAM1, userspace */
  KERNEL_IMG  (xrw) : ORIGIN = 0x2007C000, LENGTH = 16K     /* SRAM2, kernel image */
}
