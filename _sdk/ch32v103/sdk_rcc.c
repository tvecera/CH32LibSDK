
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
	HSI_VALUE,	// #define CLK_HSI	0	// HSI oscillator frequency index (default 8000000)
	HSE_VALUE,	// #define CLK_HSE	1	// HSE oscillator frequency index (default 24000000)
	LSI_VALUE,	// #define CLK_LSI	2	// LSI oscillator frequency index (default 40000)
	LSE_VALUE,	// #define CLK_LSE	3	// LSE oscillator frequency index (default 32768)
	HSI_VALUE,	// #define CLK_SYSCLK	4	// SYSCLK frequency index (system frequency from clock sources) (default 24000000=HSI)
	HSI_VALUE/3,	// #define CLK_HCLK	5	// HCLK frequency index (main clock to periphery and flash, SYSCLK divided) (default 8000000=SYSCLK/3)
	HSI_VALUE/3,	// #define CLK_PCLK1	6	// PCLK1 frequency index (default 8000000=HCLK)
	HSI_VALUE/3,	// #define CLK_PCLK2	7	// PCLK2 frequency index (default 8000000=HCLK)
	HSI_VALUE/6,	// #define CLK_ADCCLK	8	// ADCCLK frequency index (ADC clock, HCLK divided) (default 4000000=HCLK/2)
	48000000,	// #define CLK_USBCLK	9	// USBCLK frequency index (should be 48MHz)
};

// AHB clock divider table (from SYSCLK)
const u16 RCC_AHBClkDivTab[16] = {
	1,	// #define RCC_SYSCLK_DIV1	0	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	1	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	2	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	3	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	4	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	5	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	6	// prescaler off
	1,	// #define RCC_SYSCLK_DIV1	7	// prescaler off

	2,	// #define RCC_SYSCLK_DIV2	8	// SYSCLK divided by 2
	4,	// #define RCC_SYSCLK_DIV4	9	// SYSCLK divided by 4
	8,	// #define RCC_SYSCLK_DIV8	10	// SYSCLK divided by 8
	16,	// #define RCC_SYSCLK_DIV16	11	// SYSCLK divided by 16
	64,	// #define RCC_SYSCLK_DIV64	12	// SYSCLK divided by 64
	128,	// #define RCC_SYSCLK_DIV128	13	// SYSCLK divided by 128
	256,	// #define RCC_SYSCLK_DIV256	14	// SYSCLK divided by 256
	512,	// #define RCC_SYSCLK_DIV512	15	// SYSCLK divided by 512
};

// APB1 and APB2 clock divider table (from HCLK)
const u8 RCC_APBClkDivTab[8] = {
	1,	// #define RCC_HCLK_DIV1	0	// HCLK not divided
	1,	// #define RCC_HCLK_DIV1	1	// HCLK not divided
	1,	// #define RCC_HCLK_DIV1	2	// HCLK not divided
	1,	// #define RCC_HCLK_DIV1	3	// HCLK not divided
	2,	// #define RCC_HCLK_DIV2	4	// HCLK divided by 2
	4,	// #define RCC_HCLK_DIV4	5	// HCLK divided by 4
	8,	// #define RCC_HCLK_DIV8	6	// HCLK divided by 8
	16,	// #define RCC_HCLK_DIV16	7	// HCLK divided by 16
};

// PLL multiplication table
const u8 RCC_PLLMulTab[16] = {
	2,	// #define RCC_PLL_MUL2		0	// PLL input clock * 2
	3,	// #define RCC_PLL_MUL3		1	// PLL input clock * 3
	4,	// #define RCC_PLL_MUL4		2	// PLL input clock * 4
	5,	// #define RCC_PLL_MUL5		3	// PLL input clock * 5
	6,	// #define RCC_PLL_MUL6		4	// PLL input clock * 6
	7,	// #define RCC_PLL_MUL7		5	// PLL input clock * 7
	8,	// #define RCC_PLL_MUL8		6	// PLL input clock * 8
	9,	// #define RCC_PLL_MUL9		7	// PLL input clock * 9
	10,	// #define RCC_PLL_MUL10	8	// PLL input clock * 10
	11,	// #define RCC_PLL_MUL11	9	// PLL input clock * 11
	12,	// #define RCC_PLL_MUL12	10	// PLL input clock * 12
	13,	// #define RCC_PLL_MUL13	11	// PLL input clock * 13
	14,	// #define RCC_PLL_MUL14	12	// PLL input clock * 14
	15,	// #define RCC_PLL_MUL15	13	// PLL input clock * 15
	16,	// #define RCC_PLL_MUL16	14	// PLL input clock * 16
	18,	// #define RCC_PLL_MUL18	15	// PLL input clock * 18
};

// Update frequencies in clock frequency table (should be called after change settings)
void RCC_UpdateFreq(void)
{
	// get current clock source RCC_SYSCLKSRC_*
	int clksrc = RCC_SysClk();

	// get SYSCLK frequency
	switch (clksrc)
	{
//	case RCC_SYSCLKSRC_HSI:		// HSI as system clock
	default:
		RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSI];
		break;

	case RCC_SYSCLKSRC_HSE:		// HSE as system clock
		RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSE];
		break;

	case RCC_SYSCLKSRC_PLL:		// PLL as system clock
		// get PLL clock source RCC_PLLSRC_*
		clksrc = RCC_GetPLLSrc();

		// get PLL multiplication
		int pllmul = RCC_GetPLLMulVal();

		if (clksrc == RCC_PLLSRC_HSI)
		{
			// HSI source
			if (RCC_PLLHSIIsDiv()) // HSI is divided
				RCC_FreqTab[CLK_SYSCLK] = (RCC_FreqTab[CLK_HSI]>>1)*pllmul;
			else
				RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSI]*pllmul;
		}
		else
		{
			// HSE source
			if (RCC_PLLHSEIsDiv()) // HSE is divided
				RCC_FreqTab[CLK_SYSCLK] = (RCC_FreqTab[CLK_HSE]>>1)*pllmul;
			else
				RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSE]*pllmul;
		}
	}

	// get AHB clock divider 1..512, get HCLK
	u32 f = RCC_FreqTab[CLK_SYSCLK] / RCC_GetAHBDivVal();
	RCC_FreqTab[CLK_HCLK] = f;

	// get PCLK1 clock divider, get PCLK1
	RCC_FreqTab[CLK_PCLK1] = f / RCC_GetAPB1DivVal();

	// get PCLK2 clock divider, get PCLK2
	f /= RCC_GetAPB2DivVal();
	RCC_FreqTab[CLK_PCLK2] = f;

	// get ADCCLK frequency
	RCC_FreqTab[CLK_ADCCLK] = f / RCC_GetADCDivVal();
}

// Start HSE oscillator and wait to be stable (bypass = bypass HSE crystal; returns False on error)
Bool RCC_HSEStart(Bool bypass)
{
	// remap pins to use crystal
	RCC_AFIClkEnable();	// I/O auxiliary function module clock enable
	GPIO_Remap_PD0PD1(0);

	// setup bypass
	RCC_HSEBypassDisable();
	if (bypass) RCC_HSEBypassEnable();

	// enable HSE oscillator
	RCC_HSEEnable();

	// wait till HSE is running and stable
	int i;
	for (i = 50000; i > 0; i--) if (RCC_HSEIsStable()) break;

	// check time-out
	return (i != 0);
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

	// Select HSI clock source
	RCC_SysClkSrc(RCC_SYSCLKSRC_HSI);
	while (RCC_SysClk() != RCC_SYSCLKSRC_HSI) {}

	// disable MCO output
	RCC_MCOClkSrc(RCC_MCO_NONE);

	// disable PLL, CSS and HSE
	RCC->CTLR &= ~(B24|B19|B16);

	// disable HSE bypass
	RCC_HSEBypassDisable();

	// disable all interrupt flags
	RCC_IntDisable();

	// clear all interrupt flags
	RCC_IntClear();

	// setup AHB clock divider
	int i;
	for (i = 15; i > 0; i--) if (SYSCLK_DIV >= RCC_AHBClkDivTab[i]) break;
	RCC_AHBDiv(i);

	// setup APB1 divider
	for (i = 7; i > 0; i--) if (APB1CLK_DIV >= RCC_APBClkDivTab[i]) break;
	RCC_APB1Div(i);

	// setup APB2 divider
	for (i = 7; i > 0; i--) if (APB2CLK_DIV >= RCC_APBClkDivTab[i]) break;
	RCC_APB2Div(i);

	// Start HSE oscillator and wait to be stable (bypass = bypass HSE crystal; returns False on error)
// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
#if (SYSCLK_SRC==2) || (SYSCLK_SRC==5) || (SYSCLK_SRC==8) // HSE crystall
	Bool hseok = RCC_HSEStart(False);
#elif (SYSCLK_SRC==3) || (SYSCLK_SRC==6) || (SYSCLK_SRC==9) // HSE bypass
	Bool hseok = RCC_HSEStart(True);
#else
	Bool hseok = False;
#endif

	// select PLL source
#if (SYSCLK_SRC==4) || (SYSCLK_SRC==7) // HSI
#if (SYSCLK_SRC==7)
	RCC_PLLHSIDiv2(); // HSI/2
#else
	RCC_PLLHSIDiv1(); // HSI not divided
#endif
	RCC_PLLSrc(RCC_PLLSRC_HSI);
#elif (SYSCLK_SRC==5) || (SYSCLK_SRC==6) || (SYSCLK_SRC==8) || (SYSCLK_SRC==9) // HSE
#if (SYSCLK_SRC==8) || (SYSCLK_SRC==9)
	RCC_PLLHSEDiv2();	// HSE/2
#else
	RCC_PLLHSEDiv1();	// HSE not divided
#endif
	if (hseok)
		RCC_PLLSrc(RCC_PLLSRC_HSE); // set HSE source
	else
	{
		RCC_PLLHSIDiv2();
		RCC_PLLSrc(RCC_PLLSRC_HSI);
	}
#endif

	// enable PLL, wait till ready and select clock source
#if (SYSCLK_SRC>=4)
	for (i = 15; i > 0; i--) if (PLLCLK_MUL >= RCC_PLLMulTab[i]) break;
	RCC_PLLMul(i);	// set PLL multiplier
	RCC_PLLEnable();
	while (!RCC_PLLIsReady()) {}
	RCC_SysClkSrc(RCC_SYSCLKSRC_PLL);
	while (RCC_SysClk() != RCC_SYSCLKSRC_PLL) {}
#elif (SYSCLK_SRC == 2) || (SYSCLK_SRC == 3)
	if (hseok)
	{
		RCC_SysClkSrc(RCC_SYSCLKSRC_HSE);
		while (RCC_SysClk() != RCC_SYSCLKSRC_HSE) {}
	}
#endif

	// setup ADC clock divider
	i = ADCCLK_DIV/2-1;
	if (i < 0) i = 0;
	if (i > 3) i = 3;
	RCC_ADCDiv(i);

	// Update frequencies in clock frequency table
	RCC_UpdateFreq();

	// Set flash wait state  (0 wait if sysclk <= 24MHz, 1 wait if sysclk > 24MHz and syclk <= 48MHz, 2 wait if sysclk > 48MHz)
	// Note: We need fast flash with VGA output on 25 MHz.
	Flash_Latency((RCC_FreqTab[CLK_HCLK] >= 51*MHZ) ? 2 : ((RCC_FreqTab[CLK_HCLK] >= 26*MHZ) ? 1 : 0));
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
#if !defined(ENABLE_FLASH) || ENABLE_FLASH	// FLASH enable
		RCC_AHBCLK_FLASH |
#endif
#if !defined(ENABLE_CRC) || ENABLE_CRC		// CRC module enable
		RCC_AHBCLK_CRC |
#endif
#if !defined(ENABLE_USBFS) || ENABLE_USBFS	// USBFS module enable
		RCC_AHBCLK_USBFS |
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
#if !defined(ENABLE_PD) || ENABLE_PD		// PD port enable
		RCC_APB2CLK_PD |
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
#if !defined(ENABLE_TIM4) || ENABLE_TIM4	// TIM4 module enable
		RCC_APB1CLK_TIM4 |
#endif
#if !defined(ENABLE_WWDG) || ENABLE_WWDG	// Window watchdog module enable
		RCC_APB1CLK_WWDG |
#endif
#if !defined(ENABLE_SPI2) || ENABLE_SPI2	// SPI2 module enable
		RCC_APB1CLK_SPI2 |
#endif
#if !defined(ENABLE_USART2) || ENABLE_USART2	// USART2 module enable
		RCC_APB1CLK_USART2 |
#endif
#if !defined(ENABLE_USART3) || ENABLE_USART3	// USART3 module enable
		RCC_APB1CLK_USART3 |
#endif
#if !defined(ENABLE_I2C1) || ENABLE_I2C1	// I2C1 module enable
		RCC_APB1CLK_I2C1 |
#endif
#if !defined(ENABLE_I2C2) || ENABLE_I2C2	// I2C2 module enable
		RCC_APB1CLK_I2C2 |
#endif
#if !defined(ENABLE_USBPD) || ENABLE_USBPD	// USBPD module enable
		RCC_APB1CLK_USBPD |
#endif
#if !defined(ENABLE_CAN1) || ENABLE_CAN1	// CAN1 module enable
		RCC_APB1CLK_CAN1 |
#endif
#if !defined(ENABLE_BKP) || ENABLE_BKP		// BKP module enable
		RCC_APB1CLK_BKP |
#endif
#if !defined(ENABLE_PWR) || ENABLE_PWR		// Power module enable
		RCC_APB1CLK_PWR |
#endif
#if !defined(ENABLE_DAC) || ENABLE_DAC		// DAC module enable
		RCC_APB1CLK_DAC |
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
