
// Pre-set defines (use #if to check):
//	target MCU	MCU serie	MCU class	MCU subclass
//	CH32V002x4	CH32V002	CH32V0		CH32V00X
//	CH32V003x4	CH32V003	CH32V0
//	CH32V004x6	CH32V004	CH32V0		CH32V00X
//	CH32V005x6	CH32V005	CH32V0		CH32V00X
//	CH32V006x4	CH32V006	CH32V0		CH32V00X
//	CH32V006x8	CH32V006	CH32V0		CH32V00X
//	CH32V007x8	CH32V007	CH32V0		CH32V00X
//	CH32X033x8	CH32V033	CH32V0		CH32V03X
//	CH32X035x7	CH32V035	CH32V0		CH32V03X
//	CH32X035x8	CH32V035	CH32V0		CH32V03X
//	CH32V103x6	CH32V103	CH32V1
//	CH32V103x8	CH32V103	CH32V1
//	CH32L103x8	CH32L103	CH32V1

// FLASHSIZE ... Flash size in bytes
// RAMSIZE ... RAM size in bytes
// STACKSIZE ... Stack size in bytes

// ----------------------------------------------------------------------------
//                            Clock Setup
// ----------------------------------------------------------------------------

// Frequency of HSE external oscillator
#ifndef HSE_VALUE
#if CH32V103
#define HSE_VALUE	16000000	// CH32V103: 3..16 MHz
#else
#define HSE_VALUE	24000000	// CH32V0, CH32L103: 4..25 MHz
#endif
#endif

// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
#ifndef SYSCLK_SRC
#if CH32V03X
#define SYSCLK_SRC	1		// CH32V03X supports only HSI
#else
#define SYSCLK_SRC	4
#endif
#endif

// PLL multiplier
#ifndef PLLCLK_MUL
#if CH32V103
#define PLLCLK_MUL	9		// PLL multiplier 2 - 18; max. output 72MHz; 8 MHz * 9 = 72 MHz
#else
#define PLLCLK_MUL	2		// only *2 supported; 24 MHz * 2 = 48 MHz
#endif
#endif

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
#ifndef SYSCLK_DIV
#define SYSCLK_DIV	1
#endif

// APB1 PCLK1 divider: 1, 2, 4, 8, 16 (max. output 72MHz)
#ifndef APB1CLK_DIV
#if CH32V103
#define APB1CLK_DIV	2
#endif
#endif

// APB2 PCLK1 divider: 1, 2, 4, 8, 16 (max. output 72MHz)
#ifndef APB2CLK_DIV
#if CH32V103
#define APB2CLK_DIV	1
#endif
#endif

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
#ifndef ADCCLK_DIV
#if CH32V103
#define ADCCLK_DIV	6		// CH32V103: max. 14 MHz (72 / 6 = 12 MHz)
#else
#define ADCCLK_DIV	2		// CH32V0: max. 24 MHz (48 / 2 = 24 MHz)
#endif
#endif

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef HCLK_PER_US
#if CH32V2 || CH32V4
#define HCLK_PER_US	144
#elif CH32L103
#define HCLK_PER_US	72
#elif CH32V103
#define HCLK_PER_US	9		// CH32V103 supports only HCLK/8 (72/8=9)
#else
#define HCLK_PER_US	48
#endif
#endig

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#ifndef SYSTICK_MS
#define SYSTICK_MS	5
#endif

// ----------------------------------------------------------------------------
//                          Peripheral clock enable
// ----------------------------------------------------------------------------

// System
#ifndef ENABLE_SRAM
#define ENABLE_SRAM	1		// SRAM enable
#endif

#ifndef ENABLE_FLASH
#define ENABLE_FLASH	1		// FLASH enable
#endif

#ifndef ENABLE_WWDG
#define ENABLE_WWDG	1		// Window watchdog enable
#endif

#ifndef ENABLE_PWR
#define ENABLE_PWR	1		// Power module enable
#endif

#ifndef ENABLE_CRC
#define ENABLE_CRC	1		// CRC module enable
#endif

#ifndef ENABLE_BKP
#define ENABLE_BKP	1		// Backup module enable
#endif

#ifndef ENABLE_FSMC
#define ENABLE_FSMC	1		// FSMC module enable
#endif

#ifndef ENABLE_RNG
#define ENABLE_RNG	1		// RNG module enable
#endif

#ifndef ENABLE_SDIO
#define ENABLE_SDIO	1		// SDIO module enable
#endif

#ifndef ENABLE_DVP
#define ENABLE_DVP	1		// DVP module enable
#endif

#ifndef ENABLE_BLEC
#define ENABLE_BLEC	1		// BLEC module enable
#endif

#ifndef ENABLE_BLES
#define ENABLE_BLES	1		// BLES module enable
#endif

// Ports
#ifndef ENABLE_AFI
#define ENABLE_AFI	1		// I/O auxiliary function enable
#endif

#ifndef ENABLE_PA
#define ENABLE_PA	1		// PA port enable
#endif

#ifndef ENABLE_PB
#define ENABLE_PB	1		// PB port enable
#endif

#ifndef ENABLE_PC
#define ENABLE_PC	1		// PC port enable
#endif

#ifndef ENABLE_PD
#define ENABLE_PD	1		// PD port enable
#endif

#ifndef ENABLE_PE
#define ENABLE_PE	1		// PE port enable
#endif

// ADC
#ifndef ENABLE_ADC1
#define ENABLE_ADC1	1		// ADC1 module enable
#endif

#ifndef ENABLE_ADC2
#define ENABLE_ADC2	1		// ADC2 module enable
#endif

// DAC
#ifndef ENABLE_DAC
#define ENABLE_DAC	1		// DAC module enable
#endif

// Timers
#ifndef ENABLE_TIM1
#define ENABLE_TIM1	1		// TIM1 module enable
#endif

#ifndef ENABLE_TIM2
#define ENABLE_TIM2	1		// TIM2 module enable
#endif

#ifndef ENABLE_TIM3
#define ENABLE_TIM3	1		// TIM3 module enable
#endif

#ifndef ENABLE_TIM4
#define ENABLE_TIM4	1		// TIM4 module enable
#endif

#ifndef ENABLE_TIM5
#define ENABLE_TIM5	1		// TIM5 module enable
#endif

#ifndef ENABLE_TIM6
#define ENABLE_TIM6	1		// TIM6 module enable
#endif

#ifndef ENABLE_TIM7
#define ENABLE_TIM7	1		// TIM7 module enable
#endif

#ifndef ENABLE_TIM8
#define ENABLE_TIM8	1		// TIM8 module enable
#endif

#ifndef ENABLE_TIM9
#define ENABLE_TIM9	1		// TIM9 module enable
#endif

#ifndef ENABLE_TIM10
#define ENABLE_TIM10	1		// TIM10 module enable
#endif

#ifndef ENABLE_LPTIM
#define ENABLE_LPTIM	1		// LPTIM module enable
#endif

// SPI
#ifndef ENABLE_SPI1
#define ENABLE_SPI1	1		// SPI1 module enable
#endif

#ifndef ENABLE_SPI2
#define ENABLE_SPI2	1		// SPI2 module enable
#endif

// USART
#ifndef ENABLE_USART1
#define ENABLE_USART1	1		// USART1 module enable
#endif

#ifndef ENABLE_USART2
#define ENABLE_USART2	1		// USART2 module enable
#endif

#ifndef ENABLE_USART3
#define ENABLE_USART3	1		// USART3 module enable
#endif

#ifndef ENABLE_USART4
#define ENABLE_USART4	1		// USART4 module enable
#endif

#ifndef ENABLE_USART5
#define ENABLE_USART5	1		// USART5 module enable
#endif

#ifndef ENABLE_USART6
#define ENABLE_USART6	1		// USART6 module enable
#endif

#ifndef ENABLE_USART7
#define ENABLE_USART7	1		// USART7 module enable
#endif

#ifndef ENABLE_USART8
#define ENABLE_USART8	1		// USART8 module enable
#endif

// I2C
#ifndef ENABLE_I2C1
#define ENABLE_I2C1	1		// I2C1 module enable
#endif

#ifndef ENABLE_I2C2
#define ENABLE_I2C2	1		// I2C2 module enable
#endif

// CAN
#ifndef ENABLE_CAN1
#define ENABLE_CAN1	1		// CAN1 module enable
#endif

#ifndef ENABLE_CAN2
#define ENABLE_CAN2	1		// CAN2 module enable
#endif

// DMA
#ifndef ENABLE_DMA1
#define ENABLE_DMA1	1		// DMA1 module enable
#endif

#ifndef ENABLE_DMA2
#define ENABLE_DMA2	1		// DMA2 module enable
#endif

// USB
#ifndef ENABLE_USBFS
#define ENABLE_USBFS	1		// USBFS module enable
#endif

#ifndef ENABLE_USBPD
#define ENABLE_USBPD	1		// USBPD module enable
#endif

#ifndef ENABLE_USBD
#define ENABLE_USBD	1		// USBD module enable
#endif

#ifndef ENABLE_USBHS
#define ENABLE_USBHS	1		// USBHS module enable
#endif

#ifndef ENABLE_USBOTG
#define ENABLE_USBOTG	1		// USBOTG module enable
#endif

// Ethernet
#ifndef ENABLE_ETHMAC
#define ENABLE_ETHMAC	1		// ETHMAC module enable
#endif

#ifndef ENABLE_ETHMACTX
#define ENABLE_ETHMACTX	1		// ETHMACTX module enable
#endif

#ifndef ENABLE_ETHMACRX
#define ENABLE_ETHMACRX	1		// ETHMACRX module enable
#endif

#endif // _CONFIG_H
