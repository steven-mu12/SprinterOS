  .syntax unified
  .cpu cortex-m7
  .fpu fpv5-sp-d16
  .thumb

  .global PendSV_Handler

  .equ TCB_SP_OFFSET, 24

  .section .text.PendSV_Handler
  .type PendSV_Handler, %function
PendSV_Handler:
  mrs   r0, psp                 /* outgoing task's stack, hardware frame on top */
  isb

  ldr   r2, =current_task
  ldr   r1, [r2]
  cbz   r1, load_next           /* nothing running yet, so nothing to save */

  tst   lr, #0x10
  it    eq
  vstmdbeq r0!, {s16-s31}

  stmdb r0!, {r4-r11, lr}
  str   r0, [r1, #TCB_SP_OFFSET]

load_next:
  ldr   r3, =next_task
  ldr   r1, [r3]
  cbz   r1, no_switch           /* nobody to switch to, leave psp alone */

  str   r1, [r2]                /* current_task = next_task */
  ldr   r0, [r1, #TCB_SP_OFFSET]

  /* lr comes back off the task's own stack, so the fp test below is its own */
  ldmia r0!, {r4-r11, lr}

  tst   lr, #0x10
  it    eq
  vldmiaeq r0!, {s16-s31}

  msr   psp, r0
  isb

no_switch:
  bx    lr
  .size PendSV_Handler, .-PendSV_Handler

  .ltorg
