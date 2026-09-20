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

/* vector table -- must be first in the image, VTOR points here. after the core
   exceptions the entries are irq 0..109 in RM0410 table 46 order */
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler
  .word WWDG_IRQHandler
  .word PVD_IRQHandler
  .word TAMP_STAMP_IRQHandler
  .word RTC_WKUP_IRQHandler
  .word FLASH_IRQHandler
  .word RCC_IRQHandler
  .word EXTI0_IRQHandler
  .word EXTI1_IRQHandler
  .word EXTI2_IRQHandler
  .word EXTI3_IRQHandler
  .word EXTI4_IRQHandler
  .word DMA1_Stream0_IRQHandler
  .word DMA1_Stream1_IRQHandler
  .word DMA1_Stream2_IRQHandler
  .word DMA1_Stream3_IRQHandler
  .word DMA1_Stream4_IRQHandler
  .word DMA1_Stream5_IRQHandler
  .word DMA1_Stream6_IRQHandler
  .word ADC_IRQHandler
  .word CAN1_TX_IRQHandler
  .word CAN1_RX0_IRQHandler
  .word CAN1_RX1_IRQHandler
  .word CAN1_SCE_IRQHandler
  .word EXTI9_5_IRQHandler
  .word TIM1_BRK_TIM9_IRQHandler
  .word TIM1_UP_TIM10_IRQHandler
  .word TIM1_TRG_COM_TIM11_IRQHandler
  .word TIM1_CC_IRQHandler
  .word TIM2_IRQHandler
  .word TIM3_IRQHandler
  .word TIM4_IRQHandler
  .word I2C1_EV_IRQHandler
  .word I2C1_ER_IRQHandler
  .word I2C2_EV_IRQHandler
  .word I2C2_ER_IRQHandler
  .word SPI1_IRQHandler
  .word SPI2_IRQHandler
  .word USART1_IRQHandler
  .word USART2_IRQHandler
  .word USART3_IRQHandler
  .word EXTI15_10_IRQHandler
  .word RTC_ALARM_IRQHandler
  .word OTG_FS_WKUP_IRQHandler
  .word TIM8_BRK_TIM12_IRQHandler
  .word TIM8_UP_TIM13_IRQHandler
  .word TIM8_TRG_COM_TIM14_IRQHandler
  .word TIM8_CC_IRQHandler
  .word DMA1_Stream7_IRQHandler
  .word FMC_IRQHandler
  .word SDMMC1_IRQHandler
  .word TIM5_IRQHandler
  .word SPI3_IRQHandler
  .word UART4_IRQHandler
  .word UART5_IRQHandler
  .word TIM6_DAC_IRQHandler
  .word TIM7_IRQHandler
  .word DMA2_Stream0_IRQHandler
  .word DMA2_Stream1_IRQHandler
  .word DMA2_Stream2_IRQHandler
  .word DMA2_Stream3_IRQHandler
  .word DMA2_Stream4_IRQHandler
  .word ETH_IRQHandler
  .word ETH_WKUP_IRQHandler
  .word CAN2_TX_IRQHandler
  .word CAN2_RX0_IRQHandler
  .word CAN2_RX1_IRQHandler
  .word CAN2_SCE_IRQHandler
  .word OTG_FS_IRQHandler
  .word DMA2_Stream5_IRQHandler
  .word DMA2_Stream6_IRQHandler
  .word DMA2_Stream7_IRQHandler
  .word USART6_IRQHandler
  .word I2C3_EV_IRQHandler
  .word I2C3_ER_IRQHandler
  .word OTG_HS_EP1_OUT_IRQHandler
  .word OTG_HS_EP1_IN_IRQHandler
  .word OTG_HS_WKUP_IRQHandler
  .word OTG_HS_IRQHandler
  .word DCMI_IRQHandler
  .word CRYP_IRQHandler
  .word HASH_RNG_IRQHandler
  .word FPU_IRQHandler
  .word UART7_IRQHandler
  .word UART8_IRQHandler
  .word SPI4_IRQHandler
  .word SPI5_IRQHandler
  .word SPI6_IRQHandler
  .word SAI1_IRQHandler
  .word LCD_TFT_IRQHandler
  .word LCD_TFT_1_IRQHandler
  .word DMA2D_IRQHandler
  .word SAI2_IRQHandler
  .word QuadSPI_IRQHandler
  .word LP_Timer1_IRQHandler
  .word 0
  .word I2C4_EV_IRQHandler
  .word I2C4_ER_IRQHandler
  .word SPDIFRX_IRQHandler
  .word DSIHOST_IRQHandler
  .word DFSDM1_FLT0_IRQHandler
  .word DFSDM1_FLT1_IRQHandler
  .word DFSDM1_FLT2_IRQHandler
  .word DFSDM1_FLT3_IRQHandler
  .word SDMMC2_IRQHandler
  .word CAN3_TX_IRQHandler
  .word CAN3_RX0_IRQHandler
  .word CAN3_RX1_IRQHandler
  .word CAN3_SCE_IRQHandler
  .word JPEG_IRQHandler
  .word MDIOS_IRQHandler
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

/* every handler above is a weak alias of Default_Handler, so defining a C
   function with the matching name takes over that slot at link time */
  .macro weak_handler name
  .weak \name
  .thumb_set \name, Default_Handler
  .endm

  weak_handler NMI_Handler
  weak_handler HardFault_Handler
  weak_handler MemManage_Handler
  weak_handler BusFault_Handler
  weak_handler UsageFault_Handler
  weak_handler SVC_Handler
  weak_handler DebugMon_Handler
  weak_handler PendSV_Handler
  weak_handler SysTick_Handler
  weak_handler WWDG_IRQHandler
  weak_handler PVD_IRQHandler
  weak_handler TAMP_STAMP_IRQHandler
  weak_handler RTC_WKUP_IRQHandler
  weak_handler FLASH_IRQHandler
  weak_handler RCC_IRQHandler
  weak_handler EXTI0_IRQHandler
  weak_handler EXTI1_IRQHandler
  weak_handler EXTI2_IRQHandler
  weak_handler EXTI3_IRQHandler
  weak_handler EXTI4_IRQHandler
  weak_handler DMA1_Stream0_IRQHandler
  weak_handler DMA1_Stream1_IRQHandler
  weak_handler DMA1_Stream2_IRQHandler
  weak_handler DMA1_Stream3_IRQHandler
  weak_handler DMA1_Stream4_IRQHandler
  weak_handler DMA1_Stream5_IRQHandler
  weak_handler DMA1_Stream6_IRQHandler
  weak_handler ADC_IRQHandler
  weak_handler CAN1_TX_IRQHandler
  weak_handler CAN1_RX0_IRQHandler
  weak_handler CAN1_RX1_IRQHandler
  weak_handler CAN1_SCE_IRQHandler
  weak_handler EXTI9_5_IRQHandler
  weak_handler TIM1_BRK_TIM9_IRQHandler
  weak_handler TIM1_UP_TIM10_IRQHandler
  weak_handler TIM1_TRG_COM_TIM11_IRQHandler
  weak_handler TIM1_CC_IRQHandler
  weak_handler TIM2_IRQHandler
  weak_handler TIM3_IRQHandler
  weak_handler TIM4_IRQHandler
  weak_handler I2C1_EV_IRQHandler
  weak_handler I2C1_ER_IRQHandler
  weak_handler I2C2_EV_IRQHandler
  weak_handler I2C2_ER_IRQHandler
  weak_handler SPI1_IRQHandler
  weak_handler SPI2_IRQHandler
  weak_handler USART1_IRQHandler
  weak_handler USART2_IRQHandler
  weak_handler USART3_IRQHandler
  weak_handler EXTI15_10_IRQHandler
  weak_handler RTC_ALARM_IRQHandler
  weak_handler OTG_FS_WKUP_IRQHandler
  weak_handler TIM8_BRK_TIM12_IRQHandler
  weak_handler TIM8_UP_TIM13_IRQHandler
  weak_handler TIM8_TRG_COM_TIM14_IRQHandler
  weak_handler TIM8_CC_IRQHandler
  weak_handler DMA1_Stream7_IRQHandler
  weak_handler FMC_IRQHandler
  weak_handler SDMMC1_IRQHandler
  weak_handler TIM5_IRQHandler
  weak_handler SPI3_IRQHandler
  weak_handler UART4_IRQHandler
  weak_handler UART5_IRQHandler
  weak_handler TIM6_DAC_IRQHandler
  weak_handler TIM7_IRQHandler
  weak_handler DMA2_Stream0_IRQHandler
  weak_handler DMA2_Stream1_IRQHandler
  weak_handler DMA2_Stream2_IRQHandler
  weak_handler DMA2_Stream3_IRQHandler
  weak_handler DMA2_Stream4_IRQHandler
  weak_handler ETH_IRQHandler
  weak_handler ETH_WKUP_IRQHandler
  weak_handler CAN2_TX_IRQHandler
  weak_handler CAN2_RX0_IRQHandler
  weak_handler CAN2_RX1_IRQHandler
  weak_handler CAN2_SCE_IRQHandler
  weak_handler OTG_FS_IRQHandler
  weak_handler DMA2_Stream5_IRQHandler
  weak_handler DMA2_Stream6_IRQHandler
  weak_handler DMA2_Stream7_IRQHandler
  weak_handler USART6_IRQHandler
  weak_handler I2C3_EV_IRQHandler
  weak_handler I2C3_ER_IRQHandler
  weak_handler OTG_HS_EP1_OUT_IRQHandler
  weak_handler OTG_HS_EP1_IN_IRQHandler
  weak_handler OTG_HS_WKUP_IRQHandler
  weak_handler OTG_HS_IRQHandler
  weak_handler DCMI_IRQHandler
  weak_handler CRYP_IRQHandler
  weak_handler HASH_RNG_IRQHandler
  weak_handler FPU_IRQHandler
  weak_handler UART7_IRQHandler
  weak_handler UART8_IRQHandler
  weak_handler SPI4_IRQHandler
  weak_handler SPI5_IRQHandler
  weak_handler SPI6_IRQHandler
  weak_handler SAI1_IRQHandler
  weak_handler LCD_TFT_IRQHandler
  weak_handler LCD_TFT_1_IRQHandler
  weak_handler DMA2D_IRQHandler
  weak_handler SAI2_IRQHandler
  weak_handler QuadSPI_IRQHandler
  weak_handler LP_Timer1_IRQHandler
  weak_handler I2C4_EV_IRQHandler
  weak_handler I2C4_ER_IRQHandler
  weak_handler SPDIFRX_IRQHandler
  weak_handler DSIHOST_IRQHandler
  weak_handler DFSDM1_FLT0_IRQHandler
  weak_handler DFSDM1_FLT1_IRQHandler
  weak_handler DFSDM1_FLT2_IRQHandler
  weak_handler DFSDM1_FLT3_IRQHandler
  weak_handler SDMMC2_IRQHandler
  weak_handler CAN3_TX_IRQHandler
  weak_handler CAN3_RX0_IRQHandler
  weak_handler CAN3_RX1_IRQHandler
  weak_handler CAN3_SCE_IRQHandler
  weak_handler JPEG_IRQHandler
  weak_handler MDIOS_IRQHandler
