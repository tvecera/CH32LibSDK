
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
	HSI_VALUE,	// #define CLK_HSI	0	// HSI oscillator frequency index (default 24000000)
	HSE_VALUE,	// #define CLK_HSE	1	// HSE oscillator frequency index (default 24000000)
	LSI_VALUE,	// #define CLK_LSI	2	// LSI oscillator frequency index (default 128000)
	HSI_VALUE,	// #define CLK_SYSCLK	3	// SYSCLK frequency index (system frequency from clock sources) (default 24000000=HSI)
	HSI_VALUE/3,	// #define CLK_HCLK	4	// HCLK frequency index (main clock to periphery and flash, SYSCLK divided) (default 8000000=SYSCLK/3)
	HSI_VALUE/6,	// #define CLK_ADCCLK	5	// ADCCLK frequency index (ADC clock, HCLK divided) (default 4000000=HCLK/2)
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

// ADC clock divider table
const u8 RCC_ADCClkDivTab[32] = {
	2,	// #define RCC_ADCCLK_DIV2	0	// AHBCLK divided by 2 as ADC clock
	2,	// #define RCC_ADCCLK_DIV2	1	// AHBCLK divided by 2 as ADC clock
	2,	// #define RCC_ADCCLK_DIV2	2	// AHBCLK divided by 2 as ADC clock
	2,	// #define RCC_ADCCLK_DIV2	3	// AHBCLK divided by 2 as ADC clock
	4,	// #define RCC_ADCCLK_DIV4	4	// AHBCLK divided by 4 as ADC clock
	8,	// #define RCC_ADCCLK_DIV8	5	// AHBCLK divided by 8 as ADC clock
	16,	// #define RCC_ADCCLK_DIV16	6	// AHBCLK divided by 16 as ADC clock
	32,	// #define RCC_ADCCLK_DIV32	7	// AHBCLK divided by 32 as ADC clock

	4,	// #define RCC_ADCCLK_DIV4	8	// AHBCLK divided by 4 as ADC clock
	4,	// #define RCC_ADCCLK_DIV4	9	// AHBCLK divided by 4 as ADC clock
	4,	// #define RCC_ADCCLK_DIV4	10	// AHBCLK divided by 4 as ADC clock
	4,	// #define RCC_ADCCLK_DIV4	11	// AHBCLK divided by 4 as ADC clock
	8,	// #define RCC_ADCCLK_DIV8	12	// AHBCLK divided by 8 as ADC clock
	16,	// #define RCC_ADCCLK_DIV16	13	// AHBCLK divided by 16 as ADC clock
	32,	// #define RCC_ADCCLK_DIV32	14	// AHBCLK divided by 32 as ADC clock
	64,	// #define RCC_ADCCLK_DIV64	15	// AHBCLK divided by 64 as ADC clock

	6,	// #define RCC_ADCCLK_DIV6	16	// AHBCLK divided by 6 as ADC clock
	6,	// #define RCC_ADCCLK_DIV6	17	// AHBCLK divided by 6 as ADC clock
	6,	// #define RCC_ADCCLK_DIV6	18	// AHBCLK divided by 6 as ADC clock
	6,	// #define RCC_ADCCLK_DIV6	19	// AHBCLK divided by 6 as ADC clock
	12,	// #define RCC_ADCCLK_DIV12	20	// AHBCLK divided by 12 as ADC clock
	24,	// #define RCC_ADCCLK_DIV24	21	// AHBCLK divided by 24 as ADC clock
	48,	// #define RCC_ADCCLK_DIV48	22	// AHBCLK divided by 48 as ADC clock
	96,	// #define RCC_ADCCLK_DIV96	23	// AHBCLK divided by 96 as ADC clock

	8,	// #define RCC_ADCCLK_DIV8	24	// AHBCLK divided by 8 as ADC clock
	8,	// #define RCC_ADCCLK_DIV8	25	// AHBCLK divided by 8 as ADC clock
	8,	// #define RCC_ADCCLK_DIV8	26	// AHBCLK divided by 8 as ADC clock
	8,	// #define RCC_ADCCLK_DIV8	27	// AHBCLK divided by 8 as ADC clock
	16,	// #define RCC_ADCCLK_DIV16	28	// AHBCLK divided by 16 as ADC clock
	32,	// #define RCC_ADCCLK_DIV32	29	// AHBCLK divided by 32 as ADC clock
	64,	// #define RCC_ADCCLK_DIV64	30	// AHBCLK divided by 64 as ADC clock
	128,	// #define RCC_ADCCLK_DIV128	31	// AHBCLK divided by 128 as ADC clock
};

// ADC clock divider search table
const u8 RCC_ADCClkDivSrcTab[2*12] = {
	2,	RCC_ADCCLK_DIV2,
	4,	RCC_ADCCLK_DIV4,
	6,	RCC_ADCCLK_DIV6,
	8,	RCC_ADCCLK_DIV8,
	12,	RCC_ADCCLK_DIV12,
	16,	RCC_ADCCLK_DIV16,
	24,	RCC_ADCCLK_DIV24,
	32,	RCC_ADCCLK_DIV32,
	48,	RCC_ADCCLK_DIV48,
	64,	RCC_ADCCLK_DIV64,
	96,	RCC_ADCCLK_DIV96,
	128,	RCC_ADCCLK_DIV128,
};

// Adjust HSI internal high-speed clock 24MHz by vendor setup
void RCC_HSIVendorTrim(void)
{
	u8 trim = RCC_HSIGetVendorTrim();
	if (trim == 0xff) trim = 16;
	RCC_HSITrim(trim & 0x1f);
}

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

		if (clksrc == RCC_PLLSRC_HSI)
			RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSI]*2;
		else
			RCC_FreqTab[CLK_SYSCLK] = RCC_FreqTab[CLK_HSE]*2;
	}

	// get AHB clock divider 1..256
	u16 ahbdiv = RCC_GetAHBDivVal();

	// get HCLK, PCLK1 and PCLK2 frequency
	u32 f = RCC_FreqTab[CLK_SYSCLK] / ahbdiv;
	RCC_FreqTab[CLK_HCLK] = f;

	// get ADC clock divider 2..128
	u8 adcdiv = RCC_GetADCDivVal();

	// get ADCCLK frequency
	RCC_FreqTab[CLK_ADCCLK] = f / adcdiv;
}

// Start HSE oscillator and wait to be stable (bypass = bypass HSE crystal; returns False on error)
Bool RCC_HSEStart(Bool bypass)
{
	// remap pins to use crystal
	RCC_AFIClkEnable();	// I/O auxiliary function module clock enable
	GPIO_Remap_PA1PA2(1);

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
	Flash_Latency(1);

	// HSI adjust calibration by vendor setup
	RCC_HSIVendorTrim();

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
#if SYSCLK_DIV <= 0
	i = 0;
#elif SYSCLK_DIV <= 8
	i = SYSCLK_DIV - 1;
#else
	for (i = 15; i > 8; i--) if (SYSCLK_DIV >= RCC_AHBClkDivTab[i]) break;
#endif
	RCC_AHBDiv(i);

	// Start HSE oscillator and wait to be stable (bypass = bypass HSE crystal; returns False on error)
// SYSCLK_SRC System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass
#if (SYSCLK_SRC==2) || (SYSCLK_SRC==5) // HSE crystall
	Bool hseok = RCC_HSEStart(False);
#elif (SYSCLK_SRC==3) || (SYSCLK_SRC==6) // HSE bypass
	Bool hseok = RCC_HSEStart(True);
#else
	Bool hseok = False; // HSI
#endif

	// select PLL source
#if (SYSCLK_SRC==4)
	RCC_PLLSrc(RCC_PLLSRC_HSI);
#elif (SYSCLK_SRC==5) || (SYSCLK_SRC==6)
	if (hseok)
		RCC_PLLSrc(RCC_PLLSRC_HSE);
	else
		RCC_PLLSrc(RCC_PLLSRC_HSI);
#endif

	// enable PLL, wait till ready and select clock source
#if (SYSCLK_SRC==4) || (SYSCLK_SRC==5) || (SYSCLK_SRC==6)
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
	for (i = 11; i > 0; i--) if (ADCCLK_DIV >= RCC_ADCClkDivSrcTab[2*i]) break;
	RCC_ADCDiv(RCC_ADCClkDivSrcTab[2*i+1]);

	// Update frequencies in clock frequency table
	RCC_UpdateFreq();

	// Set flash wait state (0 wait if sysclk <= 24MHz, 1 wait if sysclk >= 24MHz)
	// Note: We need fast flash with VGA output on 25 MHz or display is noisy otherwise.
	Flash_Latency((RCC_FreqTab[CLK_HCLK] >= 26*MHZ) ? 1 : 0);
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
#if !defined(ENABLE_WWDG) || ENABLE_WWDG	// Window watchdog module enable
		RCC_APB1CLK_WWDG |
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
