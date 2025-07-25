
// ****************************************************************************
//
//                              Reset and Clock Control
//
// ****************************************************************************

#ifndef _SDK_RCC_H
#define _SDK_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

// default frequencies
#ifndef HSI_VALUE
#define HSI_VALUE	48000000	// frequency of HSI internal oscillator 48MHz
#endif

#ifndef SYSCLK_DIV
#define SYSCLK_DIV	1		// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256
#endif

// Reset and clock control 0x40021000
typedef struct {
	io32	CTLR;		// 0x00: Clock control register
	io32	CFGR0;		// 0x04: Clock configuration register 0
	io32	res1;		// 0x08: ...reserved
	io32	APB2PRSTR;	// 0x0C: PB2 peripheral reset register
	io32	APB1PRSTR;	// 0x10: PB1 peripheral reset register
	io32	AHBPCENR;	// 0x14: HB peripheral clock enable register
	io32	APB2PCENR;	// 0x18: PB2 peripheral clock enable register
	io32	APB1PCENR;	// 0x1C: PB1 peripheral clock enable register
	io32	res2;		// 0x20: ...reserved
	io32	RSTSCKR;	// 0x24: Control/status register
	io32	AHBRSTR;	// 0x28: HB peripheral reset register
} RCC_t;
STATIC_ASSERT(sizeof(RCC_t) == 0x2C, "Incorrect RCC_t!");
#define RCC	((RCC_t*)RCC_BASE)

// AHB clock divider
#define RCC_SYSCLK_DIV1		0	// prescaler off
//#define RCC_SYSCLK_DIV2	1	// SYSCLK divided by 2
#define RCC_SYSCLK_DIV3		2	// SYSCLK divided by 3
//#define RCC_SYSCLK_DIV4	3	// SYSCLK divided by 4
#define RCC_SYSCLK_DIV5		4	// SYSCLK divided by 5
#define RCC_SYSCLK_DIV6		5	// SYSCLK divided by 6
#define RCC_SYSCLK_DIV7		6	// SYSCLK divided by 7
//#define RCC_SYSCLK_DIV8	7	// SYSCLK divided by 8
#define RCC_SYSCLK_DIV2		8	// SYSCLK divided by 2
#define RCC_SYSCLK_DIV4		9	// SYSCLK divided by 4
#define RCC_SYSCLK_DIV8		10	// SYSCLK divided by 8
#define RCC_SYSCLK_DIV16	11	// SYSCLK divided by 16
#define RCC_SYSCLK_DIV32	12	// SYSCLK divided by 32
#define RCC_SYSCLK_DIV64	13	// SYSCLK divided by 64
#define RCC_SYSCLK_DIV128	14	// SYSCLK divided by 128
#define RCC_SYSCLK_DIV256	15	// SYSCLK divided by 256

// MCO pin clock output
#define RCC_MCO_NONE		0	// (0-3,6,7) no clock output
#define RCC_MCO_SYSCLK		4	// system clock
#define RCC_MCO_HSI		5	// HSI internal oscillator 48MHz

// AHB peripheral clock mask
#define RCC_AHBCLK_DMA1		B0	// DMA1 module clock mask
#define RCC_AHBCLK_SRAM		B2	// SRAM clock mask
#define RCC_AHBCLK_USBFS	B12	// USBFS clock mask
#define RCC_AHBCLK_USBPD	B17	// USBPD clock mask

// APB2 peripheral clock mask
#define RCC_APB2CLK_AFI		B0	// I/O auxiliary function module clock mask
#define RCC_APB2CLK_PA		B2	// PA port module clock mask
#define RCC_APB2CLK_PB		B3	// PB port module clock mask
#define RCC_APB2CLK_PC		B4	// PC port module clock mask
#define RCC_APB2CLK_ADC1	B9	// ADC1 module clock mask
#define RCC_APB2CLK_TIM1	B11	// TIM1 module clock mask
#define RCC_APB2CLK_SPI1	B12	// SPI1 module clock mask
#define RCC_APB2CLK_USART1	B14	// USART1 module clock mask

// APB1 peripheral clock mask
#define RCC_APB1CLK_TIM2	B0	// TIM2 module clock mask
#define RCC_APB1CLK_WWDG	B11	// Window watchdog clock mask
#define RCC_APB1CLK_I2C1	B21	// I2C1 module clock mask
#define RCC_APB1CLK_PWR		B28	// Power module clock mask
#define RCC_APB1CLK_TIM3	B1	// TIM3 module clock mask
#define RCC_APB1CLK_USART2	B17	// USART2 module clock mask
#define RCC_APB1CLK_USART3	B18	// USART3 module clock mask
#define RCC_APB1CLK_USART4	B19	// USART4 module clock mask

// Reset reason - copy of the RCC->RSTSCKR register on system start (check using RCC_RESETON_* flags)
#define RCC_RESETON_LPWR	B31	// Low-power reset
#define RCC_RESETON_WWD		B30	// Windows watchdog reset
#define RCC_RESETON_IWD		B29	// Independent watchdog reset
#define RCC_RESETON_SW		B28	// software reset
#define RCC_RESETON_PWR		B27	// power-on reset
#define RCC_RESETON_RST		B26	// manual reset on NRST pin
#define RCC_RESETON_OPA		B25	// OPA CMP reset
extern u32 ResetReason;

// AHB clock divider table
extern const u16 RCC_AHBClkDivTab[16];

// ADC clock divider table
//extern const u8 RCC_ADCClkDivTab[32];

// Current frequencies
#define CLK_HSI		0	// HSI oscillator frequency index
#define CLK_SYSCLK	1	// SYSCLK frequency index (system frequency from clock sources)
#define CLK_HCLK	2	// HCLK frequency index (main clock to periphery and flash, SYSCLK divided)
#define CLK_ADCCLK	3	// ADCCLK frequency index (ADC clock, HCLK divided)

#define CLK_NUM		4	// number of frequency indices

extern u32 RCC_FreqTab[CLK_NUM];	// current frequencies in [Hz]

#define SystemCoreClock RCC_FreqTab[CLK_HCLK] // system core clock

// --- HSI high-speed internal oscillator 48MHz

// Enable/Disable HSI high-speed internal oscillator 48MHz (default enabled)
INLINE void RCC_HSIEnable(void) { RCC->CTLR |= B0; }
INLINE void RCC_HSIDisable(void) { RCC->CTLR &= ~B0; }

// Check if HSI high-speed internal oscillator 48MHz is stable
INLINE Bool RCC_HSIIsStable(void) { return (RCC->CTLR & B1) != 0; }

// Get current adjustment value of the HSI internal high-speed clock 48MHz (0..31; default 16)
//  Clock adjustment value is in range 0..31, with default value 16.
//  Oscillator frequency 48MHz can be adjusted by 110KHz per step.
INLINE u8 RCC_HSIGetTrim(void) { return (u8)((RCC->CTLR >> 3) & 0x1f); }

// Set adjustment value of the HSI internal high-speed clock 48MHz (0..31)
//  Clock adjustment value is in range 0..31, with default value 16.
//  Oscillator frequency 48MHz can be adjusted by 110KHz per step.
INLINE void RCC_HSITrim(int trim) { RCC->CTLR = (RCC->CTLR & ~(0x1f << 3)) | (trim << 3); }

// Get calibration value of the HSI internal high-speed clock 48MHz (0..31)
INLINE u8 RCC_HSIGetCal(void) { return (u8)((RCC->CTLR >> 8) & 0xff); }

// --- Clock source

// Set AHB clock divider RCC_SYSCLK_DIV* (default 2 = divide by 3)
INLINE void RCC_AHBDiv(int div) { RCC->CFGR0 = (RCC->CFGR0 & ~(0x0f << 4)) | (div << 4); }

// Get AHB clock divider RCC_SYSCLK_DIV*
INLINE u8 RCC_GetAHBDiv(void) { return (u8)((RCC->CFGR0 >> 4) & 0x0f); }

// Get AHB clock divider value 1..256
INLINE u16 RCC_GetAHBDivVal(void) { return RCC_AHBClkDivTab[RCC_GetAHBDiv()]; }

// Set MCO pin clock output source RCC_MCO_* (default 0 = disabled)
INLINE void RCC_MCOClkSrc(int src) { RCC->CFGR0 = (RCC->CFGR0 & ~(7 << 24)) | (src << 24); }

// Get clock frequency in [Hz] from frequency table CLK_*
INLINE u32 RCC_GetFreq(int inx) { return RCC_FreqTab[inx]; }

// Update frequencies in clock frequency table (should be called after change settings)
void RCC_UpdateFreq(void);

// Initialize system clock to default configuration
void RCC_ClockInit(void);

// Reset system clock to default configuration (select HSI oscillator/3)
void RCC_ClockReset(void);

// --- Peripheral reset

// I/O auxiliary function module reset ON/OFF/Reset
INLINE void RCC_AFIResetOn(void) { RCC->APB2PRSTR |= B0; cb(); }
INLINE void RCC_AFIResetOff(void) { RCC->APB2PRSTR &= ~B0; cb(); }
INLINE void RCC_AFIReset(void) { RCC_AFIResetOn(); nop(); RCC_AFIResetOff(); }

// PA port module reset ON/OFF/Reset
INLINE void RCC_PAResetOn(void) { RCC->APB2PRSTR |= B2; cb(); }
INLINE void RCC_PAResetOff(void) { RCC->APB2PRSTR &= ~B2; cb(); }
INLINE void RCC_PAReset(void) { RCC_PAResetOn(); nop(); RCC_PAResetOff(); }

// PB port module reset ON/OFF/Reset
INLINE void RCC_PBResetOn(void) { RCC->APB2PRSTR |= B3; cb(); }
INLINE void RCC_PBResetOff(void) { RCC->APB2PRSTR &= ~B3; cb(); }
INLINE void RCC_PBReset(void) { RCC_PBResetOn(); nop(); RCC_PBResetOff(); }

// PC port module reset ON/OFF/Reset
INLINE void RCC_PCResetOn(void) { RCC->APB2PRSTR |= B4; cb(); }
INLINE void RCC_PCResetOff(void) { RCC->APB2PRSTR &= ~B4; cb(); }
INLINE void RCC_PCReset(void) { RCC_PCResetOn(); nop(); RCC_PCResetOff(); }

// Port module reset (use port index PORT*_INX)
INLINE void RCC_PxResetOn(int portinx) { RCC->APB2PRSTR |= BIT(portinx+2); cb(); }
INLINE void RCC_PxResetOff(int portinx) { RCC->APB2PRSTR &= ~BIT(portinx+2); cb(); }
INLINE void RCC_PxReset(int portinx) { RCC_PxResetOn(portinx); nop(); RCC_PxResetOff(portinx); }

// ADC1 module reset ON/OFF/Reset
INLINE void RCC_ADC1ResetOn(void) { RCC->APB2PRSTR |= B9; cb(); }
INLINE void RCC_ADC1ResetOff(void) { RCC->APB2PRSTR &= ~B9; cb(); }
INLINE void RCC_ADC1Reset(void) { RCC_ADC1ResetOn(); nop(); RCC_ADC1ResetOff(); }

// TIM1 module reset ON/OFF/Reset
INLINE void RCC_TIM1ResetOn(void) { RCC->APB2PRSTR |= B11; cb(); }
INLINE void RCC_TIM1ResetOff(void) { RCC->APB2PRSTR &= ~B11; cb(); }
INLINE void RCC_TIM1Reset(void) { RCC_TIM1ResetOn(); nop(); RCC_TIM1ResetOff(); }

// SPI1 module reset ON/OFF/Reset
INLINE void RCC_SPI1ResetOn(void) { RCC->APB2PRSTR |= B12; cb(); }
INLINE void RCC_SPI1ResetOff(void) { RCC->APB2PRSTR &= ~B12; cb(); }
INLINE void RCC_SPI1Reset(void) { RCC_SPI1ResetOn(); nop(); RCC_SPI1ResetOff(); }

// USART1 module reset ON/OFF/Reset
INLINE void RCC_USART1ResetOn(void) { RCC->APB2PRSTR |= B14; cb(); }
INLINE void RCC_USART1ResetOff(void) { RCC->APB2PRSTR &= ~B14; cb(); }
INLINE void RCC_USART1Reset(void) { RCC_USART1ResetOn(); nop(); RCC_USART1ResetOff(); }

// TIM2 module reset ON/OFF/Reset
INLINE void RCC_TIM2ResetOn(void) { RCC->APB1PRSTR |= B0; cb(); }
INLINE void RCC_TIM2ResetOff(void) { RCC->APB1PRSTR &= ~B0; cb(); }
INLINE void RCC_TIM2Reset(void) { RCC_TIM2ResetOn(); nop(); RCC_TIM2ResetOff(); }

// TIM3 module reset ON/OFF/Reset
INLINE void RCC_TIM3ResetOn(void) { RCC->APB1PRSTR |= B1; cb(); }
INLINE void RCC_TIM3ResetOff(void) { RCC->APB1PRSTR &= ~B1; cb(); }
INLINE void RCC_TIM3Reset(void) { RCC_TIM3ResetOn(); nop(); RCC_TIM3ResetOff(); }

// Window watchdog reset ON/OFF/Reset
INLINE void RCC_WWDGResetOn(void) { RCC->APB1PRSTR |= B11; cb(); }
INLINE void RCC_WWDGResetOff(void) { RCC->APB1PRSTR &= ~B11; cb(); }
INLINE void RCC_WWDGReset(void) { RCC_WWDGResetOn(); nop(); RCC_WWDGResetOff(); }

// USART2 module reset ON/OFF/Reset
INLINE void RCC_USART2ResetOn(void) { RCC->APB1PRSTR |= B17; cb(); }
INLINE void RCC_USART2ResetOff(void) { RCC->APB1PRSTR &= ~B17; cb(); }
INLINE void RCC_USART2Reset(void) { RCC_USART2ResetOn(); nop(); RCC_USART2ResetOff(); }

// USART3 module reset ON/OFF/Reset
INLINE void RCC_USART3ResetOn(void) { RCC->APB1PRSTR |= B18; cb(); }
INLINE void RCC_USART3ResetOff(void) { RCC->APB1PRSTR &= ~B18; cb(); }
INLINE void RCC_USART3Reset(void) { RCC_USART3ResetOn(); nop(); RCC_USART3ResetOff(); }

// USART4 module reset ON/OFF/Reset
INLINE void RCC_USART4ResetOn(void) { RCC->APB1PRSTR |= B19; cb(); }
INLINE void RCC_USART4ResetOff(void) { RCC->APB1PRSTR &= ~B19; cb(); }
INLINE void RCC_USART4Reset(void) { RCC_USART4ResetOn(); nop(); RCC_USART4ResetOff(); }

// I2C1 module reset ON/OFF/Reset
INLINE void RCC_I2C1ResetOn(void) { RCC->APB1PRSTR |= B21; cb(); }
INLINE void RCC_I2C1ResetOff(void) { RCC->APB1PRSTR &= ~B21; cb(); }
INLINE void RCC_I2C1Reset(void) { RCC_I2C1ResetOn(); nop(); RCC_I2C1ResetOff(); }

// Power module reset ON/OFF/Reset
INLINE void RCC_PWRResetOn(void) { RCC->APB1PRSTR |= B28; cb(); }
INLINE void RCC_PWRResetOff(void) { RCC->APB1PRSTR &= ~B28; cb(); }
INLINE void RCC_PWRReset(void) { RCC_PWRResetOn(); nop(); RCC_PWRResetOff(); }

// USBFS module reset ON/OFF/Reset
INLINE void RCC_USBFSResetOn(void) { RCC->AHBRSTR |= B12; cb(); }
INLINE void RCC_USBFSResetOff(void) { RCC->AHBRSTR &= ~B12; cb(); }
INLINE void RCC_USBFSReset(void) { RCC_USBFSResetOn(); nop(); RCC_USBFSResetOff(); }

// PIOC module reset ON/OFF/Reset
INLINE void RCC_PIOCResetOn(void) { RCC->AHBRSTR |= B13; cb(); }
INLINE void RCC_PIOCResetOff(void) { RCC->AHBRSTR &= ~B13; cb(); }
INLINE void RCC_PIOCReset(void) { RCC_PIOCResetOn(); nop(); RCC_PIOCResetOff(); }

// USBPD reset ON/OFF/Reset
INLINE void RCC_USBPDResetOn(void) { RCC->AHBRSTR |= B17; cb(); }
INLINE void RCC_USBPDResetOff(void) { RCC->AHBRSTR &= ~B17; cb(); }
INLINE void RCC_USBPDReset(void) { RCC_USBPDResetOn(); nop(); RCC_USBPDResetOff(); }

// --- Peripheral clock

// DMA1 module clock Enable/Disable
INLINE void RCC_DMA1ClkEnable(void) { RCC->AHBPCENR |= B0; cb(); }
INLINE void RCC_DMA1ClkDisable(void) { RCC->AHBPCENR &= ~B0; cb(); }

// SRAM module clock Enable/Disable (enabled by default)
INLINE void RCC_SRAMClkEnable(void) { RCC->AHBPCENR |= B2; cb(); }
INLINE void RCC_SRAMClkDisable(void) { RCC->AHBPCENR &= ~B2; cb(); }

// USBFS module clock Enable/Disable (enabled by default)
INLINE void RCC_USBFSClkEnable(void) { RCC->AHBPCENR |= B12; cb(); }
INLINE void RCC_USBFSClkDisable(void) { RCC->AHBPCENR &= ~B12; cb(); }

// USBPD clock Enable/Disable (enabled by default)
INLINE void RCC_USBPDClkEnable(void) { RCC->AHBPCENR |= B17; cb(); }
INLINE void RCC_USBPDClkDisable(void) { RCC->AHBPCENR &= ~B17; cb(); }

// I/O auxiliary function module clock Enable/Disable
INLINE void RCC_AFIClkEnable(void) { RCC->APB2PCENR |= B0; cb(); }
INLINE void RCC_AFIClkDisable(void) { RCC->APB2PCENR &= ~B0; cb(); }

// Px port (0=PA, 1=PB, ...) module clock Enable/Disable
INLINE void RCC_PxClkEnable(int portinx) { RCC->APB2PCENR |= (1<<(portinx+2)); cb(); }
INLINE void RCC_PxClkDisable(int portinx) { RCC->APB2PCENR &= ~(1<<(portinx+2)); cb(); }

// PA port module clock Enable/Disable
INLINE void RCC_PAClkEnable(void) { RCC->APB2PCENR |= B2; cb(); }
INLINE void RCC_PAClkDisable(void) { RCC->APB2PCENR &= ~B2; cb(); }

// PB port module clock Enable/Disable
INLINE void RCC_PBClkEnable(void) { RCC->APB2PCENR |= B3; cb(); }
INLINE void RCC_PBClkDisable(void) { RCC->APB2PCENR &= ~B3; cb(); }

// PC port module clock Enable/Disable
INLINE void RCC_PCClkEnable(void) { RCC->APB2PCENR |= B4; cb(); }
INLINE void RCC_PCClkDisable(void) { RCC->APB2PCENR &= ~B4; cb(); }

// ADC1 module clock Enable/Disable
INLINE void RCC_ADC1ClkEnable(void) { RCC->APB2PCENR |= B9; cb(); }
INLINE void RCC_ADC1ClkDisable(void) { RCC->APB2PCENR &= ~B9; cb(); }

// TIM1 module clock Enable/Disable
INLINE void RCC_TIM1ClkEnable(void) { RCC->APB2PCENR |= B11; cb(); }
INLINE void RCC_TIM1ClkDisable(void) { RCC->APB2PCENR &= ~B11; cb(); }

// SPI1 module clock Enable/Disable
INLINE void RCC_SPI1ClkEnable(void) { RCC->APB2PCENR |= B12; cb(); }
INLINE void RCC_SPI1ClkDisable(void) { RCC->APB2PCENR &= ~B12; cb(); }

// USART1 module clock Enable/Disable
INLINE void RCC_USART1ClkEnable(void) { RCC->APB2PCENR |= B14; cb(); }
INLINE void RCC_USART1ClkDisable(void) { RCC->APB2PCENR &= ~B14; cb(); }

// TIM2 module clock Enable/Disable
INLINE void RCC_TIM2ClkEnable(void) { RCC->APB1PCENR |= B0; cb(); }
INLINE void RCC_TIM2ClkDisable(void) { RCC->APB1PCENR &= ~B0; cb(); }

// TIM3 module clock Enable/Disable
INLINE void RCC_TIM3ClkEnable(void) { RCC->APB1PCENR |= B1; cb(); }
INLINE void RCC_TIM3ClkDisable(void) { RCC->APB1PCENR &= ~B1; cb(); }

// Window watchdog clock Enable/Disable
INLINE void RCC_WWDGClkEnable(void) { RCC->APB1PCENR |= B11; cb(); }
INLINE void RCC_WWDGClkDisable(void) { RCC->APB1PCENR &= ~B11; cb(); }

// USART2 module clock Enable/Disable
INLINE void RCC_USART2ClkEnable(void) { RCC->APB1PCENR |= B17; cb(); }
INLINE void RCC_USART2ClkDisable(void) { RCC->APB1PCENR &= ~B17; cb(); }

// USART3 module clock Enable/Disable
INLINE void RCC_USART3ClkEnable(void) { RCC->APB1PCENR |= B18; cb(); }
INLINE void RCC_USART3ClkDisable(void) { RCC->APB1PCENR &= ~B18; cb(); }

// USART4 module clock Enable/Disable
INLINE void RCC_USART4ClkEnable(void) { RCC->APB1PCENR |= B19; cb(); }
INLINE void RCC_USART4ClkDisable(void) { RCC->APB1PCENR &= ~B19; cb(); }

// I2C1 module clock Enable/Disable
INLINE void RCC_I2C1ClkEnable(void) { RCC->APB1PCENR |= B21; cb(); }
INLINE void RCC_I2C1ClkDisable(void) { RCC->APB1PCENR &= ~B21; cb(); }

// Power module clock Enable/Disable
INLINE void RCC_PWRClkEnable(void) { RCC->APB1PCENR |= B28; cb(); }
INLINE void RCC_PWRClkDisable(void) { RCC->APB1PCENR &= ~B28; cb(); }

// Multiply AHB peripheral clock Enable/Disable (use combination of RCC_AHBCLK_* flags)
INLINE void RCC_AHBClkEnable(u32 mask) { RCC->AHBPCENR |= mask; cb(); }
INLINE void RCC_AHBClkDisable(u32 mask) { RCC->AHBPCENR &= ~mask; cb(); }

// Multiply APB2 peripheral clock Enable/Disable (use combination of RCC_APB2CLK_* flags)
INLINE void RCC_APB2ClkEnable(u32 mask) { RCC->APB2PCENR |= mask; cb(); }
INLINE void RCC_APB2ClkDisable(u32 mask) { RCC->APB2PCENR &= ~mask; cb(); }

// Multiply APB1 peripheral clock Enable/Disable (use combination of RCC_APB1CLK_* flags)
INLINE void RCC_APB1ClkEnable(u32 mask) { RCC->APB1PCENR |= mask; cb(); }
INLINE void RCC_APB1ClkDisable(u32 mask) { RCC->APB1PCENR &= ~mask; cb(); }

// --- Reset

// Check occurence of NRST pin reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetPinRes(void) { return (RCC->RSTSCKR & B26) != 0; }

// Check occurence of power reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetPwrRes(void) { return (RCC->RSTSCKR & B27) != 0; }

// Check occurence of software reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetSwRes(void) { return (RCC->RSTSCKR & B28) != 0; }

// Check occurence of independent watchdog reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetIwdgRes(void) { return (RCC->RSTSCKR & B29) != 0; }

// Check occurence of window watchdog reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetWwdgRes(void) { return (RCC->RSTSCKR & B30) != 0; }

// Check occurence of low-power reset flag (set by hardware, cleared by software)
INLINE Bool RCC_GetLowPwrRes(void) { return (RCC->RSTSCKR & B31) != 0; }

// Clear all reset flags
INLINE void RCC_ClrRes(void) { RCC->RSTSCKR |= B24; }

// OPA reset flag ON/OFF
INLINE void RCC_OPAResetOn(void) { RCC->RSTSCKR |= B25; cb(); }
INLINE void RCC_OPAResetOff(void) { RCC->RSTSCKR &= ~B25; cb(); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_RCC_H
