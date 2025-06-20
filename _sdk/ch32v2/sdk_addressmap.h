
// ****************************************************************************
//
//                        Peripheral memory map
//
// ****************************************************************************
// Included from both C and assembly.

#ifndef _SDK_ADDRESSMAP_H
#define _SDK_ADDRESSMAP_H

#define	FLASH_BASE		0x08000000	// FLASH base address in the alias region
#define	SRAM_BASE		0x20000000	// SRAM base address in the alias region
#define	PERIPH_BASE		0x40000000	// Peripheral base address in the alias region
#define FSMC_R_BASE		0xA0000000	// FSMC registers base address

#define APB1PERIPH_BASE		PERIPH_BASE			// 0x40000000
#define APB2PERIPH_BASE		(PERIPH_BASE + 0x10000)		// 0x40010000
#define AHBPERIPH_BASE		(PERIPH_BASE + 0x20000)		// 0x40020000

#define TIM2_BASE		(APB1PERIPH_BASE + 0x0000)	// TIM2 0x40000000 general-purpose timer GPTM
#define TIM3_BASE		(APB1PERIPH_BASE + 0x0400)	// TIM3 0x40000400 general-purpose timer GPTM
#define TIM4_BASE		(APB1PERIPH_BASE + 0x0800)	// TIM4 0x40000800 general-purpose timer GPTM
#define TIM5_BASE		(APB1PERIPH_BASE + 0x0C00)	// TIM5 0x40000C00 general-purpose timer GPTM
#define TIM6_BASE		(APB1PERIPH_BASE + 0x1000)	// TIM6 0x40001000 basic timer BCTM
#define TIM7_BASE		(APB1PERIPH_BASE + 0x1400)	// TIM7 0x40001400 basic timer BCTM
#define USART6_BASE		(APB1PERIPH_BASE + 0x1800)	// USART6 0x40001800
#define USART7_BASE		(APB1PERIPH_BASE + 0x1C00)	// USART7 0x40001C00
#define USART8_BASE		(APB1PERIPH_BASE + 0x2000)	// USART8 0x40002000
#define RTC_BASE		(APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE		(APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE		(APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE		(APB1PERIPH_BASE + 0x3800)	// SPI2 0x40003800
#define SPI3_BASE		(APB1PERIPH_BASE + 0x3C00)	// SPI3 0x40003C00
#define USART2_BASE		(APB1PERIPH_BASE + 0x4400)	// USART2 0x40004400
#define USART3_BASE		(APB1PERIPH_BASE + 0x4800)	// USART3 0x40004800
#define USART4_BASE		(APB1PERIPH_BASE + 0x4C00)	// USART4 0x40004C00
#define USART5_BASE		(APB1PERIPH_BASE + 0x5000)	// USART5 0x40005000
#define I2C1_BASE		(APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE		(APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE		(APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE		(APB1PERIPH_BASE + 0x6800)
#define BKP_BASE		(APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE		(APB1PERIPH_BASE + 0x7000)	// Power control 0x40007000
#define DAC_BASE		(APB1PERIPH_BASE + 0x7400)

#define AFIO_BASE		(APB2PERIPH_BASE + 0x0000)	// alternative function select 0x40010000
#define EXTI_BASE		(APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE		(APB2PERIPH_BASE + 0x0800)	// PA port 0x40010800
#define GPIOB_BASE		(APB2PERIPH_BASE + 0x0C00)	// PB port 0x40010C00
#define GPIOC_BASE		(APB2PERIPH_BASE + 0x1000)	// PC port 0x40011000
#define GPIOD_BASE		(APB2PERIPH_BASE + 0x1400)	// PD port 0x40011400
#define GPIOE_BASE		(APB2PERIPH_BASE + 0x1800)	// PE port 0x40011800
#define ADC1_BASE		(APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE		(APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE		(APB2PERIPH_BASE + 0x2C00)	// TIM1 0x40012C00 advanced-control timer ADTM
#define SPI1_BASE		(APB2PERIPH_BASE + 0x3000)	// SPI1 0x40013000
#define TIM8_BASE		(APB2PERIPH_BASE + 0x3400)	// TIM8 0x40013400 advanced-control timer ADTM
#define USART1_BASE		(APB2PERIPH_BASE + 0x3800)	// USART1 0x40013800
#define ADC3_BASE		(APB2PERIPH_BASE + 0x3C00)
#define TIM9_BASE		(APB2PERIPH_BASE + 0x4C00)	// TIM9 0x40014C00 advanced-control timer ADTM
#define TIM10_BASE		(APB2PERIPH_BASE + 0x5000)	// TIM10 0x40015000 advanced-control timer ADTM
#define SDIO_BASE		(APB2PERIPH_BASE + 0x8000)

#define DMA1_BASE		(AHBPERIPH_BASE + 0x0000)	// DMA1 0x40020000
#define DMA1_Channel1_BASE	(AHBPERIPH_BASE + 0x0008)	// DMA1 channel 1 0x40020008
#define DMA1_Channel2_BASE	(AHBPERIPH_BASE + 0x001C)	// DMA1 channel 2 0x4002001C
#define DMA1_Channel3_BASE	(AHBPERIPH_BASE + 0x0030)	// DMA1 channel 3 0x40020030
#define DMA1_Channel4_BASE	(AHBPERIPH_BASE + 0x0044)	// DMA1 channel 4 0x40020044
#define DMA1_Channel5_BASE	(AHBPERIPH_BASE + 0x0058)	// DMA1 channel 5 0x40020058
#define DMA1_Channel6_BASE	(AHBPERIPH_BASE + 0x006C)	// DMA1 channel 6 0x4002006C
#define DMA1_Channel7_BASE	(AHBPERIPH_BASE + 0x0080)	// DMA1 channel 7 0x40020080
#define DMA1_Channel8_BASE	(AHBPERIPH_BASE + 0x0094)	// DMA1 channel 8 0x40020094
#define DMA2_BASE		(AHBPERIPH_BASE + 0x0400)	// DMA2 0x40020400
#define DMA2_Channel1_BASE	(AHBPERIPH_BASE + 0x0408)	// DMA2 channel 1 0x40020408
#define DMA2_Channel2_BASE	(AHBPERIPH_BASE + 0x041C)	// DMA2 channel 2 0x4002041C
#define DMA2_Channel3_BASE	(AHBPERIPH_BASE + 0x0430)	// DMA2 channel 3 0x40020430
#define DMA2_Channel4_BASE	(AHBPERIPH_BASE + 0x0444)	// DMA2 channel 4 0x40020444
#define DMA2_Channel5_BASE	(AHBPERIPH_BASE + 0x0458)	// DMA2 channel 5 0x40020458
#define DMA2_Channel6_BASE	(AHBPERIPH_BASE + 0x046C)	// DMA2 channel 6 0x4002046C
#define DMA2_Channel7_BASE	(AHBPERIPH_BASE + 0x0480)	// DMA2 channel 7 0x40020480
#define DMA2_Channel8_BASE	(AHBPERIPH_BASE + 0x0490)	// DMA2 channel 8 0x40020490
#define DMA2_Channel9_BASE	(AHBPERIPH_BASE + 0x04A0)	// DMA2 channel 8 0x400204A0
#define DMA2_Channel10_BASE	(AHBPERIPH_BASE + 0x04B0)	// DMA2 channel 8 0x400204B0
#define DMA2_Channel11_BASE	(AHBPERIPH_BASE + 0x04C0)	// DMA2 channel 8 0x400204C0
#define DMA2_EXTEN_BASE		(AHBPERIPH_BASE + 0x04D0)	// DMA2 extend interrupt status register 0x400204D0
#define RCC_BASE		(AHBPERIPH_BASE + 0x1000)	// Reset and clock control 0x40021000
#define FLASH_R_BASE		(AHBPERIPH_BASE + 0x2000)	// Flash registers base address 0x40022000
#define OSC_BASE		(AHBPERIPH_BASE + 0x202C)	// Oscilator registers 0x4002202C
#define CRC_BASE		(AHBPERIPH_BASE + 0x3000)
#define USBHS_BASE		(AHBPERIPH_BASE + 0x3400)
#define EXTEN_BASE		(AHBPERIPH_BASE + 0x3800)	// Enhanced Register 0x40023800
#define OPA_BASE		(AHBPERIPH_BASE + 0x3804)
#define RNG_BASE		(AHBPERIPH_BASE + 0x3C00)

#define ETH10M_BASE		(AHBPERIPH_BASE + 0x8000)
#define ETH_BASE		(AHBPERIPH_BASE + 0x8000)
#define ETH_MAC_BASE		ETH_BASE
#define ETH_MMC_BASE		(ETH_BASE + 0x0100)
#define ETH_PTP_BASE		(ETH_BASE + 0x0700)
#define ETH_DMA_BASE		(ETH_BASE + 0x1000)

#define USBFS_BASE		0x50000000
#define DVP_BASE		0x50050000

#define FSMC_Bank1_R_BASE	(FSMC_R_BASE + 0x0000) 
#define FSMC_Bank1E_R_BASE	(FSMC_R_BASE + 0x0104) 
#define FSMC_Bank2_R_BASE	(FSMC_R_BASE + 0x0060) 

#define OB_BASE			0x1FFFF800			// Flash Option Bytes base address
#define FEATURE_SIGN		0x1FFFF7D0
#define DBGMCU_BASE		0xE000D000
#define PFIC_BASE		0xE000E000			// Programmable Fast Interrupt Controller (PFIC)
#define SYSTICK_BASE		0xE000F000			// SysTick System counter

#endif // _SDK_ADDRESSMAP_H
