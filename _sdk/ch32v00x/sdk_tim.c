
// ****************************************************************************
//
//                                Timer
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_TIM		// 1=use timers

// Reset timer to default setup
void TIMx_Reset(TIM_t* tim)
{
	if (tim == TIM1) RCC_TIM1Reset();
	if (tim == TIM2) RCC_TIM2Reset();
}

// Enable timer clock source
void TIMx_ClkEnable(TIM_t* tim)
{
	if (tim == TIM1) RCC_TIM1ClkEnable();
	if (tim == TIM2) RCC_TIM2ClkEnable();
}

// Initialize simple timer with interrupt
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
// You also need to do:
// - Enable IRQ interrupt with NVIC_IRQEnable(IRQ_TIMx_UP).
// - Create interrupt service "HANDLER void TIMx_UP_IRQHandler()".
// - In interrupt service clear interrupt flag TIMx_UpIntClr().
void TIMx_InitInt(TIM_t* tim, int div, int reload)
{
	// Enable timer clock source
	TIMx_ClkEnable(tim);

	// Reset timer to default setup
	TIMx_Reset(tim);

	// select input from internal clock CK_INT
	TIMx_InMode(tim, TIM_INMODE_INT);

	// set divider
	TIMx_Div(tim, div);

	// set reload value
	TIMx_Load(tim, reload);

	// reload immediately
	TIMx_Update(tim);

	// clear update interrupt flag
	TIMx_UpIntClr(tim);

	// enable update interrupt
	TIMx_UpIntEnable(tim);

	// enable timer
	TIMx_Enable(tim);
}

// Initialize simple PWM output from channel
//   chan ... channel 1..4
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
//   comp ... compare value 0..reload
//   high ... direction HIGH->LOW (or LOW->HIGH otherwise)
// You also need to do:
// - See GPIO_Remap_TIMx() function to which pin the channel is mapped.
// - Set pin function to GPIO_Mode(pin, GPIO_MODE_AF).
void TIMx_InitPWM(TIM_t* tim, int chan, int div, int reload, int comp, Bool high)
{
	// Enable timer clock source
	TIMx_ClkEnable(tim);

	// Reset timer to default setup
	TIMx_Reset(tim);

	// select input from internal clock CK_INT
	TIMx_InMode(tim, TIM_INMODE_INT);

	// set divider
	TIMx_Div(tim, div);

	// set reload value
	TIMx_Load(tim, reload);

	// direction up
	TIMx_DirUp(tim);

	// enable auto-reload of preload compare register
	TIMx_AutoReloadEnable(tim);

	// prepare direction
	int dir = high ? TIM_COMP_PWM1 : TIM_COMP_PWM2;

	switch (chan)
	{
	// channel 1
	case 1:
		TIMx_Comp1(tim, comp);		// set compare value
		TIMx_OC1Mode(tim, dir);		// set compare mode
		TIMx_OC1PreEnable(tim);		// enable preload compare register
		TIMx_CC1Enable(tim);		// enable compare output
		break;

	// channel 2
	case 2:
		TIMx_Comp2(tim, comp);		// set compare value
		TIMx_OC2Mode(tim, dir);		// set compare mode
		TIMx_OC2PreEnable(tim);		// enable preload compare register
		TIMx_CC2Enable(tim);		// enable compare output
		break;

	// channel 3
	case 3:
		TIMx_Comp3(tim, comp);		// set compare value
		TIMx_OC3Mode(tim, dir);		// set compare mode
		TIMx_OC3PreEnable(tim);		// enable preload compare register
		TIMx_CC3Enable(tim);		// enable compare output
		break;

	// channel 4
	case 4:
		TIMx_Comp4(tim, comp);		// set compare value
		TIMx_OC4Mode(tim, dir);		// set compare mode
		TIMx_OC4PreEnable(tim);		// enable preload compare register
		TIMx_CC4Enable(tim);		// enable compare output
		break;
	}

	// enable main output
	TIMx_OCEnable(tim);

	// reload immediately
	TIMx_Update(tim);

	// enable timer
	TIMx_Enable(tim);
}

#endif // USE_TIM
