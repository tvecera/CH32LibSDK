
// ****************************************************************************
//
//                              Reset and Clock Control
//
// ****************************************************************************

#include "../../includes.h"	// globals

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
	// Set flash slow wait state
	Flash_Latency(2);

	// Update frequencies in clock frequency table
	RCC_UpdateFreq();
}
