
// ****************************************************************************
//                                 
//                        Project library configuration
//
// ****************************************************************************

#ifndef _CONFIG_H
#define _CONFIG_H

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

// default font
#define FONT		FontBold8x8	// default system font
#define FONTCOND	FontCond6x6	// default condensed font

// ----------------------------------------------------------------------------
//                             Device setup
// ----------------------------------------------------------------------------

//#define DISP_SCK_GPIO		PA5		// SCK output to SPI display
//#define DISP_MOSI_GPIO	PA7		// MOSI output to SPI display
//#define DISP_RES_GPIO		PA12		// RES output to SPI display
//#define DISP_DC_GPIO		PA13		// DC output to SPI display
//#define DISP_CS_GPIO		PA4		// CS output to SPI display
//#define DISP_BLK_GPIO		PB4		// BLK output to SPI display
//#define DISP_SPI_MAP		0		// hardware display driver: display SPI mapping
//#define DISP_SPI_DIV		1		// hardware display driver: display SPI baud divider 0..7 (means div=2..256)
//#define DISP_SPEED		2		// software display driver: wait delay
//#define DISP_GAMMA		2		// select display gamma curve 0..15
//#define USE_DISP		2		// 1=use software display driver, 2=use hardware display driver (0=no driver)

//#define SD_CS_GPIO		PB12		// CS output to SD card
//#define SD_CLK_GPIO		PB9		// CLK output to SD card
//#define SD_MISO_GPIO		PB8		// MISO input from SD card
//#define SD_MOSI_GPIO		PB7		// MOSI output to SD card
//#define SD_SPEED_INIT		(HCLK_PER_US*2)	// SD speed on init: wait delay "HCLK_PER_US*2" = 250 kbps
//#define SD_SPEED_READ		(HCLK_PER_US/4)	// SD speed on read: wait delay "HCLK_PER_US/4" = 2 Mbps
//#define SD_SPEED_WRITE	(HCLK_PER_US/1)	// SD speed on write: wait delay "HCLK_PER_US/1" = 500 kbps
//#define USE_SD		1		// 1=use SD card driver

#define USE_DRAW		1		// 1=use graphics drawing functions
#define USE_PRINT		1		// 1=use text printing functions
#define USE_SOUND		2		// use sound support 1=tone, 2=melody
#define USE_SCREENSHOT		0		// 1=use screen shot

//#define USE_KEY		1		// 1=use keyboard support
//#define KEYCNT_REL		50		// keyboard counter - release interval in [ms]
//#define KEYCNT_PRESS		400		// keyboard counter - first repeat in [ms]
//#define KEYCNT_REPEAT		100		// keyboard counter - next repeat in [ms]

// ----------------------------------------------------------------------------
//                            Library modules
// ----------------------------------------------------------------------------
/*
#define USE_CRC		1	// 1=use CRC library
#define USE_DECNUM	1	// 1=use decode number
#define USE_FAT		1	// 1=use FAT filesystem
#define USE_RAND	1	// 1=use random number generator
*/
// ----------------------------------------------------------------------------
//                             SDK modules
// ----------------------------------------------------------------------------
/*
#define USE_ADC		1	// 1=use ADC peripheral
#define USE_DMA		1	// 1=use DMA peripheral
#define USE_FLASH	1	// 1=use Flash programming
#define USE_I2C		1	// 1=use I2C peripheral
#define USE_IRQ		1	// 1=use IRQ interrupt support
#define USE_PWR		1	// 1=use power control
#define USE_SPI		1	// 1=use SPI peripheral
#define USE_TIM		1	// 1=use timers
#define USE_USART	1	// 1=use USART peripheral
*/
// ----------------------------------------------------------------------------
//                            Clock Setup
// ----------------------------------------------------------------------------
/*
// frequency of HSI internal oscillator 48MHz
#define HSI_VALUE	48000000

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
#define SYSCLK_DIV	1

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
#define ADCCLK_DIV	8		// CH32V0: max. 24 MHz (48 / 8 = 6 MHz)

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#define HCLK_PER_US	48

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#define SYSTICK_MS	16
*/
// ----------------------------------------------------------------------------
//                          Peripheral clock enable
// ----------------------------------------------------------------------------
/*
// System
#define ENABLE_SRAM	1		// SRAM enable
#define ENABLE_FLASH	1		// FLASH enable
#define ENABLE_WWDG	0		// Window watchdog enable
#define ENABLE_PWR	1		// Power module enable
#define ENABLE_CRC	0		// CRC module enable
#define ENABLE_BKP	0		// Backup module enable
#define ENABLE_FSMC	0		// FSMC module enable
#define ENABLE_RNG	0		// RNG module enable
#define ENABLE_SDIO	0		// SDIO module enable
#define ENABLE_DVP	0		// DVP module enable
#define ENABLE_BLEC	0		// BLEC module enable
#define ENABLE_BLES	0		// BLES module enable
// Ports
#define ENABLE_AFI	1		// I/O auxiliary function enable
#define ENABLE_PA	1		// PA port enable
#define ENABLE_PB	1		// PB port enable
#define ENABLE_PC	1		// PC port enable
#define ENABLE_PD	0		// PD port enable
#define ENABLE_PE	0		// PE port enable
// ADC
#define ENABLE_ADC1	1		// ADC1 module enable
#define ENABLE_ADC2	0		// ADC2 module enable
// DAC
#define ENABLE_DAC	0		// DAC module enable
// Timers
#define ENABLE_TIM1	1		// TIM1 module enable
#define ENABLE_TIM2	1		// TIM2 module enable
#define ENABLE_TIM3	1		// TIM3 module enable
#define ENABLE_TIM4	0		// TIM4 module enable
#define ENABLE_TIM5	0		// TIM5 module enable
#define ENABLE_TIM6	0		// TIM6 module enable
#define ENABLE_TIM7	0		// TIM7 module enable
#define ENABLE_TIM8	0		// TIM8 module enable
#define ENABLE_TIM9	0		// TIM9 module enable
#define ENABLE_TIM10	0		// TIM10 module enable
#define ENABLE_LPTIM	0		// LPTIM module enable
// SPI
#define ENABLE_SPI1	1		// SPI1 module enable
#define ENABLE_SPI2	0		// SPI2 module enable
// USART
#define ENABLE_USART1	1		// USART1 module enable
#define ENABLE_USART2	0		// USART2 module enable
#define ENABLE_USART3	0		// USART3 module enable
#define ENABLE_USART4	1		// USART4 module enable
#define ENABLE_USART5	0		// USART5 module enable
#define ENABLE_USART6	0		// USART6 module enable
#define ENABLE_USART7	0		// USART7 module enable
#define ENABLE_USART8	0		// USART8 module enable
// I2C
#define ENABLE_I2C1	1		// I2C1 module enable
#define ENABLE_I2C2	0		// I2C2 module enable
// CAN
#define ENABLE_CAN1	0		// CAN1 module enable
#define ENABLE_CAN2	0		// CAN2 module enable
// DMA
#define ENABLE_DMA1	1		// DMA1 module enable
#define ENABLE_DMA2	0		// DMA2 module enable
// USB
#define ENABLE_USBFS	0		// USBFS module enable
#define ENABLE_USBPD	0		// USBPD module enable
#define ENABLE_USBD	0		// USBD module enable
#define ENABLE_USBHS	0		// USBHS module enable
#define ENABLE_USBOTG	0		// USBOTG module enable
// Ethernet
#define ENABLE_ETHMAC	0		// ETHMAC module enable
#define ENABLE_ETHMACTX	0		// ETHMACTX module enable
#define ENABLE_ETHMACRX	0		// ETHMACRX module enable
*/
#endif // _CONFIG_H
