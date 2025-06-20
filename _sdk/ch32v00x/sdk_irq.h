
// ****************************************************************************
//
//                                Interrupt
//
// ****************************************************************************

#if USE_IRQ		// 1=use IRQ interrupt support

#ifndef _SDK_IRQ_H
#define _SDK_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

// CPU Internal Interrupts
#define IRQ_RESET	0		// Reset
#define IRQ_NMI		2		// NMI, Non Naskable interrupt (default weak handler NMI_Handler)
#define IRQ_HARDFAULT	3		// Hard Fault, Exception interrupt (default weak handler HardFault_Handler)
#define IRQ_SYSTICK	12		// SysTick, System Timer interrupt (default weak handler SysTick_Handler)
#define IRQ_SW		14		// Software interrupt (default weak handler SW_Handler)
// External Interrupts
#define IRQ_WWDG	16		// Window WatchDog interrupt (default weak handler WWDG_IRQHandler)
#define IRQ_PVD		17		// PVD through EXTI line detection interrupt, Supply voltage detection interrupt (default weak handler PVD_IRQHandler)
#define IRQ_FLASH	18		// Flash global interrupt (default weak handler FLASH_IRQHandler)
#define IRQ_RCC		19		// RCC global interrupt (default weak handler RCC_IRQHandler)
#define IRQ_EXTI7	20		// External line 7..0 interrupts (default weak handler EXTI7_0_IRQHandler)
#define IRQ_AWU		21		// AWU Automate WakeUp interrupt (default weak handler AWU_IRQHandler)
#define IRQ_DMA1_CH1	22		// DMA1 channel 1 global interrupt (default weak handler DMA1_Channel1_IRQHandler)
#define IRQ_DMA1_CH2	23		// DMA1 channel 2 global interrupt (default weak handler DMA1_Channel2_IRQHandler)
#define IRQ_DMA1_CH3	24		// DMA1 channel 3 global interrupt (default weak handler DMA1_Channel3_IRQHandler)
#define IRQ_DMA1_CH4	25		// DMA1 channel 4 global interrupt (default weak handler DMA1_Channel4_IRQHandler)
#define IRQ_DMA1_CH5	26		// DMA1 channel 5 global interrupt (default weak handler DMA1_Channel5_IRQHandler)
#define IRQ_DMA1_CH6	27		// DMA1 channel 6 global interrupt (default weak handler DMA1_Channel6_IRQHandler)
#define IRQ_DMA1_CH7	28		// DMA1 channel 7 global interrupt (default weak handler DMA1_Channel7_IRQHandler)
#define IRQ_ADC1	29		// ADC1 interrupt (default weak handler ADC1_IRQHandler)
#define IRQ_I2C1_EV	30		// I2C1 Event interrupt (default weak handler I2C1_EV_IRQHandler)
#define IRQ_I2C1_ER	31		// I2C1 Error interrupt (default weak handler I2C1_ER_IRQHandler)
#define IRQ_USART1	32		// USART1 global interrupt (default weak handler USART1_IRQHandler)
#define IRQ_SPI1	33		// SPI1 global interrupt (default weak handler SPI1_IRQHandler)
#define IRQ_TIM1_BRK	34		// TIM1 Break interrupt (default weak handler TIM1_BRK_IRQHandler)
#define IRQ_TIM1_UP	35		// TIM1 Update interrupt (default weak handler TIM1_UP_IRQHandler)
#define IRQ_TIM1_TRG	36		// TIM1 Trigger interrupt (default weak handler TIM1_TRG_COM_IRQHandler)
#define IRQ_TIM1_CC	37		// TIM1 Capture Compare interrupt (default weak handler TIM1_CC_IRQHandler)
#define IRQ_TIM2	38		// TIM2 global interrupt (default weak handler TIM2_IRQHandler)
#define IRQ_USART2	39		// (only CH32V005-CH32V007) USART2 global interrupt (default weak handler USART2_IRQHandler)
#define IRQ_OPCM	40		// (only CH32V005-CH32V007) OPCM global interrupt (default weak handler OPCM_IRQHandler)

#define IRQ_NUM		41		// number of interrupts

// common interrupt handler definition (need to mark handler function with HANDLER attribute)
typedef void (*irq_handler_t)(void);

// Programmable Fast Interrupt Controller (PFIC) 0xE000E000
typedef struct {
	io32	ISR[8];			// 0x00: (Read Only) Interrupt enable status register
	io32	IPR[8];			// 0x20: (Read Only) Interrupt pending status register
	io32	ITHRESDR;		// 0x40: Interrupt priority threshold configuration register
	io32	res1;			// 0x44: ... reserved
	io32	CFGR;			// 0x48: Interrupt configuration register
	io32	GISR;			// 0x4C: (Read Only) Interrupt global status register
	io8	VTFIDR[4];		// 0x50: VTF ID configuration register
	io32	res2[3];		// 0x54: ... reserved
	io32	VTFADDR[4];		// 0x60: VTF offset address register
	io32	res3[36];		// 0x70: ... reserved
	io32	IENR[8];		// 0x100: (Write Only) Interrupt enable setting register
	io32	res4[24];		// 0x120: ... reserved
	io32	IRER[8];		// 0x180: (Write Only) Interrupt enable clear register
	io32	res5[24];		// 0x1A0: ... reserved
	io32	IPSR[8];		// 0x200: (Write Only) Interrupt pending setting register
	io32	res6[24];		// 0x220: ... reserved
	io32	IPRR[8];		// 0x280: (Write Only) Interrupt pending clear register
	io32	res7[24];		// 0x2A0: ... reserved
	io32	IACTR[8];		// 0x300: (Read Only) Interrupt activation status register
	io32	res8[56];		// 0x320: ... reserved
	io8	IPRIOR[256];		// 0x400: Interrupt priority configuration register
	io32	res9[516];		// 0x500: ... reserved
	io32	SCTLR;			// 0xD10: System control register
} PFIC_t;
STATIC_ASSERT(sizeof(PFIC_t) == 0xD14, "Incorrect PFIC_t!");
#define PFIC		((PFIC_t*)PFIC_BASE)
#define NVIC		PFIC

#define NVIC_KEY1	0xFA050000
#define	NVIC_KEY2	0xBCAF0000
#define	NVIC_KEY3	0xBEEF0000

// IRQ priority
#define IRQ_PRIO_VERYHIGH	0x00	// highest priority - real-time time-critical interrupts
#define IRQ_PRIO_HIGH		0x40	// high priority - SysTick (must have higher priority than normal interrupts)
#define IRQ_PRIO_NORMAL		0x80	// default normal priority - normal code, normal interrupts
#define IRQ_PRIO_LOW		0xc0	// lowest priority - PendSV handler (must have lowest priority)

// External Interrupt/Event Controller 0x40010400
typedef struct {
	io32	INTENR;			// 0x00: Interrupt enable register
	io32	EVENR;			// 0x04: Event enable register
	io32	RTENR;			// 0x08: Rising edge trigger enable register
	io32	FTENR;			// 0x0C: Falling edge trigger enable register
	io32	SWIEVR;			// 0x10: Soft interrupt event register
	io32	INTFR;			// 0x14: Interrupt flag register
} EXTI_t;
STATIC_ASSERT(sizeof(EXTI_t) == 0x18, "Incorrect EXTI_t!");
#define EXTI		((EXTI_t*)EXTI_BASE)

// EXTI line
#define EXTI_LINE0	0		// External interrupt line 0
#define EXTI_LINE1	1		// External interrupt line 1
#define EXTI_LINE2	2		// External interrupt line 2
#define EXTI_LINE3	3		// External interrupt line 3
#define EXTI_LINE4	4		// External interrupt line 4
#define EXTI_LINE5	5		// External interrupt line 5
#define EXTI_LINE6	6		// External interrupt line 6
#define EXTI_LINE7	7		// External interrupt line 7
#define EXTI_LINE8	8		// External interrupt line 8 Connected to the PVD Output
#define EXTI_LINE9	9		// External interrupt line 9 Connected to the PWR Auto Wake-up event

// === NVIC Interrupt Controller

// get address of interrupt vector table
INLINE irq_handler_t* GetVTOR(void) { return (irq_handler_t*)(get_MTVEC() & ~3); } 

// set address of interrupt vector table
INLINE void SetVTOR(irq_handler_t* addr) { cb(); set_MTVEC((u32)addr | 3); cb(); }

// set interrupt IRQ_* service handler
// - Vector table must be located in RAM, or use weak handler otherwise, or use VTF channel.
// - Need to mark handler function with HANDLER attribute.
INLINE void SetHandler(int irq, irq_handler_t handler) { cb(); GetVTOR()[irq] = handler; dmb(); }

// get current interrupt IRQ_* service handler 
INLINE irq_handler_t GetHandler(int irq) { return GetVTOR()[irq]; }

// Enable/Disable interrupt of IRQ_* interrupt
INLINE void NVIC_IRQEnable(int irq) { NVIC->IENR[irq >> 5] = BIT(irq & 0x1f); }
INLINE void NVIC_IRQDisable(int irq) { NVIC->IRER[irq >> 5] = BIT(irq & 0x1f); isb(); }

// Check if IRQ_* interrupt is enabled
INLINE Bool NVIC_IRQIsEnabled(int irq) { return (NVIC->ISR[irq >> 5] & BIT(irq & 0x1f)) != 0; }

// Disable all interrupts
void NVIC_IRQAllDisable(void);

// Force IRQ_* interrupt, set pending
INLINE void NVIC_IRQForce(int irq) { NVIC->IPSR[irq >> 5] = BIT(irq & 0x1f); }

// Clear IRQ_* interrupt pending state
INLINE void NVIC_IRQClear(int irq) { NVIC->IPRR[irq >> 5] = BIT(irq & 0x1f); isb(); }

// Check if IRQ_* interrupt is pending
INLINE Bool NVIC_IRQIsPending(int irq) { return (NVIC->IPR[irq >> 5] & BIT(irq & 0x1f)) != 0; }

// Clear all interrupt pending states
void NVIC_IRQAllClear(void);

// Check if any interrupt is pending
INLINE Bool NVIC_IsPending(void) { return (NVIC->GISR & B9) != 0; }

// Check if IRQ_* interrupt is active (set up by hardware on interrupt handler enter, cleared on leave)
INLINE Bool NVIC_IRQIsActive(int irq) { return (NVIC->IACTR[irq >> 5] & BIT(irq & 0x1f)) != 0; }

// Check if any interrupt is active (executed right now)
INLINE Bool NVIC_IsActive(void) { return (NVIC->GISR & B8) != 0; }

// Set IRQ_* interrupt priority IRQ_PRIO_* (or 0..255, lower numer means higher priority)
INLINE void NVIC_IRQPriority(int irq, int prio) { NVIC->IPRIOR[irq] = prio; }

// Get current nesting status (3:level 2 interrupt, 1:level 1 interrupt, 0:no interrupt)
INLINE u8 NVIC_GetNest(void) { return (u8)NVIC->GISR; }

// Execute WFI command as WFE instruction
INLINE void WFItoWFE(void) { NVIC->SCTLR |= B3; }

// Set/check low power mode Sleep/DeepSleep
INLINE void NVIC_Sleep(void) { NVIC->SCTLR &= ~B2; }
INLINE void NVIC_DeepSleep(void) { NVIC->SCTLR |= B2; }
INLINE Bool NVIC_IsDeepSleep(void) { return (NVIC->SCTLR & B2) != 0; }

// Set low-power mode after leave interrupt
INLINE void NVIC_SleepOnExit(void) { NVIC->SCTLR |= B1; }

// Enter main program after leave interrupt
INLINE void NVIC_MainOnExit(void) { NVIC->SCTLR &= ~B1; }

// Set event - execute WFI command as WFE instruction
void sev(void);

// wait for event
void wfe(void);

// wait for event doubled, with set event
void wfe2(void);

// wait for interrupt
INLINE void wfi(void) { wfe2(); }

// Setup and enable VTF (Vector Table Free), channel 0..1 - set IRQ and handler address (must be two-byte alignment)
// - Need to mark handler function with HANDLER attribute.
void NVIC_VTFEnable(int ch, int irq, irq_handler_t handler);

// Disable VTF (Vector Table Free), channel 0..1
INLINE void NVIC_VTFDisable(int ch) { NVIC->VTFADDR[ch] = 0; }

// System reset
INLINE void SystemReset(void) { NVIC->CFGR = NVIC_KEY3 | B7; }

// === EXTI External Interrupt

// Enable/Disable EXTI line interrupt EXTI_LINE*
INLINE void EXTI_Enable(int line) { EXTI->INTENR |= BIT(line); }
INLINE void EXTI_Disable(int line) { EXTI->INTENR &= ~BIT(line); }

// Check if EXTI line interrupt is enabled EXTI_LINE*
INLINE Bool EXTI_IsEnabled(int line) { return (EXTI->INTENR & BIT(line)) != 0; }

// Enable/Disable EXTI line event EXTI_LINE*
INLINE void EXTI_EventEnable(int line) { EXTI->EVENR |= BIT(line); }
INLINE void EXTI_EventDisable(int line) { EXTI->EVENR &= ~BIT(line); }

// Check if EXTI line event is enabled EXTI_LINE*
INLINE Bool EXTI_EventIsEnabled(int line) { return (EXTI->EVENR & BIT(line)) != 0; }

// Enable/Disable EXTI line rising edge trigger EXTI_LINE*
INLINE void EXTI_RiseEnable(int line) { EXTI->RTENR |= BIT(line); }
INLINE void EXTI_RiseDisable(int line) { EXTI->RTENR &= ~BIT(line); }

// Check if EXTI line rising edge trigger is enabled EXTI_LINE*
INLINE Bool EXTI_RiseIsEnabled(int line) { return (EXTI->RTENR & BIT(line)) != 0; }

// Enable/Disable EXTI line falling edge trigger EXTI_LINE*
INLINE void EXTI_FallEnable(int line) { EXTI->FTENR |= BIT(line); }
INLINE void EXTI_FallDisable(int line) { EXTI->FTENR &= ~BIT(line); }

// Check if EXTI line falling edge trigger is enabled EXTI_LINE*
INLINE Bool EXTI_FallIsEnabled(int line) { return (EXTI->FTENR & BIT(line)) != 0; }

// Force/Unforce EXTI line interrupt EXTI_LINE* (software interrupt, set pending flag)
INLINE void EXTI_Force(int line) { EXTI->SWIEVR |= BIT(line); }
INLINE void EXTI_Unforce(int line) { EXTI->SWIEVR &= ~BIT(line); }

// Check if EXTI line interrupt is forced EXTI_LINE*
INLINE Bool EXTI_IsForced(int line) { return (EXTI->SWIEVR & BIT(line)) != 0; }

// Check if EXTI line interrupt is pending EXTI_LINE*
INLINE Bool EXTI_IsPending(int line) { return (EXTI->INTFR & BIT(line)) != 0; }

// Clear EXTI line pending flag EXTI_LINE*
INLINE void EXTI_Clear(int line) { EXTI->INTFR = BIT(line); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_IRQ_H

#endif // USE_IRQ
