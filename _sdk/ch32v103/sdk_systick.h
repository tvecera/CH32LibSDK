
// ****************************************************************************
//
//                           SysTick System Timer
//
// ****************************************************************************

#ifndef _SDK_SYSTICK_H
#define _SDK_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

// number of HCLK clock cycles/8 per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef HCLK_PER_US
#define HCLK_PER_US	9		// H32V103 supports only HCLK/8
#endif

// number of HCLK clock cycles/8 per 1 ms
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef HCLK_PER_MS
#define HCLK_PER_MS	(HCLK_PER_US*1000)
#endif

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#ifndef SYSTICK_MS
#define SYSTICK_MS	5
#endif

// number of HCLK clock cycles/8 per SysTick interrupt
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef SYSTICK_HCLK
#define SYSTICK_HCLK	(SYSTICK_MS*HCLK_PER_MS)
#endif

// SysTick System counter
typedef struct {
	io32	CTLR;			// 0x00: system count control register
	union {
		io8	CNT8[8];	// 0x04: (Read/Write) system counter register
		struct {
			io32	CNT;	// 0x04: (Read Only) system counter register LOW
			io32	CNTH;	// 0x08: (Read Only) system counter register HIGH
		};
	};
	union {
		io8	CMP8[8];	// 0x0C: (Read/Write) counting comparison register
		struct {
			io32	CMP;	// 0x0C: (Read Only) counting comparison register LOW
			io32	CMPH;	// 0x10: (Read Only) counting comparison register HIGH
		};
	};
} SysTick_t;
STATIC_ASSERT(sizeof(SysTick_t) == 0x14, "Incorrect SysTick_t!");
#define SysTick		((SysTick_t*)SYSTICK_BASE)

// system time counter, counts time from system start in [ms]
// - overflow after 49 days (use difference, not absolute value!)
extern volatile u32 SysTime;

// current date and time
extern volatile u32 UnixTime;	// current date and time in Unix format
				// - number of seconds since 1/1/1970 (thursday) up to 12/31/2099
extern volatile u16 CurTimeMs;	// current time in [ms] 0..999

// Enable/Disable System counter
INLINE void SysTick_Enable(void) { SysTick->CTLR |= B0; }
INLINE void SysTick_Disable(void) { SysTick->CTLR &= ~B0; }

// Set SysTick System counter (counter should be stopped)
void SysTick_Set(u64 cnt);

// Get SysTick System counter LOW
INLINE u32 SysTick_Get(void) { return SysTick->CNT; }

// Get SysTick System counter 64-bit (slow, but interrupt safe)
u64 SysTick_Get64(void);

// Set SysTick comparison value
void SysTick_SetCmp(u64 cmp);

// Get SysTick comparison value
INLINE u64 SysTick_GetCmp64(void) { return SysTick->CMP | ((u64)SysTick->CMPH << 32); }

// Initialize SysTick
void SysTick_Init(void);

// Reset SysTick
void SysTick_Reset(void);

// Get current time in number of CPU clock cycles (= 48 MHz, overflow after 89 seconds)
// - Use HCLK_PER_US to get number of ticks per 1 us.
INLINE u32 Time(void) { return SysTick_Get(); }
INLINE u64 Time64(void) { return SysTick_Get64(); }

// Wait for delay in CPU clock cycles (max. 89 seconds)
void WaitClk(u32 clk);

// Wait for delay in [us] (max. 89 seconds)
void WaitUs(u32 us);

// Wait for delay in [ms] (max. 89 seconds)
void WaitMs(u32 ms);

#ifdef __cplusplus
}
#endif

#endif // _SDK_SYSTICK_H
