/**
 ******************************************************************************
 * @file      startup_sprinter.s
 * @author    Steven Mu
 * @summary   SprinterOS kernel startup. The bootloader loads this image at
 *            ORIGIN(KERNEL_IMG) and branches to Reset_Handler, so there is no
 *            .data copy to do -- LMA and VMA are the same.
 ******************************************************************************
 */

  .syntax unified
  .cpu cortex-m7
  .fpu fpv5-sp-d16
  .thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler

/* vector table -- must be first in the image, VTOR points here */
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word Default_Handler       /* NMI          */
  .word Default_Handler       /* HardFault    */
  .word Default_Handler       /* MemManage    */
  .word Default_Handler       /* BusFault     */
  .word Default_Handler       /* UsageFault   */
  .word 0
  .word 0
  .word 0
  .word 0
  .word Default_Handler       /* SVCall       */
  .word Default_Handler       /* DebugMon     */
  .word 0
  .word Default_Handler       /* PendSV       */
  .word Default_Handler       /* SysTick      */
  .size g_pfnVectors, .-g_pfnVectors

  .section .text.Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack          /* bootloader already set MSP, do it again anyway */

  /* zero .bss */
  ldr   r0, =_sbss
  ldr   r1, =_ebss
  movs  r2, #0
bss_loop:
  cmp   r0, r1
  bcs   bss_done
  str   r2, [r0], #4
  b     bss_loop
bss_done:

  bl    _main

hang:
  b     hang
  .size Reset_Handler, .-Reset_Handler

  .section .text.Default_Handler
  .type Default_Handler, %function
Default_Handler:
  b     Default_Handler
  .size Default_Handler, .-Default_Handler
