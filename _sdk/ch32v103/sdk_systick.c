
// ****************************************************************************
//
//                           SysTick System Timer
//
// ****************************************************************************

#include "../_include.h"

// system time counter, counts time from system start in [ms]
// - overflow after 49 days (use difference, not absolute value!)
volatile u32 SysTime = 0;

// current date and time
volatile u32 UnixTime = 0;	// current date and time in Unix format
				// - number of seconds since 1/1/1970 (thursday) up to 12/31/2099
volatile u16 CurTimeMs = 0;	// current time in [ms] 0..999

u64 SysTick_OldCnt;		// old SysTick counter

#if SYSTICK_MS > 0 // 0=do not use SysTick interrupt
// SysTick handler
HANDLER void SysTick_Handler()
{
	// get new counter
	u64 cnt = SysTick_Get64();

	// load current time
	u32 sys = SysTime;
	u16 ms = CurTimeMs;
	u64 old = SysTick_OldCnt;

	// increase time
	int dif = (int)(cnt - old);
	while (dif >= SYSTICK_HCLK)
	{
		dif -= SYSTICK_HCLK;
		old += SYSTICK_HCLK;
		sys += SYSTICK_MS;
		ms += SYSTICK_MS;
		if (ms >= 1000)
		{
			UnixTime++;
			ms -= 1000;
		}
	}

	// save new current time
	SysTime = sys;
	CurTimeMs = ms;
	SysTick_OldCnt = old;

	// set new interrupt time (skip one interrupt, if remains < 100us)
	cnt += SYSTICK_HCLK - dif;
	if (dif >= SYSTICK_HCLK - 100*HCLK_PER_US) cnt += SYSTICK_HCLK;
	SysTick_SetCmp(cnt);
}
#endif // SYSTICK_MS > 0

// Initialize SysTick
void SysTick_Init(void)
{
	SysTick_Disable();	// disable counter
	SysTick_Set(0);		// clear counter
	SysTick_SetCmp(SYSTICK_HCLK-1); // set next compare value
	SysTick_OldCnt = 0;	// update current counter
	SysTick_Enable();	// enable counter
#if SYSTICK_MS > 0 // 0=do not use SysTick interrupt
	NVIC_IRQEnable(IRQ_SYSTICK); // IRQ enable
#endif
}

// Reset SysTick
void SysTick_Reset(void)
{
	NVIC_IRQDisable(IRQ_SYSTICK); // IRQ disable
	SysTick_Disable();	// disable counter
	SysTick_Set(0);		// clear counter
	SysTick_SetCmp(0);	// set next compare value
	SysTick->CTLR = 0;	// reset control register
}

// Set SysTick System counter (counter should be stopped)
void SysTick_Set(u64 cnt)
{
	SysTick->CNT8[0] = (u8)cnt;
	SysTick->CNT8[1] = (u8)(cnt >> 1*8);
	SysTick->CNT8[2] = (u8)(cnt >> 2*8);
	SysTick->CNT8[3] = (u8)(cnt >> 3*8);
	SysTick->CNT8[4] = (u8)(cnt >> 4*8);
	SysTick->CNT8[5] = (u8)(cnt >> 5*8);
	SysTick->CNT8[6] = (u8)(cnt >> 6*8);
	SysTick->CNT8[7] = (u8)(cnt >> 7*8);
}

// Get SysTick System counter 64-bit (interrupt safe)
u64 SysTick_Get64(void)
{
	u32 low, high;
	u32 high2 = SysTick->CNTH;  // get raw time HIGH

	do {
		// accept new time HIGH
		high = high2;

		// get raw time LOW
		cb(); // compiler barrier (or else the compiler could swap the order of operations)
		low = SysTick->CNT;

		// get raw time HIGH again
		cb(); // compiler barrier (or else the compiler could swap the order of operations)
		high2 = SysTick->CNTH;

	// check that HIGH has not changed, otherwise a re-read will be necessary
	} while (high != high2);

	// return result value
	return ((u64)high << 32) | low;
}

// Set SysTick comparison value
void SysTick_SetCmp(u64 cmp)
{
	SysTick->CMP8[0] = (u8)cmp;
	SysTick->CMP8[1] = (u8)(cmp >> 1*8);
	SysTick->CMP8[2] = (u8)(cmp >> 2*8);
	SysTick->CMP8[3] = (u8)(cmp >> 3*8);
	SysTick->CMP8[4] = (u8)(cmp >> 4*8);
	SysTick->CMP8[5] = (u8)(cmp >> 5*8);
	SysTick->CMP8[6] = (u8)(cmp >> 6*8);
	SysTick->CMP8[7] = (u8)(cmp >> 7*8);
}

// Wait for delay in CPU clock cycles (max. 89 seconds)
void WaitClk(u32 clk)
{
	// get start time
	u32 start = Time();

	// wait
	while ((u32)(Time() - start) < clk) {}
}

// Wait for delay in [us] (max. 89 seconds)
void WaitUs(u32 us)
{
	// recalculate us to ticks
	us *= HCLK_PER_US;

	// get start time
	u32 start = Time();

	// wait
	while ((u32)(Time() - start) < us) {}
}

// Wait for delay in [ms] (max. 89 seconds)
void WaitMs(u32 ms)
{
	// recalculate ms to ticks
	ms *= HCLK_PER_MS;

	// get start time
	u32 start = Time();

	// wait
	while ((u32)(Time() - start) < ms) {}
}
