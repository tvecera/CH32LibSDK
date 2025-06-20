
// ****************************************************************************
//
//                              Reset and Clock Control
//
// ****************************************************************************

#include "../_include.h"

// Reset reason - copy of the RCC->RSTSCKR register on system start (check using RCC_RESETON_* flags)
u32 ResetReason;

// current frequencies in [Hz]
u32 RCC_FreqTab[CLK_NUM] = {
	HSI_VALUE,	// #define CLK_HSI	0	// HSI oscillator frequency index (default 48000000)
	HSI_VALUE,	// #define CLK_SYSCLK	1	// SYSCLK frequency index (system frequency from clock sources) (default 48000000=HSI)
	HSI_VALUE/6,	// #define CLK_HCLK	2	// HCLK frequency index (main clock to periphery and flash, SYSCLK divided) (default 8000000=SYSCLK/6)
	HSI_VALUE/12,	// #define CLK_ADCCLK	3	// ADCCLK frequency index (ADC clock, HCLK divided) (default 4000000=HCLK/12)
};

// AHB clock divider table
const u16 RCC_AHBClkDivTab[16] = {
	1,	// #define RCC_SYSCLK_DIV1	0	// prescaler off
	2,	// #define RCC_SYSCLK_DIV2	1	// SYSCLK divided by 2
	3,	// #define RCC_SYSCLK_DIV3	2	// SYSCLK divided by 3
	4,	// #define RCC_SYSCLK_DIV4	3	// SYSCLK divided by 4
	5,	// #define RCC_SYSCLK_DIV5	4	// SYSCLK divided by 5
	6,	// #define RCC_SYSCLK_DIV6	5	// SYSCLK divided by 6
	7,	// #define RCC_SYSCLK_DIV7	6	// SYSCLK divided by 7
	8,	// #define RCC_SYSCLK_DIV8	7	// SYSCLK divided by 8

	2,	// #define RCC_SYSCLK_DIV2	8	// SYSCLK divided by 2
	4,	// #define RCC_SYSCLK_DIV4	9	// SYSCLK divided by 4
	8,	// #define RCC_SYSCLK_DIV8	10	// SYSCLK divided by 8
	16,	// #define RCC_SYSCLK_DIV16	11	// SYSCLK divided by 16
	32,	// #define RCC_SYSCLK_DIV32	12	// SYSCLK divided by 32
	64,	// #define RCC_SYSCLK_DIV64	13	// SYSCLK divided by 64
	128,	// #define RCC_SYSCLK_DIV128	14	// SYSCLK divided by 128
	256,	// #define RCC_SYSCLK_DIV256	15	// SYSCLK divided by 256
};

// Update frequencies in clock frequency table (should be called after change settings)
void RCC_UpdateFreq(void)
{
	// SYSCLK from HSI
	RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSI];

	// get AHB clock divider 1..256
	u16 ahbdiv = RCC_GetAHBDivVal();

	// get HCLK, PCLK1 and PCLK2 frequency
	u32 f = RCC_FreqTab[CLK_SYSCLK] / ahbdiv;
	RCC_FreqTab[CLK_HCLK] = f;

	// get ADC clock divider 2..128
//	u8 adcdiv = RCC_GetADCDivTab();
// @TODO

	// get ADCCLK frequency
	RCC_FreqTab[CLK_ADCCLK] = f;
}

// Initialize system clock to default configuration
void RCC_ClockInit(void)
{
	// Set flash slow wait state
	Flash_Latency(2);

	// enable HSI clock (should be already enabled)
	RCC_HSIEnable();

	// wait HSI to be stable and running
	while (!RCC_HSIIsStable()) {}

	// disable MCO output
	RCC_MCOClkSrc(RCC_MCO_NONE);

	// setup AHB clock divider
	int i;
#if SYSCLK_DIV <= 0
	i = 0;
#elif SYSCLK_DIV <= 8
	i = SYSCLK_DIV - 1;
#else
	for (i = 15; i > 8; i--) if (SYSCLK_DIV >= RCC_AHBClkDivTab[i]) break;
#endif
	RCC_AHBDiv(i);

	// Update frequencies in clock frequency table
	RCC_UpdateFreq();

	// Set flash wait state  (0 wait if sysclk <= 12MHz, 1 wait if sysclk > 12MHz and syclk <= 24MHz, 2 wait if sysclk > 24MHz)
	// Note: We need max. 1 wait state with VGA output on 25 MHz or display is noisy otherwise.
	Flash_Latency((RCC_FreqTab[CLK_HCLK] >= 26*MHZ) ? 2 : ((RCC_FreqTab[CLK_HCLK] >= 13*MHZ) ? 1 : 0));
}

// Reset system clock to default configuration (select HSI oscillator/3)
void RCC_ClockReset(void)
{
/*
	// Set flash slow wait state
	Flash_Latency(2);

	// enable HSI clock (should be already enabled)
	RCC_HSIEnable();

	// wait HSI to be stable and running
	while (!RCC_HSIIsStable()) {}

	// Select HSI clock source
	RCC_SysClkSrc(RCC_SYSCLKSRC_HSI);
	while (RCC_SysClk() != RCC_SYSCLKSRC_HSI) {}

	// disable PLL, CSS and HSE
	RCC->CTLR &= ~(B24|B19|B16);

	// setup default control
	// - disable PLL
	// - HSE current 1.6mA
	// - SCM disable
	// - HSE_LP enable
	// - CSS disable
	// - HSE bypass disable
	// - HSE disable
	// - HSI trim = 16
	// - HS_LP disbled
	// - HSI enable
	RCC->CTLR = B20|B7|B0;

	// setup default configuration
	// - ADC divider active
	// - ADC duty 1/2
	// - MCO disabled
	// - PLL HSI not divided
	// - ADC prescaler = /2
	// - HB clock divider = 3
	// - HSI is clock cource
	RCC->CFGR0 = B5;

	// Set flash default wait state
	Flash_Latency(0);

	// Update frequencies in clock frequency table
	RCC_UpdateFreq();
*/
}

// Initialize device
void DevInit();

// System initialize
void SystemInit(void)
{
	// save reset reason
	ResetReason = RCC->RSTSCKR;

	// Delay about 2ms allowing access of the programming tool
	int i;
	for (i = 5000; i > 0; i--) nop();

	// Initialize system clock to default configuration
	RCC_ClockInit();

	// Enable AHB clock
	u32 mask =
#if !defined(ENABLE_DMA1) || ENABLE_DMA1	// DMA1 module enable
		RCC_AHBCLK_DMA1 |
#endif
#if !defined(ENABLE_SRAM) || ENABLE_SRAM	// SRAM enable
		RCC_AHBCLK_SRAM |
#endif
#if !defined(ENABLE_USBFS) || ENABLE_USBFS	// USBFS module enable
		RCC_AHBCLK_USBFS |
#endif
#if !defined(ENABLE_USBPD) || ENABLE_USBPD	// USBPD module enable
		RCC_AHBCLK_USBPD |
#endif
		0;
	RCC_AHBClkEnable(mask);

	// Enable APB2 clock
	mask =
#if !defined(ENABLE_AFI) || ENABLE_AFI		// I/O auxiliary function enable
		RCC_APB2CLK_AFI |
#endif
#if !defined(ENABLE_PA) || ENABLE_PA		// PA port enable
		RCC_APB2CLK_PA |
#endif
#if !defined(ENABLE_PB) || ENABLE_PB		// PB port enable
		RCC_APB2CLK_PB |
#endif
#if !defined(ENABLE_PC) || ENABLE_PC		// PC port enable
		RCC_APB2CLK_PC |
#endif
#if !defined(ENABLE_ADC1) || ENABLE_ADC1	// ADC1 module enable
		RCC_APB2CLK_ADC1 |
#endif
#if !defined(ENABLE_TIM1) || ENABLE_TIM1	// TIM1 module enable
		RCC_APB2CLK_TIM1 |
#endif
#if !defined(ENABLE_SPI1) || ENABLE_SPI1	// SPI1 module enable
		RCC_APB2CLK_SPI1 |
#endif
#if !defined(ENABLE_USART1) || ENABLE_USART1	// USART1 module enable
		RCC_APB2CLK_USART1 |
#endif
		0;
	RCC_APB2ClkEnable(mask);

	// Enable APB1 clock
	mask =
#if !defined(ENABLE_TIM2) || ENABLE_TIM2	// TIM2 module enable
		RCC_APB1CLK_TIM2 |
#endif
#if !defined(ENABLE_TIM3) || ENABLE_TIM3	// TIM3 module enable
		RCC_APB1CLK_TIM3 |
#endif
#if !defined(ENABLE_WWDG) || ENABLE_WWDG	// Window watchdog module enable
		RCC_APB1CLK_WWDG |
#endif
#if !defined(ENABLE_USART2) || ENABLE_USART2	// USART2 module enable
		RCC_APB1CLK_USART2 |
#endif
#if !defined(ENABLE_USART3) || ENABLE_USART3	// USART3 module enable
		RCC_APB1CLK_USART3 |
#endif
#if !defined(ENABLE_USART4) || ENABLE_USART4	// USART4 module enable
		RCC_APB1CLK_USART4 |
#endif
#if !defined(ENABLE_I2C1) || ENABLE_I2C1	// I2C1 module enable
		RCC_APB1CLK_I2C1 |
#endif
#if !defined(ENABLE_PWR) || ENABLE_PWR		// Power module enable
		RCC_APB1CLK_PWR |
#endif
		0;
	RCC_APB1ClkEnable(mask);

	// Initialize SysTick
	SysTick_Init();

	// pre-initialize constructors
	extern void (*__preinit_array_start)();
	extern void (*__preinit_array_end)();
	void(**p)();
	for (p = &__preinit_array_start; p < &__preinit_array_end; p++) (*p)();

	// Initialize device
	DevInit();
}

// Terminate device
void DevTerm();

// System terminate
void SystemTerm(void)
{
	// Terminate device
	DevTerm();

	// disable all interrupt flags
	RCC_IntDisable();

	// Disable all interrupts
	NVIC_IRQAllDisable();

	// clear all interrupt flags
	RCC_IntClear();

	// Clear all interrupt pending states
	NVIC_IRQAllClear();

	// Reset SysTick
	SysTick_Reset();

	// Reset system clock to default configuration (set to HSI oscillator)
	RCC_ClockReset();

	// Reset peripherals, except PWR module
	RCC->APB2PRSTR = ~0;
	RCC->APB1PRSTR = ~B28;
	RCC->APB2PRSTR = 0;
	RCC->APB1PRSTR = 0;

	// disable clocks, except SRAM and PWR
	RCC->AHBPCENR = B2;
	RCC->APB2PCENR = 0;
	RCC->APB1PCENR = B28;
}

// exit application and reset to boot loader (execute software reset)
void ResetToBootLoader(void)
{
	// System terminate
	SystemTerm();

	// System reset
	SystemReset();
}
