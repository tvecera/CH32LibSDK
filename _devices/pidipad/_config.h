
#define BOOTLOADER_SIZE	0x5000	// size of boot loader (20 KB; must be multiply of sector size 512 B)

// Videomodes (9 colors: 1 black background + 8 foreground colors):
//	1 ... graphics mode 160x120 pixels mono with color attributes 8x8 pixels, required memory 2400+150 = 2550 B (driver size 548 B in RAM)
//	2 ... graphics mode 160x120 pixels mono with color attributes 4x4 pixels, required memory 2400+600 = 3000 B (driver size 540 B in RAM)
//	3 ... graphics mode 160x120 pixels mono with color attributes 2x2 pixels, required memory 2400+2400 = 4800 B (driver size 532 B in RAM)
//	4 ... graphics mode 256x192 pixels mono with color attributes 8x8 pixels, required memory 6144+384 = 6528 B (driver size 516 B in RAM)
//	5 ... graphics mode 144x96 pixels with 8 colors, required memory 6912 B (driver size 508 B in RAM ... Cannot be used with an SD card due to insufficient RAM)
//	6 ... text mode 40x30 characters of 8x8 pixels (resolution 320x240 pixels, pseudographics 80x60 pixels) with color attributes, font 2048 B in Flash, required memory 1200+600=1800 B
//	7 ... text mode 40x30 characters of 8x8 pixels (resolution 320x240 pixels, pseudographics 80x60 pixels) with color attributes, font 2048 B in RAM FontBuf, required memory 1200+600+2048=3848 B
//	8 ... text mode 80x30 characters of 8x8 pixels (resolution 640x240 pixels, pseudographics 160x60 pixels) with color attributes, font 2048 B in RAM FontBuf, required memory 2400+1200+2048=5648 B (driver size 504 B in RAM)
//	9 ... graphics mode 128x80 pixels mono with color attributes 1x1 pixel, required memory 1280+5120 = 6400 B
#ifndef VMODE
#define VMODE	1
#endif

// default device setup
#ifndef USE_DRAW
#define USE_DRAW	1	// 1=use graphics drawing functions
#endif

#ifndef USE_PRINT
#define USE_PRINT	1	// 1=use text printing functions
#endif

#ifndef USE_KEY
#define USE_KEY		1	// 1=use keyboard support
#endif

#ifndef USE_SOUND
#define USE_SOUND	1	// 1=use sound support
#endif

#ifndef USE_DISP
#define USE_DISP	1	// 1=use display support
#endif

#ifndef KEYCNT_REL
#define KEYCNT_REL	4	// keyboard counter - release interval in 1/60 sec
#endif

#ifndef KEYCNT_PRESS
#define KEYCNT_PRESS	20	// keyboard counter - first repeat in 1/60 sec
#endif

#ifndef KEYCNT_REPEAT
#define KEYCNT_REPEAT	6	// keyboard counter - next repeat in 1/60 sec
#endif

// SD card GPIOs
#define SD_CS_GPIO	PD3	// CS output to SD card
#define SD_CLK_GPIO	PD4	// CLK output to SD card
#define SD_MISO_GPIO	PD5	// MISO input from SD card
#define SD_MOSI_GPIO	PD6	// MOSI output to SD card
#define SD_SPEED_INIT	(HCLK_PER_US*2)	// SD speed on init: wait delay "HCLK_PER_US*2" = 250 kbps
#define SD_SPEED_READ	(HCLK_PER_US/8)	// SD speed on read: wait delay "HCLK_PER_US/8" = 4 Mbps
#define SD_SPEED_WRITE	(HCLK_PER_US/2)	// SD speed on read: wait delay "HCLK_PER_US/2" = 1 Mbps

// Frequency of HSE external oscillator
#ifndef HSE_VALUE
#define HSE_VALUE	25000000
#endif

// frequency of HSI internal oscillator 24MHz
#ifndef HSI_VALUE
#define HSI_VALUE	25000000
#endif

// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
#ifndef SYSCLK_SRC
#define SYSCLK_SRC	5
#endif

// PLL multiplier
#ifndef PLLCLK_MUL
#define PLLCLK_MUL	2		// only *2 supported; 24 MHz * 2 = 48 MHz
#endif

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
#ifndef SYSCLK_DIV
#define SYSCLK_DIV	1
#endif

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
#ifndef ADCCLK_DIV
#define ADCCLK_DIV	2		// CH32V0: max. 24 MHz (48 / 2 = 24 MHz)
#endif

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef HCLK_PER_US
#define HCLK_PER_US	50
#endif

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#ifndef SYSTICK_MS
#define SYSTICK_MS	0
#endif

// Periferals
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

#ifndef ENABLE_AFI
#define ENABLE_AFI	1		// I/O auxiliary function enable
#endif

#ifndef ENABLE_PA
#define ENABLE_PA	1		// PA port enable
#endif

#ifndef ENABLE_PB
#define ENABLE_PB	0		// PB port enable
#endif

#ifndef ENABLE_PC
#define ENABLE_PC	1		// PC port enable
#endif

#ifndef ENABLE_PD
#define ENABLE_PD	1		// PD port enable
#endif

#ifndef ENABLE_TIM1
#define ENABLE_TIM1	1		// TIM1 module enable
#endif

#ifndef ENABLE_TIM2
#define ENABLE_TIM2	1		// TIM2 module enable
#endif

#ifndef ENABLE_SPI1
#define ENABLE_SPI1	1		// SPI1 module enable
#endif

#ifndef ENABLE_USART1
#define ENABLE_USART1	0		// USART1 module enable
#endif

#ifndef ENABLE_I2C1
#define ENABLE_I2C1	0		// I2C1 module enable
#endif
