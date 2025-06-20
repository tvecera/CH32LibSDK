/********************************** (C) COPYRIGHT *******************************
* File Name          : startup_ch32v30x_D8C.s
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/04/06
* Description        : CH32V307x-CH32V305x vector table for eclipse toolchain.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

	.section	.init, "ax", @progbits

// ---- Initialize - vector table and reset handler

	.option	push		// push current linker setup
	.option	norvc		// disable compressed instructions in vector table (use 32-bit jumps)
	.option	norelax		// disable linker relaxation (disable optimizing programs at link-time)
	.align	2

.global _start
_start:
_vector_base:
	// CPU Internal Interrupts
	j	handle_reset		// 0: RESET handler
	.word	0			// 1:
	.word	NMI_Handler		// 2: NMI Handler, Non Naskable interrupt (IRQ_NMI)
	.word	HardFault_Handler	// 3: Hard Fault Handler, Exception interrupt (IRQ_HARDFAULT)
	.word	0			// 4:
	.word	Ecall_M_Mode_Handler	// 5: Ecall M Mode Interrupt (IRQ_ECALL_MMODE)
	.word	0			// 6:
	.word	0			// 7:
	.word	Ecall_U_Mode_Handler	// 8: Ecall U Mode Interrupt (IRQ_ECALL_UMODE)
	.word	Break_Point_Handler	// 9: Break Point Interrupt (IRQ_BREAKPOINT)
	.word	0			// 10:
	.word	0			// 11:
	.word	SysTick_Handler		// 12: SysTick Handler, System Timer interrupt (IRQ_SYSTICK)
	.word	0			// 13:
	.word	SW_Handler		// 14: SW Handler, software interrupt (IRQ_SW)
	.word	0			// 15:
	// External Interrupts
	.word	WWDG_IRQHandler		// 16: Window Watchdog, Window WatchDog interrupt (IRQ_WWDG)
	.word	PVD_IRQHandler		// 17: PVD through EXTI Line detection interrupt, Supply voltage detection interrupt (IRQ_PVD)
	.word	TAMPER_IRQHandler	// 18: Tamper interrupt (IRQ_TAMPER)
	.word	RTC_IRQHandler		// 19: RTC global interrupt (IRQ_RTC)
	.word	FLASH_IRQHandler	// 20: Flash global interrupt (IRQ_FLASH)
	.word	RCC_IRQHandler		// 21: RCC global Interrupt (IRQ_RCC)
	.word	EXTI0_IRQHandler	// 22: EXTI Line0 Interrupt (IRQ_EXTI0)
	.word	EXTI1_IRQHandler	// 23: EXTI Line1 Interrupt (IRQ_EXTI1)
	.word	EXTI2_IRQHandler	// 24: EXTI Line2 Interrupt (IRQ_EXTI2)
	.word	EXTI3_IRQHandler	// 25: EXTI Line3 Interrupt (IRQ_EXTI3)
	.word	EXTI4_IRQHandler	// 26: EXTI Line4 Interrupt (IRQ_EXTI4)
	.word	DMA1_Channel1_IRQHandler // 27: DMA1 Channel 1 global interrupt (IRQ_DMA1_CH1)
	.word	DMA1_Channel2_IRQHandler // 28: DMA1 Channel 2 global interrupt (IRQ_DMA1_CH2)
	.word	DMA1_Channel3_IRQHandler // 29: DMA1 Channel 3 global interrupt (IRQ_DMA1_CH3)
	.word	DMA1_Channel4_IRQHandler // 30: DMA1 Channel 4 global interrupt (IRQ_DMA1_CH4)
	.word	DMA1_Channel5_IRQHandler // 31: DMA1 Channel 5 global interrupt (IRQ_DMA1_CH5)
	.word	DMA1_Channel6_IRQHandler // 32: DMA1 Channel 6 global interrupt (IRQ_DMA1_CH6)
	.word	DMA1_Channel7_IRQHandler // 33: DMA1 Channel 7 global interrupt (IRQ_DMA1_CH7)
	.word	ADC1_IRQHandler		// 34: ADC1 global interrupt (IRQ_ADC1)
	.word	USB_HP_CAN1_TX_IRQHandler // 35: USB Device High Priority or CAN1 TX Interrupts (IRQ_USB_HP_CAN1_TX)
	.word	USB_LP_CAN1_RX0_IRQHandler // 36: USB Device Low Priority or CAN1 RX0 Interrupts (IRQ_USB_LP_CAN1_RX0)
	.word	CAN1_RX1_IRQHandler	// 37: CAN1 RX1 Interrupt (IRQ_CAN1_RX1)
	.word	CAN1_SCE_IRQHandler	// 38: CAN1 SCE Interrupt (IRQ_CAN1_SCE)
	.word	EXTI9_5_IRQHandler	// 39: External Line[9:5] Interrupts (IRQ_EXTI9_5)
	.word	TIM1_BRK_IRQHandler	// 40: TIM1 Break interrupt (IRQ_TIM1_BRK)
	.word	TIM1_UP_IRQHandler	// 41: TIM1 Update interrupt (IRQ_TIM1_UP)
	.word	TIM1_TRG_COM_IRQHandler	// 42: TIM1 Trigger and Commutation interrupt (IRQ_TIM1_TRG)
	.word	TIM1_CC_IRQHandler	// 43: TIM1 Capture Compare interrupt (IRQ_TIM1_CC)
	.word	TIM2_IRQHandler		// 44: TIM2 global interrupt (IRQ_TIM2)
	.word	TIM3_IRQHandler		// 45: TIM3 global interrupt (IRQ_TIM3)
	.word	TIM4_IRQHandler		// 46: TIM4 global interrupt (IRQ_TIM4)
	.word	I2C1_EV_IRQHandler	// 47: I2C1 Event Interrupt (IRQ_I2C1_EV)
	.word	I2C1_ER_IRQHandler	// 48: I2C1 Error Interrupt (IRQ_I2C1_ER)
	.word	I2C2_EV_IRQHandler	// 49: I2C2 Event Interrupt (IRQ_I2C2_EV)
	.word	I2C2_ER_IRQHandler	// 50: I2C2 Error Interrupt (IRQ_I2C2_ER)
	.word	SPI1_IRQHandler		// 51: SPI1 global Interrupt (IRQ_SPI1)
	.word	SPI2_IRQHandler		// 52: SPI2 global Interrupt (IRQ_SPI2)
	.word	USART1_IRQHandler	// 53: USART1 global Interrupt (IRQ_USART1)
	.word	USART2_IRQHandler	// 54: USART2 global Interrupt (IRQ_USART2)
	.word	USART3_IRQHandler	// 55: USART3 global Interrupt (IRQ_USART3)
	.word	EXTI15_10_IRQHandler	// 56: External Line[15:10] Interrupts (IRQ_EXTI15_10)
	.word	RTCAlarm_IRQHandler	// 57: RTC Alarm through EXTI Line Interrupt (IRQ_RTC_ALARM)
	.word	USBWakeUp_IRQHandler	// 58: USB WakeUp from suspend through EXTI Line Interrupt (IRQ_USB_WU)
	.word	TIM8_BRK_IRQHandler	// 59: TIM8 Break Interrupt (IRQ_TIM8_BRK)
	.word	TIM8_UP_IRQHandler	// 60: TIM8 Update Interrupt (IRQ_TIM8_UP)
	.word	TIM8_TRG_COM_IRQHandler	// 61: TIM8 Trigger and Commutation Interrupt (IRQ_TIM8_TRG)
	.word	TIM8_CC_IRQHandler	// 62: TIM8 Capture Compare Interrupt (IRQ_TIM8_CC)
	.word	RNG_IRQHandler		// 63: RNG global Interrupt (IRQ_RNG)
	.word	0			// 64:
	.word	SDIO_IRQHandler		// 65: SDIO global Interrupt (IRQ_SDIO)
	.word	TIM5_IRQHandler		// 66: TIM5 global Interrupt (IRQ_TIM5)
	.word	SPI3_IRQHandler		// 67: SPI3 global Interrupt (IRQ_SPI3)
	.word	USART4_IRQHandler	// 68: USART4 global Interrupt (IRQ_USART4)
	.word	USART5_IRQHandler	// 69: USART5 global Interrupt (IRQ_USART5)
	.word	TIM6_IRQHandler		// 70: TIM6 global Interrupt (IRQ_TIM6)
	.word	TIM7_IRQHandler		// 71: TIM7 global Interrupt (IRQ_TIM7)
	.word	DMA2_Channel1_IRQHandler // 72: DMA2 Channel 1 global Interrupt (IRQ_DMA2_CH1)
	.word	DMA2_Channel2_IRQHandler // 73: DMA2 Channel 2 global Interrupt (IRQ_DMA2_CH2)
	.word	DMA2_Channel3_IRQHandler // 74: DMA2 Channel 3 global Interrupt (IRQ_DMA2_CH3)
	.word	DMA2_Channel4_IRQHandler // 75: DMA2 Channel 4 global Interrupt (IRQ_DMA2_CH4)
	.word	DMA2_Channel5_IRQHandler // 76: DMA2 Channel 5 global Interrupt (IRQ_DMA2_CH5)
	.word	ETH_IRQHandler		// 77: ETH global Interrupt (IRQ_ETH)
	.word	ETH_WKUP_IRQHandler	// 78: ETH WakeUp Interrupt (IRQ_ETH_WU)
	.word	CAN2_TX_IRQHandler	// 79: CAN2 TX Interrupts (IRQ_CAN2_TX)
	.word	CAN2_RX0_IRQHandler	// 80: CAN2 RX0 Interrupts (IRQ_CAN2_RX0)
	.word	CAN2_RX1_IRQHandler	// 81: CAN2 RX1 Interrupt (IRQ_CAN2_RX1)
	.word	CAN2_SCE_IRQHandler	// 82: CAN2 SCE Interrupt (IRQ_CAN2_SCE)
	.word	USBFS_IRQHandler	// 83: USBFS global Interrupt (IRQ_USBFS)
	.word	USBHSWakeup_IRQn	// 84: USBHS WakeUp Interrupt (IRQ_USBHS_WU)
	.word	USBHS_IRQn		// 85: USBHS global Interrupt (IRQ_USBHS)
	.word	DVP_IRQn		// 86: DVP global Interrupt (IRQ_DVP)
	.word	USART6_IRQHandler	// 87: USART6 global Interrupt (IRQ_USART6)
	.word	USART7_IRQHandler	// 88: USART7 global Interrupt (IRQ_USART7)
	.word	USART8_IRQHandler	// 89: USART8 global Interrupt (IRQ_USART8)
	.word	TIM9_BRK_IRQHandler	// 90: TIM9 Break Interrupt (IRQ_TIM9_BRK)
	.word	TIM9_UP_IRQHandler	// 91: TIM9 Update Interrupt (IRQ_TIM9_UP)
	.word	TIM9_TRG_COM_IRQHandler	// 92: TIM9 Trigger and Commutation Interrupt (IRQ_TIM9_TRG)
	.word	TIM9_CC_IRQHandler	// 93: TIM9 Capture Compare Interrupt (IRQ_TIM9_CC)
	.word	TIM10_BRK_IRQHandler	// 94: TIM10 Break Interrupt (IRQ_TIM10_BRK)
	.word	TIM10_UP_IRQHandler	// 95: TIM10 Update Interrupt (IRQ_TIM10_UP)
	.word	TIM10_TRG_COM_IRQHandler // 96: TIM10 Trigger and Commutation Interrupt (IRQ_TIM10_TRG)
	.word	TIM10_CC_IRQHandler	// 97: TIM10 Capture Compare Interrupt (IRQ_TIM10_CC)
	.word	DMA2_Channel6_IRQHandler // 98: DMA2 Channel 6 global Interrupt (IRQ_DMA2_CH6)
	.word	DMA2_Channel7_IRQHandler // 99: DMA2 Channel 7 global Interrupt (IRQ_DMA2_CH7)
	.word	DMA2_Channel8_IRQHandler // 100: DMA2 Channel 8 global Interrupt (IRQ_DMA2_CH8)
	.word	DMA2_Channel9_IRQHandler // 101: DMA2 Channel 9 global Interrupt (IRQ_DMA2_CH9)
	.word	DMA2_Channel10_IRQHandler // 102: DMA2 Channel 10 global Interrupt (IRQ_DMA2_CH10)
	.word	DMA2_Channel11_IRQHandler // 103: DMA2 Channel 10 global Interrupt (IRQ_DMA2_CH11)

	.option	pop			// pop original linker setup

// ---- Weak handlers

	.align	2

.macro weak_irq name
.weak \name
.type \name,%function
\name:
.endm

	weak_irq	NMI_Handler		// 2: NMI Handler, Non Naskable interrupt (IRQ_NMI)
	weak_irq	HardFault_Handler	// 3: Hard Fault Handler, Exception interrupt (IRQ_HARDFAULT)
	weak_irq	Ecall_M_Mode_Handler	// 5: Ecall M Mode Interrupt (IRQ_ECALL_MMODE)
	weak_irq	Ecall_U_Mode_Handler	// 8: Ecall U Mode Interrupt (IRQ_ECALL_UMODE)
	weak_irq	Break_Point_Handler	// 9: Break Point Interrupt (IRQ_BREAKPOINT)
	weak_irq	SysTick_Handler		// 12: SysTick Handler, System Timer interrupt (IRQ_SYSTICK)
	weak_irq	SW_Handler		// 14: SW Handler, software interrupt (IRQ_SW)
	weak_irq	WWDG_IRQHandler		// 16: Window Watchdog, Window WatchDog interrupt (IRQ_WWDG)
	weak_irq	PVD_IRQHandler		// 17: PVD through EXTI Line detection interrupt, Supply voltage detection interrupt (IRQ_PVD)
	weak_irq	TAMPER_IRQHandler	// 18: Tamper interrupt (IRQ_TAMPER)
	weak_irq	RTC_IRQHandler		// 19: RTC global interrupt (IRQ_RTC)
	weak_irq	FLASH_IRQHandler	// 20: Flash global interrupt (IRQ_FLASH)
	weak_irq	RCC_IRQHandler		// 21: RCC global Interrupt (IRQ_RCC)
	weak_irq	EXTI0_IRQHandler	// 22: EXTI Line0 Interrupt (IRQ_EXTI0)
	weak_irq	EXTI1_IRQHandler	// 23: EXTI Line1 Interrupt (IRQ_EXTI1)
	weak_irq	EXTI2_IRQHandler	// 24: EXTI Line2 Interrupt (IRQ_EXTI2)
	weak_irq	EXTI3_IRQHandler	// 25: EXTI Line3 Interrupt (IRQ_EXTI3)
	weak_irq	EXTI4_IRQHandler	// 26: EXTI Line4 Interrupt (IRQ_EXTI4)
	weak_irq	DMA1_Channel1_IRQHandler // 27: DMA1 Channel 1 global interrupt (IRQ_DMA1_CH1)
	weak_irq	DMA1_Channel2_IRQHandler // 28: DMA1 Channel 2 global interrupt (IRQ_DMA1_CH2)
	weak_irq	DMA1_Channel3_IRQHandler // 29: DMA1 Channel 3 global interrupt (IRQ_DMA1_CH3)
	weak_irq	DMA1_Channel4_IRQHandler // 30: DMA1 Channel 4 global interrupt (IRQ_DMA1_CH4)
	weak_irq	DMA1_Channel5_IRQHandler // 31: DMA1 Channel 5 global interrupt (IRQ_DMA1_CH5)
	weak_irq	DMA1_Channel6_IRQHandler // 32: DMA1 Channel 6 global interrupt (IRQ_DMA1_CH6)
	weak_irq	DMA1_Channel7_IRQHandler // 33: DMA1 Channel 7 global interrupt (IRQ_DMA1_CH7)
	weak_irq	ADC1_IRQHandler		// 34: ADC1 global interrupt (IRQ_ADC1)
	weak_irq	USB_HP_CAN1_TX_IRQHandler // 35: USB Device High Priority or CAN1 TX Interrupts (IRQ_USB_HP_CAN1_TX)
	weak_irq	USB_LP_CAN1_RX0_IRQHandler // 36: USB Device Low Priority or CAN1 RX0 Interrupts (IRQ_USB_LP_CAN1_RX0)
	weak_irq	CAN1_RX1_IRQHandler	// 37: CAN1 RX1 Interrupt (IRQ_CAN1_RX1)
	weak_irq	CAN1_SCE_IRQHandler	// 38: CAN1 SCE Interrupt (IRQ_CAN1_SCE)
	weak_irq	EXTI9_5_IRQHandler	// 39: External Line[9:5] Interrupts (IRQ_EXTI9_5)
	weak_irq	TIM1_BRK_IRQHandler	// 40: TIM1 Break interrupt (IRQ_TIM1_BRK)
	weak_irq	TIM1_UP_IRQHandler	// 41: TIM1 Update interrupt (IRQ_TIM1_UP)
	weak_irq	TIM1_TRG_COM_IRQHandler	// 42: TIM1 Trigger and Commutation interrupt (IRQ_TIM1_TRG)
	weak_irq	TIM1_CC_IRQHandler	// 43: TIM1 Capture Compare interrupt (IRQ_TIM1_CC)
	weak_irq	TIM2_IRQHandler		// 44: TIM2 global interrupt (IRQ_TIM2)
	weak_irq	TIM3_IRQHandler		// 45: TIM3 global interrupt (IRQ_TIM3)
	weak_irq	TIM4_IRQHandler		// 46: TIM4 global interrupt (IRQ_TIM4)
	weak_irq	I2C1_EV_IRQHandler	// 47: I2C1 Event Interrupt (IRQ_I2C1_EV)
	weak_irq	I2C1_ER_IRQHandler	// 48: I2C1 Error Interrupt (IRQ_I2C1_ER)
	weak_irq	I2C2_EV_IRQHandler	// 49: I2C2 Event Interrupt (IRQ_I2C2_EV)
	weak_irq	I2C2_ER_IRQHandler	// 50: I2C2 Error Interrupt (IRQ_I2C2_ER)
	weak_irq	SPI1_IRQHandler		// 51: SPI1 global Interrupt (IRQ_SPI1)
	weak_irq	SPI2_IRQHandler		// 52: SPI2 global Interrupt (IRQ_SPI2)
	weak_irq	USART1_IRQHandler	// 53: USART1 global Interrupt (IRQ_USART1)
	weak_irq	USART2_IRQHandler	// 54: USART2 global Interrupt (IRQ_USART2)
	weak_irq	USART3_IRQHandler	// 55: USART3 global Interrupt (IRQ_USART3)
	weak_irq	EXTI15_10_IRQHandler	// 56: External Line[15:10] Interrupts (IRQ_EXTI15_10)
	weak_irq	RTCAlarm_IRQHandler	// 57: RTC Alarm through EXTI Line Interrupt (IRQ_RTC_ALARM)
	weak_irq	USBWakeUp_IRQHandler	// 58: USB WakeUp from suspend through EXTI Line Interrupt (IRQ_USB_WU)
	weak_irq	TIM8_BRK_IRQHandler	// 59: TIM8 Break Interrupt (IRQ_TIM8_BRK)
	weak_irq	TIM8_UP_IRQHandler	// 60: TIM8 Update Interrupt (IRQ_TIM8_UP)
	weak_irq	TIM8_TRG_COM_IRQHandler	// 61: TIM8 Trigger and Commutation Interrupt (IRQ_TIM8_TRG)
	weak_irq	TIM8_CC_IRQHandler	// 62: TIM8 Capture Compare Interrupt (IRQ_TIM8_CC)
	weak_irq	RNG_IRQHandler		// 63: RNG global Interrupt (IRQ_RNG)
	weak_irq	SDIO_IRQHandler		// 65: SDIO global Interrupt (IRQ_SDIO)
	weak_irq	TIM5_IRQHandler		// 66: TIM5 global Interrupt (IRQ_TIM5)
	weak_irq	SPI3_IRQHandler		// 67: SPI3 global Interrupt (IRQ_SPI3)
	weak_irq	USART4_IRQHandler	// 68: USART4 global Interrupt (IRQ_USART4)
	weak_irq	USART5_IRQHandler	// 69: USART5 global Interrupt (IRQ_USART5)
	weak_irq	TIM6_IRQHandler		// 70: TIM6 global Interrupt (IRQ_TIM6)
	weak_irq	TIM7_IRQHandler		// 71: TIM7 global Interrupt (IRQ_TIM7)
	weak_irq	DMA2_Channel1_IRQHandler // 72: DMA2 Channel 1 global Interrupt (IRQ_DMA2_CH1)
	weak_irq	DMA2_Channel2_IRQHandler // 73: DMA2 Channel 2 global Interrupt (IRQ_DMA2_CH2)
	weak_irq	DMA2_Channel3_IRQHandler // 74: DMA2 Channel 3 global Interrupt (IRQ_DMA2_CH3)
	weak_irq	DMA2_Channel4_IRQHandler // 75: DMA2 Channel 4 global Interrupt (IRQ_DMA2_CH4)
	weak_irq	DMA2_Channel5_IRQHandler // 76: DMA2 Channel 5 global Interrupt (IRQ_DMA2_CH5)
	weak_irq	ETH_IRQHandler		// 77: ETH global Interrupt (IRQ_ETH)
	weak_irq	ETH_WKUP_IRQHandler	// 78: ETH WakeUp Interrupt (IRQ_ETH_WU)
	weak_irq	CAN2_TX_IRQHandler	// 79: CAN2 TX Interrupts (IRQ_CAN2_TX)
	weak_irq	CAN2_RX0_IRQHandler	// 80: CAN2 RX0 Interrupts (IRQ_CAN2_RX0)
	weak_irq	CAN2_RX1_IRQHandler	// 81: CAN2 RX1 Interrupt (IRQ_CAN2_RX1)
	weak_irq	CAN2_SCE_IRQHandler	// 82: CAN2 SCE Interrupt (IRQ_CAN2_SCE)
	weak_irq	USBFS_IRQHandler	// 83: USBFS global Interrupt (IRQ_USBFS)
	weak_irq	USBHSWakeup_IRQn	// 84: USBHS WakeUp Interrupt (IRQ_USBHS_WU)
	weak_irq	USBHS_IRQn		// 85: USBHS global Interrupt (IRQ_USBHS)
	weak_irq	DVP_IRQn		// 86: DVP global Interrupt (IRQ_DVP)
	weak_irq	USART6_IRQHandler	// 87: USART6 global Interrupt (IRQ_USART6)
	weak_irq	USART7_IRQHandler	// 88: USART7 global Interrupt (IRQ_USART7)
	weak_irq	USART8_IRQHandler	// 89: USART8 global Interrupt (IRQ_USART8)
	weak_irq	TIM9_BRK_IRQHandler	// 90: TIM9 Break Interrupt (IRQ_TIM9_BRK)
	weak_irq	TIM9_UP_IRQHandler	// 91: TIM9 Update Interrupt (IRQ_TIM9_UP)
	weak_irq	TIM9_TRG_COM_IRQHandler	// 92: TIM9 Trigger and Commutation Interrupt (IRQ_TIM9_TRG)
	weak_irq	TIM9_CC_IRQHandler	// 93: TIM9 Capture Compare Interrupt (IRQ_TIM9_CC)
	weak_irq	TIM10_BRK_IRQHandler	// 94: TIM10 Break Interrupt (IRQ_TIM10_BRK)
	weak_irq	TIM10_UP_IRQHandler	// 95: TIM10 Update Interrupt (IRQ_TIM10_UP)
	weak_irq	TIM10_TRG_COM_IRQHandler // 96: TIM10 Trigger and Commutation Interrupt (IRQ_TIM10_TRG)
	weak_irq	TIM10_CC_IRQHandler	// 97: TIM10 Capture Compare Interrupt (IRQ_TIM10_CC)
	weak_irq	DMA2_Channel6_IRQHandler // 98: DMA2 Channel 6 global Interrupt (IRQ_DMA2_CH6)
	weak_irq	DMA2_Channel7_IRQHandler // 99: DMA2 Channel 7 global Interrupt (IRQ_DMA2_CH7)
	weak_irq	DMA2_Channel8_IRQHandler // 100: DMA2 Channel 8 global Interrupt (IRQ_DMA2_CH8)
	weak_irq	DMA2_Channel9_IRQHandler // 101: DMA2 Channel 9 global Interrupt (IRQ_DMA2_CH9)
	weak_irq	DMA2_Channel10_IRQHandler // 102: DMA2 Channel 10 global Interrupt (IRQ_DMA2_CH10)
	weak_irq	DMA2_Channel11_IRQHandler // 103: DMA2 Channel 10 global Interrupt (IRQ_DMA2_CH11)

	weak_irq	DefaultHandler		// default handler
1:	j	1b

// ---- Reset handler

	.align	2

.type handle_reset,%function
handle_reset:

	// setup global pointer
	.option	push			// push current linker setup
	.option	norelax			// disable linker relaxation (disable optimizing programs at link-time)
	la	gp,__global_pointer$	// global pointer to global data in RAM
	.option	pop			// pop original linker setup

	// setup stack pointer
	la	sp,_eusrstack		// SP <- end of user stack

	// Load data section from flash to RAM
	la	a0,_data_lma		// A0 <- load memory address of initialized data in Flash (source address)
	la	a1,_data_vma		// A1 <- virtual memory address of initialized data in RAM (destination address)
	la	a2,_edata		// A2 <- end of initialized data in RAM
	bgeu	a1,a2,2f		// skip if no data to copy

1:	lw	t0,(a0)			// T0 <- load data as 32-bit word from Flash
	sw	t0,(a1)			// save data to RAM
	addi	a0,a0,4			// A0 <- shift source address
	addi	a1,a1,4			// A1 <- shift destination address
	bltu	a1,a2,1b		// copy next word

	// Clear bss section
2:	la	a0,_sbss		// A0 <- start of bss section
	la	a1,_ebss		// A1 <- end of bss section
	bgeu	a0,a1,2f		// skip if no bss section

1:	sw	zero,(a0)		// store 0 to (a0)
	addi	a0,a0,4			// A0 <- shift destination addres
	bltu	a0,a1,1b		// continue with next data

	// Configure pipelining and instruction prediction
2:	li	t0,0x1f
	csrw	0xbc0,t0

	// Enable interrupt nesting and hardware stack
	li	t0,0x0b
	csrw	0x804,t0

	// Enable floating point and global interrupt, configure privileged mode
   	li	t0,0x6088
   	csrw	mstatus,t0

	// Configure the interrupt vector table recognition mode and entry address mode
	la	t0,_vector_base		// T0 <- start of vector table
	ori	t0,t0,3			// set vector mode
	csrw	mtvec,t0		// set vector table

	// System initialize  
	jal	SystemInit

	// jump to main function
	la	t0,main			// T0 <- pointer to main function
	csrw	mepc,t0			// setup machine exception address
	mret				// jump to main function
