;/********************************** (C) COPYRIGHT *******************************
;* File Name          : startup_ch32v10x.s
;* Author             : WCH
;* Version            : V1.0.1
;* Date               : 2024/01/11
;* Description        : CH32V10x vector table for eclipse toolchain.
;*********************************************************************************
;* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
;* Attention: This software (modified or not) and binary are used for 
;* microcontroller manufactured by Nanjing Qinheng Microelectronics.
;*******************************************************************************/

// >>> Warning: V3A processor supports only mode "jump instruction" at the vector table. <<<
//  Vector table contains jump instructions, not destination address!

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
	j	NMI_Handler		// 2: NMI Handler, Non Naskable interrupt (IRQ_NMI)
	j	HardFault_Handler	// 3: Hard Fault Handler, Exception interrupt (IRQ_HARDFAULT)
	.word	0			// 4:
	.word	0			// 5:
	.word	0			// 6:
	.word	0			// 7:
	.word	0			// 8:
	.word	0			// 9:
	.word	0			// 10:
	.word	0			// 11:
	j	SysTick_Handler		// 12: SysTick Handler, System Timer interrupt (IRQ_SYSTICK)
	.word	0			// 13:
	j	SW_Handler		// 14: SW Handler, software interrupt (IRQ_SW)
	.word	0			// 15:
	// External Interrupts
	j	WWDG_IRQHandler		// 16: Window Watchdog, Window WatchDog interrupt (IRQ_WWDG)
	j	PVD_IRQHandler		// 17: PVD through EXTI Line detection interrupt, Supply voltage detection interrupt (IRQ_PVD)
	j	TAMPER_IRQHandler	// 18: Tamper interrupt (IRQ_TAMPER)
	j	RTC_IRQHandler		// 19: RTC global interrupt (IRQ_RTC)
	j	FLASH_IRQHandler	// 20: Flash global interrupt (IRQ_FLASH)
	j	RCC_IRQHandler		// 21: RCC global Interrupt (IRQ_RCC)
	j	EXTI0_IRQHandler	// 22: EXTI Line0 Interrupt (IRQ_EXTI0)
	j	EXTI1_IRQHandler	// 23: EXTI Line1 Interrupt (IRQ_EXTI1)
	j	EXTI2_IRQHandler	// 24: EXTI Line2 Interrupt (IRQ_EXTI2)
	j	EXTI3_IRQHandler	// 25: EXTI Line3 Interrupt (IRQ_EXTI3)
	j	EXTI4_IRQHandler	// 26: EXTI Line4 Interrupt (IRQ_EXTI4)
	j	DMA1_Channel1_IRQHandler // 27: DMA1 Channel 1 global interrupt (IRQ_DMA1_CH1)
	j	DMA1_Channel2_IRQHandler // 28: DMA1 Channel 2 global interrupt (IRQ_DMA1_CH2)
	j	DMA1_Channel3_IRQHandler // 29: DMA1 Channel 3 global interrupt (IRQ_DMA1_CH3)
	j	DMA1_Channel4_IRQHandler // 30: DMA1 Channel 4 global interrupt (IRQ_DMA1_CH4)
	j	DMA1_Channel5_IRQHandler // 31: DMA1 Channel 5 global interrupt (IRQ_DMA1_CH5)
	j	DMA1_Channel6_IRQHandler // 32: DMA1 Channel 6 global interrupt (IRQ_DMA1_CH6)
	j	DMA1_Channel7_IRQHandler // 33: DMA1 Channel 7 global interrupt (IRQ_DMA1_CH7)
	j	ADC1_IRQHandler		// 34: ADC1 global interrupt (IRQ_ADC1)
	.word	0			// 35:
	.word	0			// 36:
	.word	0			// 37:
	.word	0			// 38:
	j	EXTI9_5_IRQHandler	// 39: External Line[9:5] Interrupts (IRQ_EXTI9_5)
	j	TIM1_BRK_IRQHandler	// 40: TIM1 Break interrupt (IRQ_TIM1_BRK)
	j	TIM1_UP_IRQHandler	// 41: TIM1 Update interrupt (IRQ_TIM1_UP)
	j	TIM1_TRG_COM_IRQHandler	// 42: TIM1 Trigger and Commutation interrupt (IRQ_TIM1_TRG)
	j	TIM1_CC_IRQHandler	// 43: TIM1 Capture Compare interrupt (IRQ_TIM1_CC)
	j	TIM2_IRQHandler		// 44: TIM2 global interrupt (IRQ_TIM2)
	j	TIM3_IRQHandler		// 45: TIM3 global interrupt (IRQ_TIM3)
	j	TIM4_IRQHandler		// 46: TIM4 global interrupt (IRQ_TIM4)
	j	I2C1_EV_IRQHandler	// 47: I2C1 Event Interrupt (IRQ_I2C1_EV)
	j	I2C1_ER_IRQHandler	// 48: I2C1 Error Interrupt (IRQ_I2C1_ER)
	j	I2C2_EV_IRQHandler	// 49: I2C2 Event Interrupt (IRQ_I2C2_EV)
	j	I2C2_ER_IRQHandler	// 50: I2C2 Error Interrupt (IRQ_I2C2_ER)
	j	SPI1_IRQHandler		// 51: SPI1 global Interrupt (IRQ_SPI1)
	j	SPI2_IRQHandler		// 52: SPI2 global Interrupt (IRQ_SPI2)
	j	USART1_IRQHandler	// 53: USART1 global Interrupt (IRQ_USART1)
	j	USART2_IRQHandler	// 54: USART2 global Interrupt (IRQ_USART2)
	j	USART3_IRQHandler	// 55: USART3 global Interrupt (IRQ_USART3)
	j	EXTI15_10_IRQHandler	// 56: External Line[15:10] Interrupts (IRQ_EXTI15_10)
	j	RTCAlarm_IRQHandler	// 57: RTC Alarm through EXTI Line Interrupt (IRQ_RTC_ALARM)
	j	USBWakeUp_IRQHandler	// 58: USB WakeUp from suspend through EXTI Line Interrupt (IRQ_USB_WU)
	j	USBFS_IRQHandler	// 59: USBFS Interrupt (IRQ_USBFS)

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
	weak_irq	USBFS_IRQHandler	// 59: USBFS Interrupt (IRQ_USBFS)

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

	// Enable global interrupt and configure privileged mode
2:	li	t0,0x88
	csrw	mstatus,t0

	// Configure the interrupt vector table recognition mode and entry jump mode
	la	t0,_vector_base		// T0 <- start of vector table
	ori	t0,t0,1			// set vector jump mode
	csrw	mtvec,t0		// set vector table

	// System initialize  
	jal	SystemInit

	// jump to main function
	la	t0,main			// T0 <- pointer to main function
	csrw	mepc,t0			// setup machine exception address
	mret				// jump to main function
