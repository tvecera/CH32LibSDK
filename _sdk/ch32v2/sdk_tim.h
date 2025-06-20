
// ****************************************************************************
//
//                                Timer
//
// ****************************************************************************
// Timer 5 has 32-bit counter on some models - use TIM5_Get* functions instead of TIMx_Get*.

/*
Timer description
-----------------
Input clock CK_PSC, select source:
	- SMCFGR.ECE is 0, TIM2_EC2Disable(), do not used external clock mode 2, select source by TIMx_InMode() TIM_INMODE_*:
		- TIM_INMODE_INT: internal clock CK_INT, source is HB bus clock
		- TIM_INMODE_ENC1..3: encoder mode 1..3, count edges of TI1FP1 and TI2FP2
		- TIM_INMODE_RESET: reset mode, trigger input TRGI will inicialize counter and generate update
			clock source is TRGI, select trigger by TIMx_Trig() TIM_TRIG_*, see below
		- TIM_INMODE_GATE: gated mode, counter clock enabled by level od TRGI input
			clock source is TRGI, select trigger by TIMx_Trig() TIM_TRIG_*, see below
		- TIM_INMODE_TRIG: trigger mode, counter is started on rising edge of TRGI
			clock source is TRGI, select trigger by TIMx_Trig() TIM_TRIG_*, see below
		- TIM_INMODE_EDGE: rising edge of selected trigger input TRGI drives counter
			clock source is TRGI, select trigger by TIMx_Trig() TIM_TRIG_*
			- TIM_TRIG_ITR0..3 internal trigger ITR0..3
			- TIM_TRIG_TI1FED edge detector of TI1, comparison of the signal after capturing channel 1
			- TIM_TRIG_TI1FP1..2 filtered timer input TI1FP1..2, comparison of the capture channel
			- TIM_TRIG_ETRF external trigger input ETRF, input from external clock input

	- SMCFGR.ECE is 1, TIM2_EC2Enable(), used external clock mode 2, external input pin ETR is used as clock source.

	Complex clock sources can also be output as TRGO to other peripherals

Core counter:
	- CK_PSC input clock divided by 16-bit prescaler PSC TIMx_Presc() to CK_CNT counter clock. Dividing factor is PSC+1.
	- count clock CK_CNT by 16-bit core counter CNT TIMx_Cnt().
	- automatic reload register TIMx_Load() reloads initial value of the counter at the end of each count cycle.
	- auxiliary counter TIMx_Repeat() counts number of times the AutoReload reloads initial value for CNT

Four compare capture channels CH1..CH4:
	- input from exclusive pins TIMx_CHx
	- output to the pins TIMx_CHx and TIMx_CHxN (complementary)
	- input from each channel supports filtering, dividing edge detection and mutual triggering between channels
	- each channel has compare capture register CHxCVR to compare with main counter CNT to output pulses

	Input from external TIx pin is filtered by down counter TIMx_Filter() TIM_FILTER_* and generates TIxF flag.
	Signal TIxF is processed by edge detector and generates signals TIxFP1, TIxFP2 and TIxFED signals.

	Input compare mode:
		Capture event occurs when a determined edge on the ICxPS signal is detected,
		and current value of counter is latched into compare capture register CHxCVR.
		CCxIF flag is set and interrupt or DMA occurs.
			1) Configure CCxS input mode TIMx_CC1Sel() TIM_CCSEL_*
			2) Select input filter TIMx_IC1Filter() TIM_FILTER_*
			3) Select polarity TIMx_IC1Fall()/TIMx_IC1Rise()
			4) Enable capture TIMx_CC1Enable()
			5) Enable interrupt TIMx_CC1IntEnable() or DMA TIMx_CC1DMAReqEnable()
			When captured pulse is input to TI1, the value of counter CNT is recorded
			in the compare capture register, CC1IF is set, interrupt or DMA is generated.

	Compare output mode:
		Output changes when counter agrees with compare register.
			1) Configure CCxS output mode TIMx_CC1Sel() TIM_CCSEL_*
			2) Setup auto-reload value TIMx_Load()
			3) Setup value to be compared TIMx_Comp1()
			4) Enable interrupt TIMx_CC1IntEnable()
			5) Disable preload compare register TIMx_OC1PreDisable()
			6) Setup compare mode TIMx_OC1Mode() TIM_COMP_*
			7) Setup output polarity TIMx_OC1Low() or TIMx_OC1High()
			8) Enable output TIMx_CC1Enable()
			9) Start timer TIMx_Enable()
	Force output mode:
		Set compare mode TIMx_OC1Mode() to TIM_COMP_INVALID (low) or TIM_COMP_VALID (high).

	PWM input mode:
		Used to measure duty cycle and frequency. PWM occupies two compare capture channels
		with opposite polarity.
			1) Select TI1 as IC1 input TIMx_CC1Sel() TIM_CCSEL_TI1
			2) Select TI1FP1 to rising edge TIMx_IC1Rise()
			3) Select TI1 as IC2 input TIMx_CC2Sel() TIM_CCSEL_TI1
			4) Select TI1FP1 to falling edge TIMx_IC2Fall()
			5) Select TI1FP1 as clock source TIMx_Trig() TIM_TRIG_TI1FP1
			6) Set reset mode TIMx_InMode() TIM_INMODE_RESET
			7) enable input capture TIMx_CC1Enable() and TIMx_CC2Enable()

	PWM output mode:
			1) Set TIMx_OC1Mode() to TIM_COMP_PWM1 or TIM_COMP_PWM2.
			2) Enable preload register TIMx_OC1PreEnable()
			3) Enable auto-reload TIMx_AutoReloadEnable() (defines timer period)
			4) Generate first update with TIMx_Update()

			In PWM mode, core counter and compare register are always comparing,
			outputing edge-aligned or center-aligned PWM signals.

			Edge alignment: counter is incremented or decremented, OCxREF changes by compare register
			Central alignment: counter is incremented and decremented
*/

#if USE_TIM		// 1=use timers

#ifndef _SDK_TIM_H
#define _SDK_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

// Timer registers
typedef struct {
	io32	CTLR1;		// 0x00: control register 1
	io32	CTLR2;		// 0x04: control register 2
	io32	SMCFGR;		// 0x08: slave mode control register
	io32	DMAINTENR;	// 0x0C: DMA/interrupt enable register
	io32	INTFR;		// 0x10: interrupt status register
	io32	SWEVGR;		// 0x14: even generation register
	io32	CHCTLR1;	// 0x18: compare/capture control register 1
	io32	CHCTLR2;	// 0x1C: compare/capture control register 2
	io32	CCER;		// 0x20: compare/capture enable register
	io32	CNT;		// 0x24: counter
	io32	PSC;		// 0x28: prescaler
	io32	ATRLR;		// 0x2C: auto-reload value register
	io32	RPTCR;		// 0x30: recurring count value register
	io32	CH1CVR;		// 0x34: compare/capture register 1
	io32	CH2CVR;		// 0x38: compare/capture register 2
	io32	CH3CVR;		// 0x3C: compare/capture register 3
	io32	CH4CVR;		// 0x40: compare/capture register 4
	io32	BDTR;		// 0x44: brake and deadband register
	io32	DMACFGR;	// 0x48: DMA control register
	io32	DMAADR;		// 0x4C: DMA address register
	io32	AUX;		// 0x50: dual-edge capture register
} TIM_t;
STATIC_ASSERT(sizeof(TIM_t) == 0x54, "Incorrect TIM_t!");
#define TIM1	((TIM_t*)TIM1_BASE)	// 0x40012C00 advanced-control timer ADTM
#define TIM2	((TIM_t*)TIM2_BASE)	// 0x40000000 general-purpose timer GPTM
#define TIM3	((TIM_t*)TIM3_BASE)	// 0x40000400 general-purpose timer GPTM
#define TIM4	((TIM_t*)TIM4_BASE)	// 0x40000800 general-purpose timer GPTM
#define TIM5	((TIM_t*)TIM5_BASE)	// 0x40000C00 general-purpose timer GPTM (32-bit on some models)
#define TIM6	((TIM_t*)TIM6_BASE)	// 0x40001000 basic timer BCTM
#define TIM7	((TIM_t*)TIM7_BASE)	// 0x40001400 basic timer BCTM
#define TIM8	((TIM_t*)TIM8_BASE)	// 0x40013400 advanced-control timer ADTM
#define TIM9	((TIM_t*)TIM9_BASE)	// 0x40014C00 advanced-control timer ADTM
#define TIM10	((TIM_t*)TIM10_BASE)	// 0x40015000 advanced-control timer ADTM

// central alignment mode
#define TIM_ALIGN_EDGE		0	// edge alignment, counter counts up or down
#define TIM_ALIGN_DOWN		1	// central alignment 1, counts up and down, compare flag is set on down direction
#define TIM_ALIGN_UP		2	// central alignment 2, counts up and down, compare flag is set on up direction
#define TIM_ALIGN_BOTH		3	// central alignment 2, counts up and down, compare flag is set on both directions

// clock division ratio of dead time generator
#define TIM_CKD_DIV1		0	// no division
#define TIM_CKD_DIV2		1	// 2x division
#define TIM_CKD_DIV4		2	// 4x division

// master to slave synchronization mode (select trigger output signal)
#define TIM_MSSYNC_RESET	0	// UG bit of Reset-TIM_EGR register
#define TIM_MSSYNC_ENABLE	1	// counter enable signal
#define TIM_MSSYNC_UPDATE	2	// update event
#define TIM_MSSYNC_COMP		3	// comparison or capture
#define TIM_MSSYNC_OC1		4	// comparison-OC1REF
#define TIM_MSSYNC_OC2		5	// comparison-OC2REF
#define TIM_MSSYNC_OC3		6	// comparison-OC3REF
#define TIM_MSSYNC_OC4		7	// comparison-OC4REF

// input mode
#define TIM_INMODE_INT		0	// driven by internal clock CK_INT
#define TIM_INMODE_ENC1		1	// encoder mode 1, count edges of TI2FP2 depending on TI1FP1
#define TIM_INMODE_ENC2		2	// encoder mode 2, count edges of TI1FP1 depending on TI2FP2
#define TIM_INMODE_ENC3		3	// encoder mode 3, count edges of TI1FP1 and TI2FP2 depending on another signal
#define TIM_INMODE_RESET	4	// reset mode, trigger input TRGI will inicialize counter and generate update
#define TIM_INMODE_GATE		5	// gated mode, counter clock enabled by level od TRGI input
#define TIM_INMODE_TRIG		6	// trigger mode, counter is started on rising edge of TRGI
#define TIM_INMODE_EDGE		7	// rising edge of selected trigger input TRGI drives counter

// trigger selection
#define TIM_TRIG_ITR0		0	// internal trigger 0 ITR0
#define TIM_TRIG_ITR1		1	// internal trigger 1 ITR1
#define TIM_TRIG_ITR2		2	// internal trigger 2 ITR2
#define TIM_TRIG_ITR3		3	// internal trigger 3 ITR3
#define TIM_TRIG_TI1FED		4	// edge detector of TI1
#define TIM_TRIG_TI1FP1		5	// filtered timer input 1 TI1FP1
#define TIM_TRIG_TI2FP2		6	// filtered timer input 2 TI2FP2
#define TIM_TRIG_ETRF		7	// external trigger input ERTF

// external filtering (record up to N events)
#define TIM_FILTER_0		0	// no filter, sampled at Fdts
#define TIM_FILTER_1_2		1	// frequency CK_INT, N=2
#define TIM_FILTER_1_4		2	// frequency CK_INT, N=4
#define TIM_FILTER_1_8		3	// frequency CK_INT, N=8
#define TIM_FILTER_2_6		4	// frequency Fdts/2, N=6
#define TIM_FILTER_2_8		5	// frequency Fdts/2, N=8
#define TIM_FILTER_4_6		6	// frequency Fdts/4, N=6
#define TIM_FILTER_4_8		7	// frequency Fdts/4, N=8
#define TIM_FILTER_8_6		8	// frequency Fdts/8, N=6
#define TIM_FILTER_8_8		9	// frequency Fdts/8, N=8
#define TIM_FILTER_16_5		10	// frequency Fdts/16, N=5
#define TIM_FILTER_16_6		11	// frequency Fdts/16, N=6
#define TIM_FILTER_16_8		12	// frequency Fdts/16, N=8
#define TIM_FILTER_32_5		13	// frequency Fdts/32, N=5
#define TIM_FILTER_32_6		14	// frequency Fdts/32, N=6
#define TIM_FILTER_32_8		15	// frequency Fdts/32, N=8

// prescaler of external trigger signal
#define TIM_ETRPDIV_1		0	// prescaler off
#define TIM_ETRPDIV_2		1	// ETRP frequency divided by 2
#define TIM_ETRPDIV_4		2	// ETRP frequency divided by 4
#define TIM_ETRPDIV_8		3	// ETRP frequency divided by 8

// compare mode (output)
#define TIM_COMP_FREEZE		0	// freeze OCxREF level
#define TIM_COMP_VALIDEQ	1	// force to set OCxREF to valid level high if equal
#define TIM_COMP_INVALIDEQ	2	// force to set OCxREF to invalid level low if equal
#define TIM_COMP_FLIP		3	// flip OCxREF level if match
#define TIM_COMP_INVALID	4	// force to set OCxREF to invalid level low
#define TIM_COMP_VALID		5	// force to set OCxREF to valid level high
#define TIM_COMP_PWM1		6	// PWM mode 1, counting up or down: state valid->invalid
#define TIM_COMP_PWM2		7	// PWM mode 2, counting up or down: state invalid->valid

// compare capture input selection
#define TIM_CCSEL_OUT		0	// channel is configured as output
#define TIM_CCSEL_TI1		1	// channel is configured as input TI1 (TI3)
#define TIM_CCSEL_TI2		2	// channel is configured as input TI2 (TI4)
#define TIM_CCSEL_TRC		3	// channel is configured as input TRC

// lock function
#define TIM_LOCK_OFF		0	// no lock
#define TIM_LOCK_1		1	// lock level 1, no DTG, BKE, BKP, AOE, OIS* bits can be writen
#define TIM_LOCK_2		2	// lock level 2, as level 1 plus CC polarity, OSSR and OSSI bits
#define TIM_LOCK_3		3	// lock level 3, as level 2 plus CC control bits

// === Control

// Enable/Disable counter (default disabled; register CTLR1.CEN)
INLINE void TIMx_Enable(TIM_t* tim) { tim->CTLR1 |= B0; }
INLINE void TIM1_Enable(void) { TIMx_Enable(TIM1); }
INLINE void TIM2_Enable(void) { TIMx_Enable(TIM2); }
INLINE void TIM3_Enable(void) { TIMx_Enable(TIM3); }
INLINE void TIM4_Enable(void) { TIMx_Enable(TIM4); }
INLINE void TIM5_Enable(void) { TIMx_Enable(TIM5); }
INLINE void TIM6_Enable(void) { TIMx_Enable(TIM6); }
INLINE void TIM7_Enable(void) { TIMx_Enable(TIM7); }
INLINE void TIM8_Enable(void) { TIMx_Enable(TIM8); }
INLINE void TIM9_Enable(void) { TIMx_Enable(TIM9); }
INLINE void TIM10_Enable(void){ TIMx_Enable(TIM10); }

INLINE void TIMx_Disable(TIM_t* tim) { tim->CTLR1 &= ~B0; }
INLINE void TIM1_Disable(void) { TIMx_Disable(TIM1); }
INLINE void TIM2_Disable(void) { TIMx_Disable(TIM2); }
INLINE void TIM3_Disable(void) { TIMx_Disable(TIM3); }
INLINE void TIM4_Disable(void) { TIMx_Disable(TIM4); }
INLINE void TIM5_Disable(void) { TIMx_Disable(TIM5); }
INLINE void TIM6_Disable(void) { TIMx_Disable(TIM6); }
INLINE void TIM7_Disable(void) { TIMx_Disable(TIM7); }
INLINE void TIM8_Disable(void) { TIMx_Disable(TIM8); }
INLINE void TIM9_Disable(void) { TIMx_Disable(TIM9); }
INLINE void TIM10_Disable(void) { TIMx_Disable(TIM10); }

// Enable/Disable generation of UEV update events (default enabled; register CTLR1.UDIS)
INLINE void TIMx_UEVEnable(TIM_t* tim) { tim->CTLR1 &= ~B1; }
INLINE void TIM1_UEVEnable(void) { TIMx_UEVEnable(TIM1); }
INLINE void TIM2_UEVEnable(void) { TIMx_UEVEnable(TIM2); }
INLINE void TIM3_UEVEnable(void) { TIMx_UEVEnable(TIM3); }
INLINE void TIM4_UEVEnable(void) { TIMx_UEVEnable(TIM4); }
INLINE void TIM5_UEVEnable(void) { TIMx_UEVEnable(TIM5); }
INLINE void TIM6_UEVEnable(void) { TIMx_UEVEnable(TIM6); }
INLINE void TIM7_UEVEnable(void) { TIMx_UEVEnable(TIM7); }
INLINE void TIM8_UEVEnable(void) { TIMx_UEVEnable(TIM8); }
INLINE void TIM9_UEVEnable(void) { TIMx_UEVEnable(TIM9); }
INLINE void TIM10_UEVEnable(void) { TIMx_UEVEnable(TIM10); }

INLINE void TIMx_UEVDisable(TIM_t* tim) { tim->CTLR1 |= B1; }
INLINE void TIM1_UEVDisable(void) { TIMx_UEVDisable(TIM1); }
INLINE void TIM2_UEVDisable(void) { TIMx_UEVDisable(TIM2); }
INLINE void TIM3_UEVDisable(void) { TIMx_UEVDisable(TIM3); }
INLINE void TIM4_UEVDisable(void) { TIMx_UEVDisable(TIM4); }
INLINE void TIM5_UEVDisable(void) { TIMx_UEVDisable(TIM5); }
INLINE void TIM6_UEVDisable(void) { TIMx_UEVDisable(TIM6); }
INLINE void TIM7_UEVDisable(void) { TIMx_UEVDisable(TIM7); }
INLINE void TIM8_UEVDisable(void) { TIMx_UEVDisable(TIM8); }
INLINE void TIM9_UEVDisable(void) { TIMx_UEVDisable(TIM9); }
INLINE void TIM10_UEVDisable(void) { TIMx_UEVDisable(TIM10); }

// Select UEV update source to all events or to overflow only (default all events; register CTLR1.URS)
INLINE void TIMx_UEVSrcAll(TIM_t* tim) { tim->CTLR1 &= ~B2; }
INLINE void TIM1_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM1); }
INLINE void TIM2_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM2); }
INLINE void TIM3_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM3); }
INLINE void TIM4_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM4); }
INLINE void TIM5_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM5); }
INLINE void TIM6_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM6); }
INLINE void TIM7_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM7); }
INLINE void TIM8_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM8); }
INLINE void TIM9_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM9); }
INLINE void TIM10_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM10); }

INLINE void TIMx_UEVSrcOver(TIM_t* tim) { tim->CTLR1 |= B2; }
INLINE void TIM1_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM1); }
INLINE void TIM2_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM2); }
INLINE void TIM3_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM3); }
INLINE void TIM4_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM4); }
INLINE void TIM5_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM5); }
INLINE void TIM6_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM6); }
INLINE void TIM7_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM7); }
INLINE void TIM8_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM8); }
INLINE void TIM9_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM9); }
INLINE void TIM10_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM10); }

// Select pulse mode (single run) or repeating mode (default repeating mode; register CTLR1.OPM)
INLINE void TIMx_PulseMode(TIM_t* tim) { tim->CTLR1 |= B3; }
INLINE void TIM1_PulseMode(void) { TIMx_PulseMode(TIM1); }
INLINE void TIM2_PulseMode(void) { TIMx_PulseMode(TIM2); }
INLINE void TIM3_PulseMode(void) { TIMx_PulseMode(TIM3); }
INLINE void TIM4_PulseMode(void) { TIMx_PulseMode(TIM4); }
INLINE void TIM5_PulseMode(void) { TIMx_PulseMode(TIM5); }
INLINE void TIM6_PulseMode(void) { TIMx_PulseMode(TIM6); }
INLINE void TIM7_PulseMode(void) { TIMx_PulseMode(TIM7); }
INLINE void TIM8_PulseMode(void) { TIMx_PulseMode(TIM8); }
INLINE void TIM9_PulseMode(void) { TIMx_PulseMode(TIM9); }
INLINE void TIM10_PulseMode(void) { TIMx_PulseMode(TIM10); }

INLINE void TIMx_RepeatMode(TIM_t* tim) { tim->CTLR1 &= ~B3; }
INLINE void TIM1_RepeatMode(void) { TIMx_RepeatMode(TIM1); }
INLINE void TIM2_RepeatMode(void) { TIMx_RepeatMode(TIM2); }
INLINE void TIM3_RepeatMode(void) { TIMx_RepeatMode(TIM3); }
INLINE void TIM4_RepeatMode(void) { TIMx_RepeatMode(TIM4); }
INLINE void TIM5_RepeatMode(void) { TIMx_RepeatMode(TIM5); }
INLINE void TIM6_RepeatMode(void) { TIMx_RepeatMode(TIM6); }
INLINE void TIM7_RepeatMode(void) { TIMx_RepeatMode(TIM7); }
INLINE void TIM8_RepeatMode(void) { TIMx_RepeatMode(TIM8); }
INLINE void TIM9_RepeatMode(void) { TIMx_RepeatMode(TIM9); }
INLINE void TIM10_RepeatMode(void) { TIMx_RepeatMode(TIM10); }

// Select counting direction up or down (default up direction; register CTLR1.DIR)
INLINE void TIMx_DirUp(TIM_t* tim) { tim->CTLR1 &= ~B4; }
INLINE void TIM1_DirUp(void) { TIMx_DirUp(TIM1); }
INLINE void TIM2_DirUp(void) { TIMx_DirUp(TIM2); }
INLINE void TIM3_DirUp(void) { TIMx_DirUp(TIM3); }
INLINE void TIM4_DirUp(void) { TIMx_DirUp(TIM4); }
INLINE void TIM5_DirUp(void) { TIMx_DirUp(TIM5); }
INLINE void TIM6_DirUp(void) { TIMx_DirUp(TIM6); }
INLINE void TIM7_DirUp(void) { TIMx_DirUp(TIM7); }
INLINE void TIM8_DirUp(void) { TIMx_DirUp(TIM8); }
INLINE void TIM9_DirUp(void) { TIMx_DirUp(TIM9); }
INLINE void TIM10_DirUp(void) { TIMx_DirUp(TIM10); }

INLINE void TIMx_DirDown(TIM_t* tim) { tim->CTLR1 |= B4; }
INLINE void TIM1_DirDown(void) { TIMx_DirDown(TIM1); }
INLINE void TIM2_DirDown(void) { TIMx_DirDown(TIM2); }
INLINE void TIM3_DirDown(void) { TIMx_DirDown(TIM3); }
INLINE void TIM4_DirDown(void) { TIMx_DirDown(TIM4); }
INLINE void TIM5_DirDown(void) { TIMx_DirDown(TIM5); }
INLINE void TIM6_DirDown(void) { TIMx_DirDown(TIM6); }
INLINE void TIM7_DirDown(void) { TIMx_DirDown(TIM7); }
INLINE void TIM8_DirDown(void) { TIMx_DirDown(TIM8); }
INLINE void TIM9_DirDown(void) { TIMx_DirDown(TIM9); }
INLINE void TIM10_DirDown(void) { TIMx_DirDown(TIM10); }

// Set alignment mode selection TIM_ALIGN_* (default edge alignment; register CTLR1.CMS[1:0])
INLINE void TIMx_Align(TIM_t* tim, int align) { tim->CTLR1 = (tim->CTLR1 & ~(3<<5)) | (align<<5); }
INLINE void TIM1_Align(int align) { TIMx_Align(TIM1, align); }
INLINE void TIM2_Align(int align) { TIMx_Align(TIM2, align); }
INLINE void TIM3_Align(int align) { TIMx_Align(TIM3, align); }
INLINE void TIM4_Align(int align) { TIMx_Align(TIM4, align); }
INLINE void TIM5_Align(int align) { TIMx_Align(TIM5, align); }
INLINE void TIM6_Align(int align) { TIMx_Align(TIM6, align); }
INLINE void TIM7_Align(int align) { TIMx_Align(TIM7, align); }
INLINE void TIM8_Align(int align) { TIMx_Align(TIM8, align); }
INLINE void TIM9_Align(int align) { TIMx_Align(TIM9, align); }
INLINE void TIM10_Align(int align) { TIMx_Align(TIM10, align); }

// Enable/Disable auto-reload (default disabled; register CTLR1.ARPE)
INLINE void TIMx_AutoReloadEnable(TIM_t* tim) { tim->CTLR1 |= B7; }
INLINE void TIM1_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM1); }
INLINE void TIM2_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM2); }
INLINE void TIM3_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM3); }
INLINE void TIM4_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM4); }
INLINE void TIM5_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM5); }
INLINE void TIM6_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM6); }
INLINE void TIM7_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM7); }
INLINE void TIM8_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM8); }
INLINE void TIM9_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM9); }
INLINE void TIM10_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM10); }

INLINE void TIMx_AutoReloadDisable(TIM_t* tim) { tim->CTLR1 &= ~B7; }
INLINE void TIM1_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM1); }
INLINE void TIM2_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM2); }
INLINE void TIM3_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM3); }
INLINE void TIM4_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM4); }
INLINE void TIM5_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM5); }
INLINE void TIM6_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM6); }
INLINE void TIM7_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM7); }
INLINE void TIM8_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM8); }
INLINE void TIM9_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM9); }
INLINE void TIM10_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM10); }

// Set clock division ratio of dead time generator TIM_CKD_* (default no division; register CTLR1.CKD[1:0])
INLINE void TIMx_CKDDiv(TIM_t* tim, int div) { tim->CTLR1 = (tim->CTLR1 & ~(3<<8)) | (div<<8); }
INLINE void TIM1_CKDDiv(int div) { TIMx_CKDDiv(TIM1, div); }
INLINE void TIM2_CKDDiv(int div) { TIMx_CKDDiv(TIM2, div); }
INLINE void TIM3_CKDDiv(int div) { TIMx_CKDDiv(TIM3, div); }
INLINE void TIM4_CKDDiv(int div) { TIMx_CKDDiv(TIM4, div); }
INLINE void TIM5_CKDDiv(int div) { TIMx_CKDDiv(TIM5, div); }
INLINE void TIM6_CKDDiv(int div) { TIMx_CKDDiv(TIM6, div); }
INLINE void TIM7_CKDDiv(int div) { TIMx_CKDDiv(TIM7, div); }
INLINE void TIM8_CKDDiv(int div) { TIMx_CKDDiv(TIM8, div); }
INLINE void TIM9_CKDDiv(int div) { TIMx_CKDDiv(TIM9, div); }
INLINE void TIM10_CKDDiv(int div) { TIMx_CKDDiv(TIM10, div); }

// Enable/Disable voltage results are used for brake input (default disabled; register CTLR1.CMP_BK)
INLINE void TIMx_CmpBrkEnable(TIM_t* tim) { tim->CTLR1 |= B12; }
INLINE void TIM1_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM1); }
INLINE void TIM2_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM2); }
INLINE void TIM3_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM3); }
INLINE void TIM4_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM4); }
INLINE void TIM5_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM5); }
INLINE void TIM6_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM6); }
INLINE void TIM7_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM7); }
INLINE void TIM8_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM8); }
INLINE void TIM9_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM9); }
INLINE void TIM10_CmpBrkEnable(void) { TIMx_CmpBrkEnable(TIM10); }

INLINE void TIMx_CmpBrkDisable(TIM_t* tim) { tim->CTLR1 &= ~B12; }
INLINE void TIM1_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM1); }
INLINE void TIM2_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM2); }
INLINE void TIM3_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM3); }
INLINE void TIM4_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM4); }
INLINE void TIM5_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM5); }
INLINE void TIM6_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM6); }
INLINE void TIM7_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM7); }
INLINE void TIM8_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM8); }
INLINE void TIM9_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM9); }
INLINE void TIM10_CmpBrkDisable(void) { TIMx_CmpBrkDisable(TIM10); }

// Select capture value to current counter or to 0xffff when counter overflow (default current counter; register CTLR1.CAPOV)
INLINE void TIMx_CapCnt(TIM_t* tim) { tim->CTLR1 &= ~B14; }
INLINE void TIM1_CapCnt(void) { TIMx_CapCnt(TIM1); }
INLINE void TIM2_CapCnt(void) { TIMx_CapCnt(TIM2); }
INLINE void TIM3_CapCnt(void) { TIMx_CapCnt(TIM3); }
INLINE void TIM4_CapCnt(void) { TIMx_CapCnt(TIM4); }
INLINE void TIM5_CapCnt(void) { TIMx_CapCnt(TIM5); }
INLINE void TIM6_CapCnt(void) { TIMx_CapCnt(TIM6); }
INLINE void TIM7_CapCnt(void) { TIMx_CapCnt(TIM7); }
INLINE void TIM8_CapCnt(void) { TIMx_CapCnt(TIM8); }
INLINE void TIM9_CapCnt(void) { TIMx_CapCnt(TIM9); }
INLINE void TIM10_CapCnt(void) { TIMx_CapCnt(TIM10); }

INLINE void TIMx_CapOver(TIM_t* tim) { tim->CTLR1 |= B14; }
INLINE void TIM1_CapOver(void) { TIMx_CapOver(TIM1); }
INLINE void TIM2_CapOver(void) { TIMx_CapOver(TIM2); }
INLINE void TIM3_CapOver(void) { TIMx_CapOver(TIM3); }
INLINE void TIM4_CapOver(void) { TIMx_CapOver(TIM4); }
INLINE void TIM5_CapOver(void) { TIMx_CapOver(TIM5); }
INLINE void TIM6_CapOver(void) { TIMx_CapOver(TIM6); }
INLINE void TIM7_CapOver(void) { TIMx_CapOver(TIM7); }
INLINE void TIM8_CapOver(void) { TIMx_CapOver(TIM8); }
INLINE void TIM9_CapOver(void) { TIMx_CapOver(TIM9); }
INLINE void TIM10_CapOver(void) { TIMx_CapOver(TIM10); }

// Enable/Disable capture level indication function (default disabled; register CTLR1.CAPLVL)
INLINE void TIMx_CapLvlEnable(TIM_t* tim) { tim->CTLR1 |= B15; }
INLINE void TIM1_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM1); }
INLINE void TIM2_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM2); }
INLINE void TIM3_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM3); }
INLINE void TIM4_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM4); }
INLINE void TIM5_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM5); }
INLINE void TIM6_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM6); }
INLINE void TIM7_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM7); }
INLINE void TIM8_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM8); }
INLINE void TIM9_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM9); }
INLINE void TIM10_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM10); }

INLINE void TIMx_CapLvlDisable(TIM_t* tim) { tim->CTLR1 &= ~B15; }
INLINE void TIM1_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM1); }
INLINE void TIM2_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM2); }
INLINE void TIM3_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM3); }
INLINE void TIM4_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM4); }
INLINE void TIM5_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM5); }
INLINE void TIM6_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM6); }
INLINE void TIM7_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM7); }
INLINE void TIM8_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM8); }
INLINE void TIM9_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM9); }
INLINE void TIM10_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM10); }

// Enable/Disable preloading compare capture bits CCxE, CCxNE and OCxM (default disabled; register CTLR2.CCPC)
// - Only for channels with complementary outputs
INLINE void TIMx_CCPreEnable(TIM_t* tim) { tim->CTLR2 |= B0; }
INLINE void TIM1_CCPreEnable(void) { TIMx_CCPreEnable(TIM1); }
INLINE void TIM2_CCPreEnable(void) { TIMx_CCPreEnable(TIM2); }
INLINE void TIM3_CCPreEnable(void) { TIMx_CCPreEnable(TIM3); }
INLINE void TIM4_CCPreEnable(void) { TIMx_CCPreEnable(TIM4); }
INLINE void TIM5_CCPreEnable(void) { TIMx_CCPreEnable(TIM5); }
INLINE void TIM6_CCPreEnable(void) { TIMx_CCPreEnable(TIM6); }
INLINE void TIM7_CCPreEnable(void) { TIMx_CCPreEnable(TIM7); }
INLINE void TIM8_CCPreEnable(void) { TIMx_CCPreEnable(TIM8); }
INLINE void TIM9_CCPreEnable(void) { TIMx_CCPreEnable(TIM9); }
INLINE void TIM10_CCPreEnable(void) { TIMx_CCPreEnable(TIM10); }

INLINE void TIMx_CCPreDisable(TIM_t* tim) { tim->CTLR2 &= ~B0; }
INLINE void TIM1_CCPreDisable(void) { TIMx_CCPreDisable(TIM1); }
INLINE void TIM2_CCPreDisable(void) { TIMx_CCPreDisable(TIM2); }
INLINE void TIM3_CCPreDisable(void) { TIMx_CCPreDisable(TIM3); }
INLINE void TIM4_CCPreDisable(void) { TIMx_CCPreDisable(TIM4); }
INLINE void TIM5_CCPreDisable(void) { TIMx_CCPreDisable(TIM5); }
INLINE void TIM6_CCPreDisable(void) { TIMx_CCPreDisable(TIM6); }
INLINE void TIM7_CCPreDisable(void) { TIMx_CCPreDisable(TIM7); }
INLINE void TIM8_CCPreDisable(void) { TIMx_CCPreDisable(TIM8); }
INLINE void TIM9_CCPreDisable(void) { TIMx_CCPreDisable(TIM9); }
INLINE void TIM10_CCPreDisable(void) { TIMx_CCPreDisable(TIM10); }

// Enable/Disable update compare capture also with rising edge on TRGI (default disabled; register CTLR2.CCUS)
// - Only for channels with complementary outputs
INLINE void TIMx_CCTrgiEnable(TIM_t* tim) { tim->CTLR2 |= B2; }
INLINE void TIM1_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM1); }
INLINE void TIM2_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM2); }
INLINE void TIM3_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM3); }
INLINE void TIM4_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM4); }
INLINE void TIM5_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM5); }
INLINE void TIM6_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM6); }
INLINE void TIM7_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM7); }
INLINE void TIM8_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM8); }
INLINE void TIM9_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM9); }
INLINE void TIM10_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM10); }

INLINE void TIMx_CCTrgiDisable(TIM_t* tim) { tim->CTLR2 &= ~B2; }
INLINE void TIM1_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM1); }
INLINE void TIM2_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM2); }
INLINE void TIM3_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM3); }
INLINE void TIM4_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM4); }
INLINE void TIM5_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM5); }
INLINE void TIM6_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM6); }
INLINE void TIM7_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM7); }
INLINE void TIM8_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM8); }
INLINE void TIM9_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM9); }
INLINE void TIM10_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM10); }

// Select capture DMA request on update or on compare (default compare; register CTLR2.CCDS)
INLINE void TIMx_CCDMAOnUpdate(TIM_t* tim) { tim->CTLR2 |= B3; }
INLINE void TIM1_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM1); }
INLINE void TIM2_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM2); }
INLINE void TIM3_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM3); }
INLINE void TIM4_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM4); }
INLINE void TIM5_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM5); }
INLINE void TIM6_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM6); }
INLINE void TIM7_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM7); }
INLINE void TIM8_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM8); }
INLINE void TIM9_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM9); }
INLINE void TIM10_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM10); }

INLINE void TIMx_CCDMAOnComp(TIM_t* tim) { tim->CTLR2 &= ~B3; }
INLINE void TIM1_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM1); }
INLINE void TIM2_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM2); }
INLINE void TIM3_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM3); }
INLINE void TIM4_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM4); }
INLINE void TIM5_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM5); }
INLINE void TIM6_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM6); }
INLINE void TIM7_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM7); }
INLINE void TIM8_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM8); }
INLINE void TIM9_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM9); }
INLINE void TIM10_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM10); }

// Select master to slave synchronization mode TIM_MSSYNC_* (select trigger output signal; default reset; register CTLR2.MMS[2:0])
INLINE void TIMx_MSSync(TIM_t* tim, int sync) { tim->CTLR2 = (tim->CTLR1 & ~(7<<4)) | (sync<<4); }
INLINE void TIM1_MSSync(int sync) { TIMx_MSSync(TIM1, sync); }
INLINE void TIM2_MSSync(int sync) { TIMx_MSSync(TIM2, sync); }
INLINE void TIM3_MSSync(int sync) { TIMx_MSSync(TIM3, sync); }
INLINE void TIM4_MSSync(int sync) { TIMx_MSSync(TIM4, sync); }
INLINE void TIM5_MSSync(int sync) { TIMx_MSSync(TIM5, sync); }
INLINE void TIM6_MSSync(int sync) { TIMx_MSSync(TIM6, sync); }
INLINE void TIM7_MSSync(int sync) { TIMx_MSSync(TIM7, sync); }
INLINE void TIM8_MSSync(int sync) { TIMx_MSSync(TIM8, sync); }
INLINE void TIM9_MSSync(int sync) { TIMx_MSSync(TIM9, sync); }
INLINE void TIM10_MSSync(int sync) { TIMx_MSSync(TIM10, sync); }

// Select TI1 input to CH1 or to heterodyn of CH1/CH2/CH3 (default CH1; register CTLR2.TI1S)
INLINE void TIMx_TI1CH1(TIM_t* tim) { tim->CTLR2 &= ~B7; }
INLINE void TIM1_TI1CH1(void) { TIMx_TI1CH1(TIM1); }
INLINE void TIM2_TI1CH1(void) { TIMx_TI1CH1(TIM2); }
INLINE void TIM3_TI1CH1(void) { TIMx_TI1CH1(TIM3); }
INLINE void TIM4_TI1CH1(void) { TIMx_TI1CH1(TIM4); }
INLINE void TIM5_TI1CH1(void) { TIMx_TI1CH1(TIM5); }
INLINE void TIM6_TI1CH1(void) { TIMx_TI1CH1(TIM6); }
INLINE void TIM7_TI1CH1(void) { TIMx_TI1CH1(TIM7); }
INLINE void TIM8_TI1CH1(void) { TIMx_TI1CH1(TIM8); }
INLINE void TIM9_TI1CH1(void) { TIMx_TI1CH1(TIM9); }
INLINE void TIM10_TI1CH1(void) { TIMx_TI1CH1(TIM10); }

INLINE void TIMx_TI1CH123(TIM_t* tim) { tim->CTLR2 |= B7; }
INLINE void TIM1_TI1CH123(void) { TIMx_TI1CH123(TIM1); }
INLINE void TIM2_TI1CH123(void) { TIMx_TI1CH123(TIM2); }
INLINE void TIM3_TI1CH123(void) { TIMx_TI1CH123(TIM3); }
INLINE void TIM4_TI1CH123(void) { TIMx_TI1CH123(TIM4); }
INLINE void TIM5_TI1CH123(void) { TIMx_TI1CH123(TIM5); }
INLINE void TIM6_TI1CH123(void) { TIMx_TI1CH123(TIM6); }
INLINE void TIM7_TI1CH123(void) { TIMx_TI1CH123(TIM7); }
INLINE void TIM8_TI1CH123(void) { TIMx_TI1CH123(TIM8); }
INLINE void TIM9_TI1CH123(void) { TIMx_TI1CH123(TIM9); }
INLINE void TIM10_TI1CH123(void) { TIMx_TI1CH123(TIM10); }

// Select output idle state of OC1 after deadband to 0 or 1 (default 0; register CTLR2.OIS1)
INLINE void TIMx_OI1_0(TIM_t* tim) { tim->CTLR2 &= ~B8; }
INLINE void TIM1_OI1_0(void) { TIMx_OI1_0(TIM1); }
INLINE void TIM2_OI1_0(void) { TIMx_OI1_0(TIM2); }
INLINE void TIM3_OI1_0(void) { TIMx_OI1_0(TIM3); }
INLINE void TIM4_OI1_0(void) { TIMx_OI1_0(TIM4); }
INLINE void TIM5_OI1_0(void) { TIMx_OI1_0(TIM5); }
INLINE void TIM6_OI1_0(void) { TIMx_OI1_0(TIM6); }
INLINE void TIM7_OI1_0(void) { TIMx_OI1_0(TIM7); }
INLINE void TIM8_OI1_0(void) { TIMx_OI1_0(TIM8); }
INLINE void TIM9_OI1_0(void) { TIMx_OI1_0(TIM9); }
INLINE void TIM10_OI1_0(void) { TIMx_OI1_0(TIM10); }

INLINE void TIMx_OI1_1(TIM_t* tim) { tim->CTLR2 |= B8; }
INLINE void TIM1_OI1_1(void) { TIMx_OI1_1(TIM1); }
INLINE void TIM2_OI1_1(void) { TIMx_OI1_1(TIM2); }
INLINE void TIM3_OI1_1(void) { TIMx_OI1_1(TIM3); }
INLINE void TIM4_OI1_1(void) { TIMx_OI1_1(TIM4); }
INLINE void TIM5_OI1_1(void) { TIMx_OI1_1(TIM5); }
INLINE void TIM6_OI1_1(void) { TIMx_OI1_1(TIM6); }
INLINE void TIM7_OI1_1(void) { TIMx_OI1_1(TIM7); }
INLINE void TIM8_OI1_1(void) { TIMx_OI1_1(TIM8); }
INLINE void TIM9_OI1_1(void) { TIMx_OI1_1(TIM9); }
INLINE void TIM10_OI1_1(void) { TIMx_OI1_1(TIM10); }

// Select output idle state of OC1N after deadband to 0 or 1 (default 0; register CTLR2.OIS1N)
INLINE void TIMx_OI1N_0(TIM_t* tim) { tim->CTLR2 &= ~B9; }
INLINE void TIM1_OI1N_0(void) { TIMx_OI1N_0(TIM1); }
INLINE void TIM2_OI1N_0(void) { TIMx_OI1N_0(TIM2); }
INLINE void TIM3_OI1N_0(void) { TIMx_OI1N_0(TIM3); }
INLINE void TIM4_OI1N_0(void) { TIMx_OI1N_0(TIM4); }
INLINE void TIM5_OI1N_0(void) { TIMx_OI1N_0(TIM5); }
INLINE void TIM6_OI1N_0(void) { TIMx_OI1N_0(TIM6); }
INLINE void TIM7_OI1N_0(void) { TIMx_OI1N_0(TIM7); }
INLINE void TIM8_OI1N_0(void) { TIMx_OI1N_0(TIM8); }
INLINE void TIM9_OI1N_0(void) { TIMx_OI1N_0(TIM9); }
INLINE void TIM10_OI1N_0(void) { TIMx_OI1N_0(TIM10); }

INLINE void TIMx_OI1N_1(TIM_t* tim) { tim->CTLR2 |= B9; }
INLINE void TIM1_OI1N_1(void) { TIMx_OI1N_1(TIM1); }
INLINE void TIM2_OI1N_1(void) { TIMx_OI1N_1(TIM2); }
INLINE void TIM3_OI1N_1(void) { TIMx_OI1N_1(TIM3); }
INLINE void TIM4_OI1N_1(void) { TIMx_OI1N_1(TIM4); }
INLINE void TIM5_OI1N_1(void) { TIMx_OI1N_1(TIM5); }
INLINE void TIM6_OI1N_1(void) { TIMx_OI1N_1(TIM6); }
INLINE void TIM7_OI1N_1(void) { TIMx_OI1N_1(TIM7); }
INLINE void TIM8_OI1N_1(void) { TIMx_OI1N_1(TIM8); }
INLINE void TIM9_OI1N_1(void) { TIMx_OI1N_1(TIM9); }
INLINE void TIM10_OI1N_1(void) { TIMx_OI1N_1(TIM10); }

// Select output idle state of OC2 after deadband to 0 or 1 (default 0; register CTLR2.OIS2)
INLINE void TIMx_OI2_0(TIM_t* tim) { tim->CTLR2 &= ~B10; }
INLINE void TIM1_OI2_0(void) { TIMx_OI2_0(TIM1); }
INLINE void TIM2_OI2_0(void) { TIMx_OI2_0(TIM2); }
INLINE void TIM3_OI2_0(void) { TIMx_OI2_0(TIM3); }
INLINE void TIM4_OI2_0(void) { TIMx_OI2_0(TIM4); }
INLINE void TIM5_OI2_0(void) { TIMx_OI2_0(TIM5); }
INLINE void TIM6_OI2_0(void) { TIMx_OI2_0(TIM6); }
INLINE void TIM7_OI2_0(void) { TIMx_OI2_0(TIM7); }
INLINE void TIM8_OI2_0(void) { TIMx_OI2_0(TIM8); }
INLINE void TIM9_OI2_0(void) { TIMx_OI2_0(TIM9); }
INLINE void TIM10_OI2_0(void) { TIMx_OI2_0(TIM10); }

INLINE void TIMx_OI2_1(TIM_t* tim) { tim->CTLR2 |= B10; }
INLINE void TIM1_OI2_1(void) { TIMx_OI2_1(TIM1); }
INLINE void TIM2_OI2_1(void) { TIMx_OI2_1(TIM2); }
INLINE void TIM3_OI2_1(void) { TIMx_OI2_1(TIM3); }
INLINE void TIM4_OI2_1(void) { TIMx_OI2_1(TIM4); }
INLINE void TIM5_OI2_1(void) { TIMx_OI2_1(TIM5); }
INLINE void TIM6_OI2_1(void) { TIMx_OI2_1(TIM6); }
INLINE void TIM7_OI2_1(void) { TIMx_OI2_1(TIM7); }
INLINE void TIM8_OI2_1(void) { TIMx_OI2_1(TIM8); }
INLINE void TIM9_OI2_1(void) { TIMx_OI2_1(TIM9); }
INLINE void TIM10_OI2_1(void) { TIMx_OI2_1(TIM10); }

// Select output idle state of OC2N after deadband to 0 or 1 (default 0; register CTLR2.OIS2N)
INLINE void TIMx_OI2N_0(TIM_t* tim) { tim->CTLR2 &= ~B11; }
INLINE void TIM1_OI2N_0(void) { TIMx_OI2N_0(TIM1); }
INLINE void TIM2_OI2N_0(void) { TIMx_OI2N_0(TIM2); }
INLINE void TIM3_OI2N_0(void) { TIMx_OI2N_0(TIM3); }
INLINE void TIM4_OI2N_0(void) { TIMx_OI2N_0(TIM4); }
INLINE void TIM5_OI2N_0(void) { TIMx_OI2N_0(TIM5); }
INLINE void TIM6_OI2N_0(void) { TIMx_OI2N_0(TIM6); }
INLINE void TIM7_OI2N_0(void) { TIMx_OI2N_0(TIM7); }
INLINE void TIM8_OI2N_0(void) { TIMx_OI2N_0(TIM8); }
INLINE void TIM9_OI2N_0(void) { TIMx_OI2N_0(TIM9); }
INLINE void TIM10_OI2N_0(void) { TIMx_OI2N_0(TIM10); }

INLINE void TIMx_OI2N_1(TIM_t* tim) { tim->CTLR2 |= B11; }
INLINE void TIM1_OI2N_1(void) { TIMx_OI2N_1(TIM1); }
INLINE void TIM2_OI2N_1(void) { TIMx_OI2N_1(TIM2); }
INLINE void TIM3_OI2N_1(void) { TIMx_OI2N_1(TIM3); }
INLINE void TIM4_OI2N_1(void) { TIMx_OI2N_1(TIM4); }
INLINE void TIM5_OI2N_1(void) { TIMx_OI2N_1(TIM5); }
INLINE void TIM6_OI2N_1(void) { TIMx_OI2N_1(TIM6); }
INLINE void TIM7_OI2N_1(void) { TIMx_OI2N_1(TIM7); }
INLINE void TIM8_OI2N_1(void) { TIMx_OI2N_1(TIM8); }
INLINE void TIM9_OI2N_1(void) { TIMx_OI2N_1(TIM9); }
INLINE void TIM10_OI2N_1(void) { TIMx_OI2N_1(TIM10); }

// Select output idle state of OC3 after deadband to 0 or 1 (default 0; register CTLR2.OIS3)
INLINE void TIMx_OI3_0(TIM_t* tim) { tim->CTLR2 &= ~B12; }
INLINE void TIM1_OI3_0(void) { TIMx_OI3_0(TIM1); }
INLINE void TIM2_OI3_0(void) { TIMx_OI3_0(TIM2); }
INLINE void TIM3_OI3_0(void) { TIMx_OI3_0(TIM3); }
INLINE void TIM4_OI3_0(void) { TIMx_OI3_0(TIM4); }
INLINE void TIM5_OI3_0(void) { TIMx_OI3_0(TIM5); }
INLINE void TIM6_OI3_0(void) { TIMx_OI3_0(TIM6); }
INLINE void TIM7_OI3_0(void) { TIMx_OI3_0(TIM7); }
INLINE void TIM8_OI3_0(void) { TIMx_OI3_0(TIM8); }
INLINE void TIM9_OI3_0(void) { TIMx_OI3_0(TIM9); }
INLINE void TIM10_OI3_0(void) { TIMx_OI3_0(TIM10); }

INLINE void TIMx_OI3_1(TIM_t* tim) { tim->CTLR2 |= B12; }
INLINE void TIM1_OI3_1(void) { TIMx_OI3_1(TIM1); }
INLINE void TIM2_OI3_1(void) { TIMx_OI3_1(TIM2); }
INLINE void TIM3_OI3_1(void) { TIMx_OI3_1(TIM3); }
INLINE void TIM4_OI3_1(void) { TIMx_OI3_1(TIM4); }
INLINE void TIM5_OI3_1(void) { TIMx_OI3_1(TIM5); }
INLINE void TIM6_OI3_1(void) { TIMx_OI3_1(TIM6); }
INLINE void TIM7_OI3_1(void) { TIMx_OI3_1(TIM7); }
INLINE void TIM8_OI3_1(void) { TIMx_OI3_1(TIM8); }
INLINE void TIM9_OI3_1(void) { TIMx_OI3_1(TIM9); }
INLINE void TIM10_OI3_1(void) { TIMx_OI3_1(TIM10); }

// Select output idle state of OC3N after deadband to 0 or 1 (default 0; register CTLR2.OIS3N)
INLINE void TIMx_OI3N_0(TIM_t* tim) { tim->CTLR2 &= ~B13; }
INLINE void TIM1_OI3N_0(void) { TIMx_OI3N_0(TIM1); }
INLINE void TIM2_OI3N_0(void) { TIMx_OI3N_0(TIM2); }
INLINE void TIM3_OI3N_0(void) { TIMx_OI3N_0(TIM3); }
INLINE void TIM4_OI3N_0(void) { TIMx_OI3N_0(TIM4); }
INLINE void TIM5_OI3N_0(void) { TIMx_OI3N_0(TIM5); }
INLINE void TIM6_OI3N_0(void) { TIMx_OI3N_0(TIM6); }
INLINE void TIM7_OI3N_0(void) { TIMx_OI3N_0(TIM7); }
INLINE void TIM8_OI3N_0(void) { TIMx_OI3N_0(TIM8); }
INLINE void TIM9_OI3N_0(void) { TIMx_OI3N_0(TIM9); }
INLINE void TIM10_OI3N_0(void) { TIMx_OI3N_0(TIM10); }

INLINE void TIMx_OI3N_1(TIM_t* tim) { tim->CTLR2 |= B13; }
INLINE void TIM1_OI3N_1(void) { TIMx_OI3N_1(TIM1); }
INLINE void TIM2_OI3N_1(void) { TIMx_OI3N_1(TIM2); }
INLINE void TIM3_OI3N_1(void) { TIMx_OI3N_1(TIM3); }
INLINE void TIM4_OI3N_1(void) { TIMx_OI3N_1(TIM4); }
INLINE void TIM5_OI3N_1(void) { TIMx_OI3N_1(TIM5); }
INLINE void TIM6_OI3N_1(void) { TIMx_OI3N_1(TIM6); }
INLINE void TIM7_OI3N_1(void) { TIMx_OI3N_1(TIM7); }
INLINE void TIM8_OI3N_1(void) { TIMx_OI3N_1(TIM8); }
INLINE void TIM9_OI3N_1(void) { TIMx_OI3N_1(TIM9); }
INLINE void TIM10_OI3N_1(void) { TIMx_OI3N_1(TIM10); }

// Select output idle state of OC4 after deadband to 0 or 1 (default 0; register CTLR2.OIS4)
INLINE void TIMx_OI4_0(TIM_t* tim) { tim->CTLR2 &= ~B14; }
INLINE void TIM1_OI4_0(void) { TIMx_OI4_0(TIM1); }
INLINE void TIM2_OI4_0(void) { TIMx_OI4_0(TIM2); }
INLINE void TIM3_OI4_0(void) { TIMx_OI4_0(TIM3); }
INLINE void TIM4_OI4_0(void) { TIMx_OI4_0(TIM4); }
INLINE void TIM5_OI4_0(void) { TIMx_OI4_0(TIM5); }
INLINE void TIM6_OI4_0(void) { TIMx_OI4_0(TIM6); }
INLINE void TIM7_OI4_0(void) { TIMx_OI4_0(TIM7); }
INLINE void TIM8_OI4_0(void) { TIMx_OI4_0(TIM8); }
INLINE void TIM9_OI4_0(void) { TIMx_OI4_0(TIM9); }
INLINE void TIM10_OI4_0(void) { TIMx_OI4_0(TIM10); }

INLINE void TIMx_OI4_1(TIM_t* tim) { tim->CTLR2 |= B14; }
INLINE void TIM1_OI4_1(void) { TIMx_OI4_1(TIM1); }
INLINE void TIM2_OI4_1(void) { TIMx_OI4_1(TIM2); }
INLINE void TIM3_OI4_1(void) { TIMx_OI4_1(TIM3); }
INLINE void TIM4_OI4_1(void) { TIMx_OI4_1(TIM4); }
INLINE void TIM5_OI4_1(void) { TIMx_OI4_1(TIM5); }
INLINE void TIM6_OI4_1(void) { TIMx_OI4_1(TIM6); }
INLINE void TIM7_OI4_1(void) { TIMx_OI4_1(TIM7); }
INLINE void TIM8_OI4_1(void) { TIMx_OI4_1(TIM8); }
INLINE void TIM9_OI4_1(void) { TIMx_OI4_1(TIM9); }
INLINE void TIM10_OI4_1(void) { TIMx_OI4_1(TIM10); }

// === Slave mode

// Select input mode TIM_INMODE_* (default internal clock CK_INT; register SMCFGR.SMS[2:0])
INLINE void TIMx_InMode(TIM_t* tim, int inmode) { tim->SMCFGR = (tim->SMCFGR & ~7) | inmode; }
INLINE void TIM1_InMode(int inmode) { TIMx_InMode(TIM1, inmode); }
INLINE void TIM2_InMode(int inmode) { TIMx_InMode(TIM2, inmode); }
INLINE void TIM3_InMode(int inmode) { TIMx_InMode(TIM3, inmode); }
INLINE void TIM4_InMode(int inmode) { TIMx_InMode(TIM4, inmode); }
INLINE void TIM5_InMode(int inmode) { TIMx_InMode(TIM5, inmode); }
INLINE void TIM6_InMode(int inmode) { TIMx_InMode(TIM6, inmode); }
INLINE void TIM7_InMode(int inmode) { TIMx_InMode(TIM7, inmode); }
INLINE void TIM8_InMode(int inmode) { TIMx_InMode(TIM8, inmode); }
INLINE void TIM9_InMode(int inmode) { TIMx_InMode(TIM9, inmode); }
INLINE void TIM10_InMode(int inmode) { TIMx_InMode(TIM10, inmode); }

// Select trigger selection TIM_TRIG_* (input mode must be internal clock; default internal trigger 0 ITR0; register SMCFGR.TS[2:0])
INLINE void TIMx_Trig(TIM_t* tim, int trig) { tim->SMCFGR = (tim->SMCFGR & ~(7<<4)) | (trig<<4); }
INLINE void TIM1_Trig(int trig) { TIMx_Trig(TIM1, trig); }
INLINE void TIM2_Trig(int trig) { TIMx_Trig(TIM2, trig); }
INLINE void TIM3_Trig(int trig) { TIMx_Trig(TIM3, trig); }
INLINE void TIM4_Trig(int trig) { TIMx_Trig(TIM4, trig); }
INLINE void TIM5_Trig(int trig) { TIMx_Trig(TIM5, trig); }
INLINE void TIM6_Trig(int trig) { TIMx_Trig(TIM6, trig); }
INLINE void TIM7_Trig(int trig) { TIMx_Trig(TIM7, trig); }
INLINE void TIM8_Trig(int trig) { TIMx_Trig(TIM8, trig); }
INLINE void TIM9_Trig(int trig) { TIMx_Trig(TIM9, trig); }
INLINE void TIM10_Trig(int trig) { TIMx_Trig(TIM10, trig); }

// Enable/Disable delay event of trigger input TRGI (default disabled; register SMCFGR.MSM)
INLINE void TIMx_TRGIDelayEnable(TIM_t* tim) { tim->SMCFGR |= B7; }
INLINE void TIM1_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM1); }
INLINE void TIM2_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM2); }
INLINE void TIM3_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM3); }
INLINE void TIM4_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM4); }
INLINE void TIM5_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM5); }
INLINE void TIM6_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM6); }
INLINE void TIM7_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM7); }
INLINE void TIM8_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM8); }
INLINE void TIM9_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM9); }
INLINE void TIM10_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM10); }

INLINE void TIMx_TRGIDelayDisable(TIM_t* tim) { tim->SMCFGR &= ~B7; }
INLINE void TIM1_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM1); }
INLINE void TIM2_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM2); }
INLINE void TIM3_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM3); }
INLINE void TIM4_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM4); }
INLINE void TIM5_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM5); }
INLINE void TIM6_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM6); }
INLINE void TIM7_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM7); }
INLINE void TIM8_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM8); }
INLINE void TIM9_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM9); }
INLINE void TIM10_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM10); }

// Select external filtering TIM_FILTER_* (record up to N events; default 0; register SMCFGR.ETF[3:0])
INLINE void TIMx_Filter(TIM_t* tim, int filt) { tim->SMCFGR = (tim->SMCFGR & ~(0x0f<<8)) | (filt<<8); }
INLINE void TIM1_Filter(int filt) { TIMx_Filter(TIM1, filt); }
INLINE void TIM2_Filter(int filt) { TIMx_Filter(TIM2, filt); }
INLINE void TIM3_Filter(int filt) { TIMx_Filter(TIM3, filt); }
INLINE void TIM4_Filter(int filt) { TIMx_Filter(TIM4, filt); }
INLINE void TIM5_Filter(int filt) { TIMx_Filter(TIM5, filt); }
INLINE void TIM6_Filter(int filt) { TIMx_Filter(TIM6, filt); }
INLINE void TIM7_Filter(int filt) { TIMx_Filter(TIM7, filt); }
INLINE void TIM8_Filter(int filt) { TIMx_Filter(TIM8, filt); }
INLINE void TIM9_Filter(int filt) { TIMx_Filter(TIM9, filt); }
INLINE void TIM10_Filter(int filt) { TIMx_Filter(TIM10, filt); }

// Set prescaler of external trigger signal TIM_ETRPDIV_* (default off; register SMCFGR.ETPS[1:0])
INLINE void TIMx_ETRPDiv(TIM_t* tim, int div) { tim->SMCFGR = (tim->SMCFGR & ~(3<<12)) | (div<<12); }
INLINE void TIM1_ETRPDiv(int div) { TIMx_ETRPDiv(TIM1, div); }
INLINE void TIM2_ETRPDiv(int div) { TIMx_ETRPDiv(TIM2, div); }
INLINE void TIM3_ETRPDiv(int div) { TIMx_ETRPDiv(TIM3, div); }
INLINE void TIM4_ETRPDiv(int div) { TIMx_ETRPDiv(TIM4, div); }
INLINE void TIM5_ETRPDiv(int div) { TIMx_ETRPDiv(TIM5, div); }
INLINE void TIM6_ETRPDiv(int div) { TIMx_ETRPDiv(TIM6, div); }
INLINE void TIM7_ETRPDiv(int div) { TIMx_ETRPDiv(TIM7, div); }
INLINE void TIM8_ETRPDiv(int div) { TIMx_ETRPDiv(TIM8, div); }
INLINE void TIM9_ETRPDiv(int div) { TIMx_ETRPDiv(TIM9, div); }
INLINE void TIM10_ETRPDiv(int div) { TIMx_ETRPDiv(TIM10, div); }

// Enable/Disable external clock mode 2 (default disabled; register SMCFGR.ECE)
INLINE void TIMx_EC2Enable(TIM_t* tim) { tim->SMCFGR |= B14; }
INLINE void TIM1_EC2Enable(void) { TIMx_EC2Enable(TIM1); }
INLINE void TIM2_EC2Enable(void) { TIMx_EC2Enable(TIM2); }
INLINE void TIM3_EC2Enable(void) { TIMx_EC2Enable(TIM3); }
INLINE void TIM4_EC2Enable(void) { TIMx_EC2Enable(TIM4); }
INLINE void TIM5_EC2Enable(void) { TIMx_EC2Enable(TIM5); }
INLINE void TIM6_EC2Enable(void) { TIMx_EC2Enable(TIM6); }
INLINE void TIM7_EC2Enable(void) { TIMx_EC2Enable(TIM7); }
INLINE void TIM8_EC2Enable(void) { TIMx_EC2Enable(TIM8); }
INLINE void TIM9_EC2Enable(void) { TIMx_EC2Enable(TIM9); }
INLINE void TIM10_EC2Enable(void) { TIMx_EC2Enable(TIM10); }

INLINE void TIMx_EC2Disable(TIM_t* tim) { tim->SMCFGR &= ~B14; }
INLINE void TIM1_EC2Disable(void) { TIMx_EC2Disable(TIM1); }
INLINE void TIM2_EC2Disable(void) { TIMx_EC2Disable(TIM2); }
INLINE void TIM3_EC2Disable(void) { TIMx_EC2Disable(TIM3); }
INLINE void TIM4_EC2Disable(void) { TIMx_EC2Disable(TIM4); }
INLINE void TIM5_EC2Disable(void) { TIMx_EC2Disable(TIM5); }
INLINE void TIM6_EC2Disable(void) { TIMx_EC2Disable(TIM6); }
INLINE void TIM7_EC2Disable(void) { TIMx_EC2Disable(TIM7); }
INLINE void TIM8_EC2Disable(void) { TIMx_EC2Disable(TIM8); }
INLINE void TIM9_EC2Disable(void) { TIMx_EC2Disable(TIM9); }
INLINE void TIM10_EC2Disable(void) { TIMx_EC2Disable(TIM10); }

// Select ETR trigger polarity to HIGH/Rising or LOW/Falling (default HIGH/Rising; register SMCFGR.ETP)
INLINE void TIMx_ETRHighRise(TIM_t* tim) { tim->SMCFGR &= ~B15; }
INLINE void TIM1_ETRHighRise(void) { TIMx_ETRHighRise(TIM1); }
INLINE void TIM2_ETRHighRise(void) { TIMx_ETRHighRise(TIM2); }
INLINE void TIM3_ETRHighRise(void) { TIMx_ETRHighRise(TIM3); }
INLINE void TIM4_ETRHighRise(void) { TIMx_ETRHighRise(TIM4); }
INLINE void TIM5_ETRHighRise(void) { TIMx_ETRHighRise(TIM5); }
INLINE void TIM6_ETRHighRise(void) { TIMx_ETRHighRise(TIM6); }
INLINE void TIM7_ETRHighRise(void) { TIMx_ETRHighRise(TIM7); }
INLINE void TIM8_ETRHighRise(void) { TIMx_ETRHighRise(TIM8); }
INLINE void TIM9_ETRHighRise(void) { TIMx_ETRHighRise(TIM9); }
INLINE void TIM10_ETRHighRise(void) { TIMx_ETRHighRise(TIM10); }

INLINE void TIMx_ETRLowFall(TIM_t* tim) { tim->SMCFGR |= B15; }
INLINE void TIM1_ETRLowFall(void) { TIMx_ETRLowFall(TIM1); }
INLINE void TIM2_ETRLowFall(void) { TIMx_ETRLowFall(TIM2); }
INLINE void TIM3_ETRLowFall(void) { TIMx_ETRLowFall(TIM3); }
INLINE void TIM4_ETRLowFall(void) { TIMx_ETRLowFall(TIM4); }
INLINE void TIM5_ETRLowFall(void) { TIMx_ETRLowFall(TIM5); }
INLINE void TIM6_ETRLowFall(void) { TIMx_ETRLowFall(TIM6); }
INLINE void TIM7_ETRLowFall(void) { TIMx_ETRLowFall(TIM7); }
INLINE void TIM8_ETRLowFall(void) { TIMx_ETRLowFall(TIM8); }
INLINE void TIM9_ETRLowFall(void) { TIMx_ETRLowFall(TIM9); }
INLINE void TIM10_ETRLowFall(void) { TIMx_ETRLowFall(TIM10); }

// === DMA and Interrupt

// Enable/Disable update interrupt (default disabled; register DMAINTENR.UIE)
INLINE void TIMx_UpIntEnable(TIM_t* tim) { tim->DMAINTENR |= B0; }
INLINE void TIM1_UpIntEnable(void) { TIMx_UpIntEnable(TIM1); }
INLINE void TIM2_UpIntEnable(void) { TIMx_UpIntEnable(TIM2); }
INLINE void TIM3_UpIntEnable(void) { TIMx_UpIntEnable(TIM3); }
INLINE void TIM4_UpIntEnable(void) { TIMx_UpIntEnable(TIM4); }
INLINE void TIM5_UpIntEnable(void) { TIMx_UpIntEnable(TIM5); }
INLINE void TIM6_UpIntEnable(void) { TIMx_UpIntEnable(TIM6); }
INLINE void TIM7_UpIntEnable(void) { TIMx_UpIntEnable(TIM7); }
INLINE void TIM8_UpIntEnable(void) { TIMx_UpIntEnable(TIM8); }
INLINE void TIM9_UpIntEnable(void) { TIMx_UpIntEnable(TIM9); }
INLINE void TIM10_UpIntEnable(void) { TIMx_UpIntEnable(TIM10); }

INLINE void TIMx_UpIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B0; }
INLINE void TIM1_UpIntDisable(void) { TIMx_UpIntDisable(TIM1); }
INLINE void TIM2_UpIntDisable(void) { TIMx_UpIntDisable(TIM2); }
INLINE void TIM3_UpIntDisable(void) { TIMx_UpIntDisable(TIM3); }
INLINE void TIM4_UpIntDisable(void) { TIMx_UpIntDisable(TIM4); }
INLINE void TIM5_UpIntDisable(void) { TIMx_UpIntDisable(TIM5); }
INLINE void TIM6_UpIntDisable(void) { TIMx_UpIntDisable(TIM6); }
INLINE void TIM7_UpIntDisable(void) { TIMx_UpIntDisable(TIM7); }
INLINE void TIM8_UpIntDisable(void) { TIMx_UpIntDisable(TIM8); }
INLINE void TIM9_UpIntDisable(void) { TIMx_UpIntDisable(TIM9); }
INLINE void TIM10_UpIntDisable(void) { TIMx_UpIntDisable(TIM10); }

// Check/Clear update interrupt flag (register INTFR.UIF)
INLINE Bool TIMx_UpInt(TIM_t* tim) { return (tim->INTFR & B0) != 0; }
INLINE Bool TIM1_UpInt(void) { return TIMx_UpInt(TIM1); }
INLINE Bool TIM2_UpInt(void) { return TIMx_UpInt(TIM2); }
INLINE Bool TIM3_UpInt(void) { return TIMx_UpInt(TIM3); }
INLINE Bool TIM4_UpInt(void) { return TIMx_UpInt(TIM4); }
INLINE Bool TIM5_UpInt(void) { return TIMx_UpInt(TIM5); }
INLINE Bool TIM6_UpInt(void) { return TIMx_UpInt(TIM6); }
INLINE Bool TIM7_UpInt(void) { return TIMx_UpInt(TIM7); }
INLINE Bool TIM8_UpInt(void) { return TIMx_UpInt(TIM8); }
INLINE Bool TIM9_UpInt(void) { return TIMx_UpInt(TIM9); }
INLINE Bool TIM10_UpInt(void) { return TIMx_UpInt(TIM10); }

INLINE void TIMx_UpIntClr(TIM_t* tim) { tim->INTFR &= ~B0; }
INLINE void TIM1_UpIntClr(void) { TIMx_UpIntClr(TIM1); }
INLINE void TIM2_UpIntClr(void) { TIMx_UpIntClr(TIM2); }
INLINE void TIM3_UpIntClr(void) { TIMx_UpIntClr(TIM3); }
INLINE void TIM4_UpIntClr(void) { TIMx_UpIntClr(TIM4); }
INLINE void TIM5_UpIntClr(void) { TIMx_UpIntClr(TIM5); }
INLINE void TIM6_UpIntClr(void) { TIMx_UpIntClr(TIM6); }
INLINE void TIM7_UpIntClr(void) { TIMx_UpIntClr(TIM7); }
INLINE void TIM8_UpIntClr(void) { TIMx_UpIntClr(TIM8); }
INLINE void TIM9_UpIntClr(void) { TIMx_UpIntClr(TIM9); }
INLINE void TIM10_UpIntClr(void) { TIMx_UpIntClr(TIM10); }

// Enable/Disable compare capture channel 1 interrupt (default disabled; register DMAINTENR.CC1IE)
INLINE void TIMx_CC1IntEnable(TIM_t* tim) { tim->DMAINTENR |= B1; }
INLINE void TIM1_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM1); }
INLINE void TIM2_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM2); }
INLINE void TIM3_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM3); }
INLINE void TIM4_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM4); }
INLINE void TIM5_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM5); }
INLINE void TIM6_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM6); }
INLINE void TIM7_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM7); }
INLINE void TIM8_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM8); }
INLINE void TIM9_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM9); }
INLINE void TIM10_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM10); }

INLINE void TIMx_CC1IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B1; }
INLINE void TIM1_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM1); }
INLINE void TIM2_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM2); }
INLINE void TIM3_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM3); }
INLINE void TIM4_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM4); }
INLINE void TIM5_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM5); }
INLINE void TIM6_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM6); }
INLINE void TIM7_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM7); }
INLINE void TIM8_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM8); }
INLINE void TIM9_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM9); }
INLINE void TIM10_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM10); }

// Check/Clear compare capture channel 1 interrupt flag (register INTFR.CC1IF)
INLINE Bool TIMx_CC1Int(TIM_t* tim) { return (tim->INTFR & B1) != 0; }
INLINE Bool TIM1_CC1Int(void) { return TIMx_CC1Int(TIM1); }
INLINE Bool TIM2_CC1Int(void) { return TIMx_CC1Int(TIM2); }
INLINE Bool TIM3_CC1Int(void) { return TIMx_CC1Int(TIM3); }
INLINE Bool TIM4_CC1Int(void) { return TIMx_CC1Int(TIM4); }
INLINE Bool TIM5_CC1Int(void) { return TIMx_CC1Int(TIM5); }
INLINE Bool TIM6_CC1Int(void) { return TIMx_CC1Int(TIM6); }
INLINE Bool TIM7_CC1Int(void) { return TIMx_CC1Int(TIM7); }
INLINE Bool TIM8_CC1Int(void) { return TIMx_CC1Int(TIM8); }
INLINE Bool TIM9_CC1Int(void) { return TIMx_CC1Int(TIM9); }
INLINE Bool TIM10_CC1Int(void) { return TIMx_CC1Int(TIM10); }

INLINE void TIMx_CC1IntClr(TIM_t* tim) { tim->INTFR &= ~B1; }
INLINE void TIM1_CC1IntClr(void) { TIMx_CC1IntClr(TIM1); }
INLINE void TIM2_CC1IntClr(void) { TIMx_CC1IntClr(TIM2); }
INLINE void TIM3_CC1IntClr(void) { TIMx_CC1IntClr(TIM3); }
INLINE void TIM4_CC1IntClr(void) { TIMx_CC1IntClr(TIM4); }
INLINE void TIM5_CC1IntClr(void) { TIMx_CC1IntClr(TIM5); }
INLINE void TIM6_CC1IntClr(void) { TIMx_CC1IntClr(TIM6); }
INLINE void TIM7_CC1IntClr(void) { TIMx_CC1IntClr(TIM7); }
INLINE void TIM8_CC1IntClr(void) { TIMx_CC1IntClr(TIM8); }
INLINE void TIM9_CC1IntClr(void) { TIMx_CC1IntClr(TIM9); }
INLINE void TIM10_CC1IntClr(void) { TIMx_CC1IntClr(TIM10); }

// Enable/Disable compare capture channel 2 interrupt (default disabled; register DMAINTENR.CC2IE)
INLINE void TIMx_CC2IntEnable(TIM_t* tim) { tim->DMAINTENR |= B2; }
INLINE void TIM1_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM1); }
INLINE void TIM2_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM2); }
INLINE void TIM3_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM3); }
INLINE void TIM4_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM4); }
INLINE void TIM5_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM5); }
INLINE void TIM6_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM6); }
INLINE void TIM7_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM7); }
INLINE void TIM8_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM8); }
INLINE void TIM9_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM9); }
INLINE void TIM10_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM10); }

INLINE void TIMx_CC2IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B2; }
INLINE void TIM1_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM1); }
INLINE void TIM2_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM2); }
INLINE void TIM3_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM3); }
INLINE void TIM4_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM4); }
INLINE void TIM5_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM5); }
INLINE void TIM6_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM6); }
INLINE void TIM7_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM7); }
INLINE void TIM8_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM8); }
INLINE void TIM9_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM9); }
INLINE void TIM10_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM10); }

// Check/Clear compare capture channel 2 interrupt flag (register INTFR.CC2IF)
INLINE Bool TIMx_CC2Int(TIM_t* tim) { return (tim->INTFR & B2) != 0; }
INLINE Bool TIM1_CC2Int(void) { return TIMx_CC2Int(TIM1); }
INLINE Bool TIM2_CC2Int(void) { return TIMx_CC2Int(TIM2); }
INLINE Bool TIM3_CC2Int(void) { return TIMx_CC2Int(TIM3); }
INLINE Bool TIM4_CC2Int(void) { return TIMx_CC2Int(TIM4); }
INLINE Bool TIM5_CC2Int(void) { return TIMx_CC2Int(TIM5); }
INLINE Bool TIM6_CC2Int(void) { return TIMx_CC2Int(TIM6); }
INLINE Bool TIM7_CC2Int(void) { return TIMx_CC2Int(TIM7); }
INLINE Bool TIM8_CC2Int(void) { return TIMx_CC2Int(TIM8); }
INLINE Bool TIM9_CC2Int(void) { return TIMx_CC2Int(TIM9); }
INLINE Bool TIM10_CC2Int(void) { return TIMx_CC2Int(TIM10); }

INLINE void TIMx_CC2IntClr(TIM_t* tim) { tim->INTFR &= ~B2; }
INLINE void TIM1_CC2IntClr(void) { TIMx_CC2IntClr(TIM1); }
INLINE void TIM2_CC2IntClr(void) { TIMx_CC2IntClr(TIM2); }
INLINE void TIM3_CC2IntClr(void) { TIMx_CC2IntClr(TIM3); }
INLINE void TIM4_CC2IntClr(void) { TIMx_CC2IntClr(TIM4); }
INLINE void TIM5_CC2IntClr(void) { TIMx_CC2IntClr(TIM5); }
INLINE void TIM6_CC2IntClr(void) { TIMx_CC2IntClr(TIM6); }
INLINE void TIM7_CC2IntClr(void) { TIMx_CC2IntClr(TIM7); }
INLINE void TIM8_CC2IntClr(void) { TIMx_CC2IntClr(TIM8); }
INLINE void TIM9_CC2IntClr(void) { TIMx_CC2IntClr(TIM9); }
INLINE void TIM10_CC2IntClr(void) { TIMx_CC2IntClr(TIM10); }

// Enable/Disable compare capture channel 3 interrupt (default disabled; register DMAINTENR.CC3IE)
INLINE void TIMx_CC3IntEnable(TIM_t* tim) { tim->DMAINTENR |= B3; }
INLINE void TIM1_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM1); }
INLINE void TIM2_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM2); }
INLINE void TIM3_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM3); }
INLINE void TIM4_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM4); }
INLINE void TIM5_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM5); }
INLINE void TIM6_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM6); }
INLINE void TIM7_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM7); }
INLINE void TIM8_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM8); }
INLINE void TIM9_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM9); }
INLINE void TIM10_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM10); }

INLINE void TIMx_CC3IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B3; }
INLINE void TIM1_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM1); }
INLINE void TIM2_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM2); }
INLINE void TIM3_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM3); }
INLINE void TIM4_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM4); }
INLINE void TIM5_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM5); }
INLINE void TIM6_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM6); }
INLINE void TIM7_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM7); }
INLINE void TIM8_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM8); }
INLINE void TIM9_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM9); }
INLINE void TIM10_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM10); }

// Check/Clear compare capture channel 3 interrupt flag (register INTFR.CC3IF)
INLINE Bool TIMx_CC3Int(TIM_t* tim) { return (tim->INTFR & B3) != 0; }
INLINE Bool TIM1_CC3Int(void) { return TIMx_CC3Int(TIM1); }
INLINE Bool TIM2_CC3Int(void) { return TIMx_CC3Int(TIM2); }
INLINE Bool TIM3_CC3Int(void) { return TIMx_CC3Int(TIM3); }
INLINE Bool TIM4_CC3Int(void) { return TIMx_CC3Int(TIM4); }
INLINE Bool TIM5_CC3Int(void) { return TIMx_CC3Int(TIM5); }
INLINE Bool TIM6_CC3Int(void) { return TIMx_CC3Int(TIM6); }
INLINE Bool TIM7_CC3Int(void) { return TIMx_CC3Int(TIM7); }
INLINE Bool TIM8_CC3Int(void) { return TIMx_CC3Int(TIM8); }
INLINE Bool TIM9_CC3Int(void) { return TIMx_CC3Int(TIM9); }
INLINE Bool TIM10_CC3Int(void) { return TIMx_CC3Int(TIM10); }

INLINE void TIMx_CC3IntClr(TIM_t* tim) { tim->INTFR &= ~B3; }
INLINE void TIM1_CC3IntClr(void) { TIMx_CC3IntClr(TIM1); }
INLINE void TIM2_CC3IntClr(void) { TIMx_CC3IntClr(TIM2); }
INLINE void TIM3_CC3IntClr(void) { TIMx_CC3IntClr(TIM3); }
INLINE void TIM4_CC3IntClr(void) { TIMx_CC3IntClr(TIM4); }
INLINE void TIM5_CC3IntClr(void) { TIMx_CC3IntClr(TIM5); }
INLINE void TIM6_CC3IntClr(void) { TIMx_CC3IntClr(TIM6); }
INLINE void TIM7_CC3IntClr(void) { TIMx_CC3IntClr(TIM7); }
INLINE void TIM8_CC3IntClr(void) { TIMx_CC3IntClr(TIM8); }
INLINE void TIM9_CC3IntClr(void) { TIMx_CC3IntClr(TIM9); }
INLINE void TIM10_CC3IntClr(void) { TIMx_CC3IntClr(TIM10); }

// Enable/Disable compare capture channel 4 interrupt (default disabled; register DMAINTENR.CC4IE)
INLINE void TIMx_CC4IntEnable(TIM_t* tim) { tim->DMAINTENR |= B4; }
INLINE void TIM1_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM1); }
INLINE void TIM2_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM2); }
INLINE void TIM3_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM3); }
INLINE void TIM4_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM4); }
INLINE void TIM5_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM5); }
INLINE void TIM6_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM6); }
INLINE void TIM7_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM7); }
INLINE void TIM8_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM8); }
INLINE void TIM9_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM9); }
INLINE void TIM10_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM10); }

INLINE void TIMx_CC4IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B4; }
INLINE void TIM1_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM1); }
INLINE void TIM2_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM2); }
INLINE void TIM3_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM3); }
INLINE void TIM4_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM4); }
INLINE void TIM5_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM5); }
INLINE void TIM6_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM6); }
INLINE void TIM7_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM7); }
INLINE void TIM8_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM8); }
INLINE void TIM9_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM9); }
INLINE void TIM10_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM10); }

// Check/Clear compare capture channel 4 interrupt flag (register INTFR.CC4IF)
INLINE Bool TIMx_CC4Int(TIM_t* tim) { return (tim->INTFR & B4) != 0; }
INLINE Bool TIM1_CC4Int(void) { return TIMx_CC4Int(TIM1); }
INLINE Bool TIM2_CC4Int(void) { return TIMx_CC4Int(TIM2); }
INLINE Bool TIM3_CC4Int(void) { return TIMx_CC4Int(TIM3); }
INLINE Bool TIM4_CC4Int(void) { return TIMx_CC4Int(TIM4); }
INLINE Bool TIM5_CC4Int(void) { return TIMx_CC4Int(TIM5); }
INLINE Bool TIM6_CC4Int(void) { return TIMx_CC4Int(TIM6); }
INLINE Bool TIM7_CC4Int(void) { return TIMx_CC4Int(TIM7); }
INLINE Bool TIM8_CC4Int(void) { return TIMx_CC4Int(TIM8); }
INLINE Bool TIM9_CC4Int(void) { return TIMx_CC4Int(TIM9); }
INLINE Bool TIM10_CC4Int(void) { return TIMx_CC4Int(TIM10); }

INLINE void TIMx_CC4IntClr(TIM_t* tim) { tim->INTFR &= ~B4; }
INLINE void TIM1_CC4IntClr(void) { TIMx_CC4IntClr(TIM1); }
INLINE void TIM2_CC4IntClr(void) { TIMx_CC4IntClr(TIM2); }
INLINE void TIM3_CC4IntClr(void) { TIMx_CC4IntClr(TIM3); }
INLINE void TIM4_CC4IntClr(void) { TIMx_CC4IntClr(TIM4); }
INLINE void TIM5_CC4IntClr(void) { TIMx_CC4IntClr(TIM5); }
INLINE void TIM6_CC4IntClr(void) { TIMx_CC4IntClr(TIM6); }
INLINE void TIM7_CC4IntClr(void) { TIMx_CC4IntClr(TIM7); }
INLINE void TIM8_CC4IntClr(void) { TIMx_CC4IntClr(TIM8); }
INLINE void TIM9_CC4IntClr(void) { TIMx_CC4IntClr(TIM9); }
INLINE void TIM10_CC4IntClr(void) { TIMx_CC4IntClr(TIM10); }

// Enable/Disable COM interrupt (default disabled; register DMAINTENR.COMIE)
INLINE void TIMx_COMIntEnable(TIM_t* tim) { tim->DMAINTENR |= B5; }
INLINE void TIM1_COMIntEnable(void) { TIMx_COMIntEnable(TIM1); }
INLINE void TIM2_COMIntEnable(void) { TIMx_COMIntEnable(TIM2); }
INLINE void TIM3_COMIntEnable(void) { TIMx_COMIntEnable(TIM3); }
INLINE void TIM4_COMIntEnable(void) { TIMx_COMIntEnable(TIM4); }
INLINE void TIM5_COMIntEnable(void) { TIMx_COMIntEnable(TIM5); }
INLINE void TIM6_COMIntEnable(void) { TIMx_COMIntEnable(TIM6); }
INLINE void TIM7_COMIntEnable(void) { TIMx_COMIntEnable(TIM7); }
INLINE void TIM8_COMIntEnable(void) { TIMx_COMIntEnable(TIM8); }
INLINE void TIM9_COMIntEnable(void) { TIMx_COMIntEnable(TIM9); }
INLINE void TIM10_COMIntEnable(void) { TIMx_COMIntEnable(TIM10); }

INLINE void TIMx_COMIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B5; }
INLINE void TIM1_COMIntDisable(void) { TIMx_COMIntDisable(TIM1); }
INLINE void TIM2_COMIntDisable(void) { TIMx_COMIntDisable(TIM2); }
INLINE void TIM3_COMIntDisable(void) { TIMx_COMIntDisable(TIM3); }
INLINE void TIM4_COMIntDisable(void) { TIMx_COMIntDisable(TIM4); }
INLINE void TIM5_COMIntDisable(void) { TIMx_COMIntDisable(TIM5); }
INLINE void TIM6_COMIntDisable(void) { TIMx_COMIntDisable(TIM6); }
INLINE void TIM7_COMIntDisable(void) { TIMx_COMIntDisable(TIM7); }
INLINE void TIM8_COMIntDisable(void) { TIMx_COMIntDisable(TIM8); }
INLINE void TIM9_COMIntDisable(void) { TIMx_COMIntDisable(TIM9); }
INLINE void TIM10_COMIntDisable(void) { TIMx_COMIntDisable(TIM10); }

// Check/Clear COM interrupt flag (register INTFR.COMIF)
INLINE Bool TIMx_COMInt(TIM_t* tim) { return (tim->INTFR & B5) != 0; }
INLINE Bool TIM1_COMInt(void) { return TIMx_COMInt(TIM1); }
INLINE Bool TIM2_COMInt(void) { return TIMx_COMInt(TIM2); }
INLINE Bool TIM3_COMInt(void) { return TIMx_COMInt(TIM3); }
INLINE Bool TIM4_COMInt(void) { return TIMx_COMInt(TIM4); }
INLINE Bool TIM5_COMInt(void) { return TIMx_COMInt(TIM5); }
INLINE Bool TIM6_COMInt(void) { return TIMx_COMInt(TIM6); }
INLINE Bool TIM7_COMInt(void) { return TIMx_COMInt(TIM7); }
INLINE Bool TIM8_COMInt(void) { return TIMx_COMInt(TIM8); }
INLINE Bool TIM9_COMInt(void) { return TIMx_COMInt(TIM9); }
INLINE Bool TIM10_COMInt(void) { return TIMx_COMInt(TIM10); }

INLINE void TIMx_COMIntClr(TIM_t* tim) { tim->INTFR &= ~B5; }
INLINE void TIM1_COMIntClr(void) { TIMx_COMIntClr(TIM1); }
INLINE void TIM2_COMIntClr(void) { TIMx_COMIntClr(TIM2); }
INLINE void TIM3_COMIntClr(void) { TIMx_COMIntClr(TIM3); }
INLINE void TIM4_COMIntClr(void) { TIMx_COMIntClr(TIM4); }
INLINE void TIM5_COMIntClr(void) { TIMx_COMIntClr(TIM5); }
INLINE void TIM6_COMIntClr(void) { TIMx_COMIntClr(TIM6); }
INLINE void TIM7_COMIntClr(void) { TIMx_COMIntClr(TIM7); }
INLINE void TIM8_COMIntClr(void) { TIMx_COMIntClr(TIM8); }
INLINE void TIM9_COMIntClr(void) { TIMx_COMIntClr(TIM9); }
INLINE void TIM10_COMIntClr(void) { TIMx_COMIntClr(TIM10); }

// Enable/Disable trigger interrupt (default disabled; register DMAINTENR.TIE)
INLINE void TIMx_TRGIntEnable(TIM_t* tim) { tim->DMAINTENR |= B6; }
INLINE void TIM1_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM1); }
INLINE void TIM2_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM2); }
INLINE void TIM3_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM3); }
INLINE void TIM4_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM4); }
INLINE void TIM5_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM5); }
INLINE void TIM6_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM6); }
INLINE void TIM7_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM7); }
INLINE void TIM8_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM8); }
INLINE void TIM9_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM9); }
INLINE void TIM10_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM10); }

INLINE void TIMx_TRGIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B6; }
INLINE void TIM1_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM1); }
INLINE void TIM2_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM2); }
INLINE void TIM3_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM3); }
INLINE void TIM4_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM4); }
INLINE void TIM5_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM5); }
INLINE void TIM6_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM6); }
INLINE void TIM7_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM7); }
INLINE void TIM8_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM8); }
INLINE void TIM9_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM9); }
INLINE void TIM10_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM10); }

// Check/Clear TRG interrupt flag (register INTFR.TIF)
INLINE Bool TIMx_TRGInt(TIM_t* tim) { return (tim->INTFR & B6) != 0; }
INLINE Bool TIM1_TRGInt(void) { return TIMx_TRGInt(TIM1); }
INLINE Bool TIM2_TRGInt(void) { return TIMx_TRGInt(TIM2); }
INLINE Bool TIM3_TRGInt(void) { return TIMx_TRGInt(TIM3); }
INLINE Bool TIM4_TRGInt(void) { return TIMx_TRGInt(TIM4); }
INLINE Bool TIM5_TRGInt(void) { return TIMx_TRGInt(TIM5); }
INLINE Bool TIM6_TRGInt(void) { return TIMx_TRGInt(TIM6); }
INLINE Bool TIM7_TRGInt(void) { return TIMx_TRGInt(TIM7); }
INLINE Bool TIM8_TRGInt(void) { return TIMx_TRGInt(TIM8); }
INLINE Bool TIM9_TRGInt(void) { return TIMx_TRGInt(TIM9); }
INLINE Bool TIM10_TRGInt(void) { return TIMx_TRGInt(TIM10); }

INLINE void TIMx_TRGIntClr(TIM_t* tim) { tim->INTFR &= ~B6; }
INLINE void TIM1_TRGIntClr(void) { TIMx_TRGIntClr(TIM1); }
INLINE void TIM2_TRGIntClr(void) { TIMx_TRGIntClr(TIM2); }
INLINE void TIM3_TRGIntClr(void) { TIMx_TRGIntClr(TIM3); }
INLINE void TIM4_TRGIntClr(void) { TIMx_TRGIntClr(TIM4); }
INLINE void TIM5_TRGIntClr(void) { TIMx_TRGIntClr(TIM5); }
INLINE void TIM6_TRGIntClr(void) { TIMx_TRGIntClr(TIM6); }
INLINE void TIM7_TRGIntClr(void) { TIMx_TRGIntClr(TIM7); }
INLINE void TIM8_TRGIntClr(void) { TIMx_TRGIntClr(TIM8); }
INLINE void TIM9_TRGIntClr(void) { TIMx_TRGIntClr(TIM9); }
INLINE void TIM10_TRGIntClr(void) { TIMx_TRGIntClr(TIM10); }

// Enable/Disable break interrupt (default disabled; register DMAINTENR.BIE)
INLINE void TIMx_BrkIntEnable(TIM_t* tim) { tim->DMAINTENR |= B7; }
INLINE void TIM1_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM1); }
INLINE void TIM2_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM2); }
INLINE void TIM3_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM3); }
INLINE void TIM4_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM4); }
INLINE void TIM5_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM5); }
INLINE void TIM6_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM6); }
INLINE void TIM7_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM7); }
INLINE void TIM8_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM8); }
INLINE void TIM9_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM9); }
INLINE void TIM10_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM10); }

INLINE void TIMx_BrkIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B7; }
INLINE void TIM1_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM1); }
INLINE void TIM2_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM2); }
INLINE void TIM3_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM3); }
INLINE void TIM4_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM4); }
INLINE void TIM5_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM5); }
INLINE void TIM6_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM6); }
INLINE void TIM7_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM7); }
INLINE void TIM8_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM8); }
INLINE void TIM9_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM9); }
INLINE void TIM10_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM10); }

// Check/Clear Brk interrupt flag (register INTFR.BIF)
INLINE Bool TIMx_BrkInt(TIM_t* tim) { return (tim->INTFR & B7) != 0; }
INLINE Bool TIM1_BrkInt(void) { return TIMx_BrkInt(TIM1); }
INLINE Bool TIM2_BrkInt(void) { return TIMx_BrkInt(TIM2); }
INLINE Bool TIM3_BrkInt(void) { return TIMx_BrkInt(TIM3); }
INLINE Bool TIM4_BrkInt(void) { return TIMx_BrkInt(TIM4); }
INLINE Bool TIM5_BrkInt(void) { return TIMx_BrkInt(TIM5); }
INLINE Bool TIM6_BrkInt(void) { return TIMx_BrkInt(TIM6); }
INLINE Bool TIM7_BrkInt(void) { return TIMx_BrkInt(TIM7); }
INLINE Bool TIM8_BrkInt(void) { return TIMx_BrkInt(TIM8); }
INLINE Bool TIM9_BrkInt(void) { return TIMx_BrkInt(TIM9); }
INLINE Bool TIM10_BrkInt(void) { return TIMx_BrkInt(TIM10); }

INLINE void TIMx_BrkIntClr(TIM_t* tim) { tim->INTFR &= ~B7; }
INLINE void TIM1_BrkIntClr(void) { TIMx_BrkIntClr(TIM1); }
INLINE void TIM2_BrkIntClr(void) { TIMx_BrkIntClr(TIM2); }
INLINE void TIM3_BrkIntClr(void) { TIMx_BrkIntClr(TIM3); }
INLINE void TIM4_BrkIntClr(void) { TIMx_BrkIntClr(TIM4); }
INLINE void TIM5_BrkIntClr(void) { TIMx_BrkIntClr(TIM5); }
INLINE void TIM6_BrkIntClr(void) { TIMx_BrkIntClr(TIM6); }
INLINE void TIM7_BrkIntClr(void) { TIMx_BrkIntClr(TIM7); }
INLINE void TIM8_BrkIntClr(void) { TIMx_BrkIntClr(TIM8); }
INLINE void TIM9_BrkIntClr(void) { TIMx_BrkIntClr(TIM9); }
INLINE void TIM10_BrkIntClr(void) { TIMx_BrkIntClr(TIM10); }

// Enable/Disable update DMA request (default disabled; register DMAINTENR.UDE)
INLINE void TIMx_UDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B8; }
INLINE void TIM1_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM1); }
INLINE void TIM2_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM2); }
INLINE void TIM3_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM3); }
INLINE void TIM4_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM4); }
INLINE void TIM5_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM5); }
INLINE void TIM6_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM6); }
INLINE void TIM7_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM7); }
INLINE void TIM8_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM8); }
INLINE void TIM9_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM9); }
INLINE void TIM10_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM10); }

INLINE void TIMx_UDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B8; }
INLINE void TIM1_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM1); }
INLINE void TIM2_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM2); }
INLINE void TIM3_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM3); }
INLINE void TIM4_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM4); }
INLINE void TIM5_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM5); }
INLINE void TIM6_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM6); }
INLINE void TIM7_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM7); }
INLINE void TIM8_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM8); }
INLINE void TIM9_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM9); }
INLINE void TIM10_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM10); }

// Enable/Disable compare channel 1 DMA request (default disabled; register DMAINTENR.CC1DE)
INLINE void TIMx_CC1DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B9; }
INLINE void TIM1_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM1); }
INLINE void TIM2_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM2); }
INLINE void TIM3_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM3); }
INLINE void TIM4_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM4); }
INLINE void TIM5_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM5); }
INLINE void TIM6_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM6); }
INLINE void TIM7_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM7); }
INLINE void TIM8_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM8); }
INLINE void TIM9_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM9); }
INLINE void TIM10_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM10); }

INLINE void TIMx_CC1DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B9; }
INLINE void TIM1_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM1); }
INLINE void TIM2_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM2); }
INLINE void TIM3_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM3); }
INLINE void TIM4_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM4); }
INLINE void TIM5_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM5); }
INLINE void TIM6_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM6); }
INLINE void TIM7_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM7); }
INLINE void TIM8_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM8); }
INLINE void TIM9_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM9); }
INLINE void TIM10_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM10); }

// Enable/Disable compare channel 2 DMA request (default disabled; register DMAINTENR.CC2DE)
INLINE void TIMx_CC2DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B10; }
INLINE void TIM1_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM1); }
INLINE void TIM2_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM2); }
INLINE void TIM3_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM3); }
INLINE void TIM4_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM4); }
INLINE void TIM5_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM5); }
INLINE void TIM6_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM6); }
INLINE void TIM7_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM7); }
INLINE void TIM8_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM8); }
INLINE void TIM9_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM9); }
INLINE void TIM10_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM10); }

INLINE void TIMx_CC2DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B10; }
INLINE void TIM1_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM1); }
INLINE void TIM2_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM2); }
INLINE void TIM3_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM3); }
INLINE void TIM4_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM4); }
INLINE void TIM5_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM5); }
INLINE void TIM6_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM6); }
INLINE void TIM7_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM7); }
INLINE void TIM8_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM8); }
INLINE void TIM9_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM9); }
INLINE void TIM10_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM10); }

// Enable/Disable compare channel 3 DMA request (default disabled; register DMAINTENR.CC3DE)
INLINE void TIMx_CC3DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B11; }
INLINE void TIM1_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM1); }
INLINE void TIM2_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM2); }
INLINE void TIM3_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM3); }
INLINE void TIM4_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM4); }
INLINE void TIM5_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM5); }
INLINE void TIM6_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM6); }
INLINE void TIM7_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM7); }
INLINE void TIM8_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM8); }
INLINE void TIM9_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM9); }
INLINE void TIM10_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM10); }

INLINE void TIMx_CC3DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B11; }
INLINE void TIM1_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM1); }
INLINE void TIM2_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM2); }
INLINE void TIM3_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM3); }
INLINE void TIM4_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM4); }
INLINE void TIM5_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM5); }
INLINE void TIM6_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM6); }
INLINE void TIM7_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM7); }
INLINE void TIM8_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM8); }
INLINE void TIM9_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM9); }
INLINE void TIM10_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM10); }

// Enable/Disable compare channel 4 DMA request (default disabled; register DMAINTENR.CC4DE)
INLINE void TIMx_CC4DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B12; }
INLINE void TIM1_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM1); }
INLINE void TIM2_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM2); }
INLINE void TIM3_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM3); }
INLINE void TIM4_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM4); }
INLINE void TIM5_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM5); }
INLINE void TIM6_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM6); }
INLINE void TIM7_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM7); }
INLINE void TIM8_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM8); }
INLINE void TIM9_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM9); }
INLINE void TIM10_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM10); }

INLINE void TIMx_CC4DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B12; }
INLINE void TIM1_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM1); }
INLINE void TIM2_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM2); }
INLINE void TIM3_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM3); }
INLINE void TIM4_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM4); }
INLINE void TIM5_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM5); }
INLINE void TIM6_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM6); }
INLINE void TIM7_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM7); }
INLINE void TIM8_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM8); }
INLINE void TIM9_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM9); }
INLINE void TIM10_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM10); }

// Enable/Disable COM DMA request (default disabled; register DMAINTENR.COMDE)
INLINE void TIMx_COMDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B13; }
INLINE void TIM1_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM1); }
INLINE void TIM2_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM2); }
INLINE void TIM3_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM3); }
INLINE void TIM4_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM4); }
INLINE void TIM5_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM5); }
INLINE void TIM6_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM6); }
INLINE void TIM7_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM7); }
INLINE void TIM8_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM8); }
INLINE void TIM9_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM9); }
INLINE void TIM10_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM10); }

INLINE void TIMx_COMDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B13; }
INLINE void TIM1_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM1); }
INLINE void TIM2_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM2); }
INLINE void TIM3_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM3); }
INLINE void TIM4_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM4); }
INLINE void TIM5_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM5); }
INLINE void TIM6_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM6); }
INLINE void TIM7_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM7); }
INLINE void TIM8_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM8); }
INLINE void TIM9_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM9); }
INLINE void TIM10_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM10); }

// Enable/Disable trigger DMA request (default disabled; register DMAINTENR.TDE)
INLINE void TIMx_TRGDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B14; }
INLINE void TIM1_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM1); }
INLINE void TIM2_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM2); }
INLINE void TIM3_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM3); }
INLINE void TIM4_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM4); }
INLINE void TIM5_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM5); }
INLINE void TIM6_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM6); }
INLINE void TIM7_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM7); }
INLINE void TIM8_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM8); }
INLINE void TIM9_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM9); }
INLINE void TIM10_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM10); }

INLINE void TIMx_TRGDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B14; }
INLINE void TIM1_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM1); }
INLINE void TIM2_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM2); }
INLINE void TIM3_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM3); }
INLINE void TIM4_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM4); }
INLINE void TIM5_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM5); }
INLINE void TIM6_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM6); }
INLINE void TIM7_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM7); }
INLINE void TIM8_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM8); }
INLINE void TIM9_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM9); }
INLINE void TIM10_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM10); }

// Check/Clear compare capture 1 repeat flag (register INTFR.CC1OF)
INLINE Bool TIMx_CC1Rep(TIM_t* tim) { return (tim->INTFR & B9) != 0; }
INLINE Bool TIM1_CC1Rep(void) { return TIMx_CC1Rep(TIM1); }
INLINE Bool TIM2_CC1Rep(void) { return TIMx_CC1Rep(TIM2); }
INLINE Bool TIM3_CC1Rep(void) { return TIMx_CC1Rep(TIM3); }
INLINE Bool TIM4_CC1Rep(void) { return TIMx_CC1Rep(TIM4); }
INLINE Bool TIM5_CC1Rep(void) { return TIMx_CC1Rep(TIM5); }
INLINE Bool TIM6_CC1Rep(void) { return TIMx_CC1Rep(TIM6); }
INLINE Bool TIM7_CC1Rep(void) { return TIMx_CC1Rep(TIM7); }
INLINE Bool TIM8_CC1Rep(void) { return TIMx_CC1Rep(TIM8); }
INLINE Bool TIM9_CC1Rep(void) { return TIMx_CC1Rep(TIM9); }
INLINE Bool TIM10_CC1Rep(void) { return TIMx_CC1Rep(TIM10); }

INLINE void TIMx_CC1RepClr(TIM_t* tim) { tim->INTFR &= ~B9; }
INLINE void TIM1_CC1RepClr(void) { TIMx_CC1RepClr(TIM1); }
INLINE void TIM2_CC1RepClr(void) { TIMx_CC1RepClr(TIM2); }
INLINE void TIM3_CC1RepClr(void) { TIMx_CC1RepClr(TIM3); }
INLINE void TIM4_CC1RepClr(void) { TIMx_CC1RepClr(TIM4); }
INLINE void TIM5_CC1RepClr(void) { TIMx_CC1RepClr(TIM5); }
INLINE void TIM6_CC1RepClr(void) { TIMx_CC1RepClr(TIM6); }
INLINE void TIM7_CC1RepClr(void) { TIMx_CC1RepClr(TIM7); }
INLINE void TIM8_CC1RepClr(void) { TIMx_CC1RepClr(TIM8); }
INLINE void TIM9_CC1RepClr(void) { TIMx_CC1RepClr(TIM9); }
INLINE void TIM10_CC1RepClr(void) { TIMx_CC1RepClr(TIM10); }

// Check/Clear compare capture 2 repeat flag (register INTFR.CC2OF)
INLINE Bool TIMx_CC2Rep(TIM_t* tim) { return (tim->INTFR & B10) != 0; }
INLINE Bool TIM1_CC2Rep(void) { return TIMx_CC2Rep(TIM1); }
INLINE Bool TIM2_CC2Rep(void) { return TIMx_CC2Rep(TIM2); }
INLINE Bool TIM3_CC2Rep(void) { return TIMx_CC2Rep(TIM3); }
INLINE Bool TIM4_CC2Rep(void) { return TIMx_CC2Rep(TIM4); }
INLINE Bool TIM5_CC2Rep(void) { return TIMx_CC2Rep(TIM5); }
INLINE Bool TIM6_CC2Rep(void) { return TIMx_CC2Rep(TIM6); }
INLINE Bool TIM7_CC2Rep(void) { return TIMx_CC2Rep(TIM7); }
INLINE Bool TIM8_CC2Rep(void) { return TIMx_CC2Rep(TIM8); }
INLINE Bool TIM9_CC2Rep(void) { return TIMx_CC2Rep(TIM9); }
INLINE Bool TIM10_CC2Rep(void) { return TIMx_CC2Rep(TIM10); }

INLINE void TIMx_CC2RepClr(TIM_t* tim) { tim->INTFR &= ~B10; }
INLINE void TIM1_CC2RepClr(void) { TIMx_CC2RepClr(TIM1); }
INLINE void TIM2_CC2RepClr(void) { TIMx_CC2RepClr(TIM2); }
INLINE void TIM3_CC2RepClr(void) { TIMx_CC2RepClr(TIM3); }
INLINE void TIM4_CC2RepClr(void) { TIMx_CC2RepClr(TIM4); }
INLINE void TIM5_CC2RepClr(void) { TIMx_CC2RepClr(TIM5); }
INLINE void TIM6_CC2RepClr(void) { TIMx_CC2RepClr(TIM6); }
INLINE void TIM7_CC2RepClr(void) { TIMx_CC2RepClr(TIM7); }
INLINE void TIM8_CC2RepClr(void) { TIMx_CC2RepClr(TIM8); }
INLINE void TIM9_CC2RepClr(void) { TIMx_CC2RepClr(TIM9); }
INLINE void TIM10_CC2RepClr(void) { TIMx_CC2RepClr(TIM10); }

// Check/Clear compare capture 3 repeat flag (register INTFR.CC3OF)
INLINE Bool TIMx_CC3Rep(TIM_t* tim) { return (tim->INTFR & B11) != 0; }
INLINE Bool TIM1_CC3Rep(void) { return TIMx_CC3Rep(TIM1); }
INLINE Bool TIM2_CC3Rep(void) { return TIMx_CC3Rep(TIM2); }
INLINE Bool TIM3_CC3Rep(void) { return TIMx_CC3Rep(TIM3); }
INLINE Bool TIM4_CC3Rep(void) { return TIMx_CC3Rep(TIM4); }
INLINE Bool TIM5_CC3Rep(void) { return TIMx_CC3Rep(TIM5); }
INLINE Bool TIM6_CC3Rep(void) { return TIMx_CC3Rep(TIM6); }
INLINE Bool TIM7_CC3Rep(void) { return TIMx_CC3Rep(TIM7); }
INLINE Bool TIM8_CC3Rep(void) { return TIMx_CC3Rep(TIM8); }
INLINE Bool TIM9_CC3Rep(void) { return TIMx_CC3Rep(TIM9); }
INLINE Bool TIM10_CC3Rep(void) { return TIMx_CC3Rep(TIM10); }

INLINE void TIMx_CC3RepClr(TIM_t* tim) { tim->INTFR &= ~B11; }
INLINE void TIM1_CC3RepClr(void) { TIMx_CC3RepClr(TIM1); }
INLINE void TIM2_CC3RepClr(void) { TIMx_CC3RepClr(TIM2); }
INLINE void TIM3_CC3RepClr(void) { TIMx_CC3RepClr(TIM3); }
INLINE void TIM4_CC3RepClr(void) { TIMx_CC3RepClr(TIM4); }
INLINE void TIM5_CC3RepClr(void) { TIMx_CC3RepClr(TIM5); }
INLINE void TIM6_CC3RepClr(void) { TIMx_CC3RepClr(TIM6); }
INLINE void TIM7_CC3RepClr(void) { TIMx_CC3RepClr(TIM7); }
INLINE void TIM8_CC3RepClr(void) { TIMx_CC3RepClr(TIM8); }
INLINE void TIM9_CC3RepClr(void) { TIMx_CC3RepClr(TIM9); }
INLINE void TIM10_CC3RepClr(void) { TIMx_CC3RepClr(TIM10); }

// Check/Clear compare capture 4 repeat flag (register INTFR.CC4OF)
INLINE Bool TIMx_CC4Rep(TIM_t* tim) { return (tim->INTFR & B12) != 0; }
INLINE Bool TIM1_CC4Rep(void) { return TIMx_CC4Rep(TIM1); }
INLINE Bool TIM2_CC4Rep(void) { return TIMx_CC4Rep(TIM2); }
INLINE Bool TIM3_CC4Rep(void) { return TIMx_CC4Rep(TIM3); }
INLINE Bool TIM4_CC4Rep(void) { return TIMx_CC4Rep(TIM4); }
INLINE Bool TIM5_CC4Rep(void) { return TIMx_CC4Rep(TIM5); }
INLINE Bool TIM6_CC4Rep(void) { return TIMx_CC4Rep(TIM6); }
INLINE Bool TIM7_CC4Rep(void) { return TIMx_CC4Rep(TIM7); }
INLINE Bool TIM8_CC4Rep(void) { return TIMx_CC4Rep(TIM8); }
INLINE Bool TIM9_CC4Rep(void) { return TIMx_CC4Rep(TIM9); }
INLINE Bool TIM10_CC4Rep(void) { return TIMx_CC4Rep(TIM10); }

INLINE void TIMx_CC4RepClr(TIM_t* tim) { tim->INTFR &= ~B12; }
INLINE void TIM1_CC4RepClr(void) { TIMx_CC4RepClr(TIM1); }
INLINE void TIM2_CC4RepClr(void) { TIMx_CC4RepClr(TIM2); }
INLINE void TIM3_CC4RepClr(void) { TIMx_CC4RepClr(TIM3); }
INLINE void TIM4_CC4RepClr(void) { TIMx_CC4RepClr(TIM4); }
INLINE void TIM5_CC4RepClr(void) { TIMx_CC4RepClr(TIM5); }
INLINE void TIM6_CC4RepClr(void) { TIMx_CC4RepClr(TIM6); }
INLINE void TIM7_CC4RepClr(void) { TIMx_CC4RepClr(TIM7); }
INLINE void TIM8_CC4RepClr(void) { TIMx_CC4RepClr(TIM8); }
INLINE void TIM9_CC4RepClr(void) { TIMx_CC4RepClr(TIM9); }
INLINE void TIM10_CC4RepClr(void) { TIMx_CC4RepClr(TIM10); }

// === Generate event

// Generate update event (register SWEVGR.UG)
INLINE void TIMx_Update(TIM_t* tim) { tim->SWEVGR |= B0; }
INLINE void TIM1_Update(void) { TIMx_Update(TIM1); }
INLINE void TIM2_Update(void) { TIMx_Update(TIM2); }
INLINE void TIM3_Update(void) { TIMx_Update(TIM3); }
INLINE void TIM4_Update(void) { TIMx_Update(TIM4); }
INLINE void TIM5_Update(void) { TIMx_Update(TIM5); }
INLINE void TIM6_Update(void) { TIMx_Update(TIM6); }
INLINE void TIM7_Update(void) { TIMx_Update(TIM7); }
INLINE void TIM8_Update(void) { TIMx_Update(TIM8); }
INLINE void TIM9_Update(void) { TIMx_Update(TIM9); }
INLINE void TIM10_Update(void) { TIMx_Update(TIM10); }

// Generate compare capture 1 event (register SWEVGR.CC1G)
INLINE void TIMx_CC1Event(TIM_t* tim) { tim->SWEVGR |= B1; }
INLINE void TIM1_CC1Event(void) { TIMx_CC1Event(TIM1); }
INLINE void TIM2_CC1Event(void) { TIMx_CC1Event(TIM2); }
INLINE void TIM3_CC1Event(void) { TIMx_CC1Event(TIM3); }
INLINE void TIM4_CC1Event(void) { TIMx_CC1Event(TIM4); }
INLINE void TIM5_CC1Event(void) { TIMx_CC1Event(TIM5); }
INLINE void TIM6_CC1Event(void) { TIMx_CC1Event(TIM6); }
INLINE void TIM7_CC1Event(void) { TIMx_CC1Event(TIM7); }
INLINE void TIM8_CC1Event(void) { TIMx_CC1Event(TIM8); }
INLINE void TIM9_CC1Event(void) { TIMx_CC1Event(TIM9); }
INLINE void TIM10_CC1Event(void) { TIMx_CC1Event(TIM10); }

// Generate compare capture 2 event (register SWEVGR.CC2G)
INLINE void TIMx_CC2Event(TIM_t* tim) { tim->SWEVGR |= B2; }
INLINE void TIM1_CC2Event(void) { TIMx_CC2Event(TIM1); }
INLINE void TIM2_CC2Event(void) { TIMx_CC2Event(TIM2); }
INLINE void TIM3_CC2Event(void) { TIMx_CC2Event(TIM3); }
INLINE void TIM4_CC2Event(void) { TIMx_CC2Event(TIM4); }
INLINE void TIM5_CC2Event(void) { TIMx_CC2Event(TIM5); }
INLINE void TIM6_CC2Event(void) { TIMx_CC2Event(TIM6); }
INLINE void TIM7_CC2Event(void) { TIMx_CC2Event(TIM7); }
INLINE void TIM8_CC2Event(void) { TIMx_CC2Event(TIM8); }
INLINE void TIM9_CC2Event(void) { TIMx_CC2Event(TIM9); }
INLINE void TIM10_CC2Event(void) { TIMx_CC2Event(TIM10); }

// Generate compare capture 3 event (register SWEVGR.CC3G)
INLINE void TIMx_CC3Event(TIM_t* tim) { tim->SWEVGR |= B3; }
INLINE void TIM1_CC3Event(void) { TIMx_CC3Event(TIM1); }
INLINE void TIM2_CC3Event(void) { TIMx_CC3Event(TIM2); }
INLINE void TIM3_CC3Event(void) { TIMx_CC3Event(TIM3); }
INLINE void TIM4_CC3Event(void) { TIMx_CC3Event(TIM4); }
INLINE void TIM5_CC3Event(void) { TIMx_CC3Event(TIM5); }
INLINE void TIM6_CC3Event(void) { TIMx_CC3Event(TIM6); }
INLINE void TIM7_CC3Event(void) { TIMx_CC3Event(TIM7); }
INLINE void TIM8_CC3Event(void) { TIMx_CC3Event(TIM8); }
INLINE void TIM9_CC3Event(void) { TIMx_CC3Event(TIM9); }
INLINE void TIM10_CC3Event(void) { TIMx_CC3Event(TIM10); }

// Generate compare capture 4 event (register SWEVGR.CC4G)
INLINE void TIMx_CC4Event(TIM_t* tim) { tim->SWEVGR |= B4; }
INLINE void TIM1_CC4Event(void) { TIMx_CC4Event(TIM1); }
INLINE void TIM2_CC4Event(void) { TIMx_CC4Event(TIM2); }
INLINE void TIM3_CC4Event(void) { TIMx_CC4Event(TIM3); }
INLINE void TIM4_CC4Event(void) { TIMx_CC4Event(TIM4); }
INLINE void TIM5_CC4Event(void) { TIMx_CC4Event(TIM5); }
INLINE void TIM6_CC4Event(void) { TIMx_CC4Event(TIM6); }
INLINE void TIM7_CC4Event(void) { TIMx_CC4Event(TIM7); }
INLINE void TIM8_CC4Event(void) { TIMx_CC4Event(TIM8); }
INLINE void TIM9_CC4Event(void) { TIMx_CC4Event(TIM9); }
INLINE void TIM10_CC4Event(void) { TIMx_CC4Event(TIM10); }

// Generate COM update event (register SWEVGR.COMG)
INLINE void TIMx_COMEvent(TIM_t* tim) { tim->SWEVGR |= B5; }
INLINE void TIM1_COMEvent(void) { TIMx_COMEvent(TIM1); }
INLINE void TIM2_COMEvent(void) { TIMx_COMEvent(TIM2); }
INLINE void TIM3_COMEvent(void) { TIMx_COMEvent(TIM3); }
INLINE void TIM4_COMEvent(void) { TIMx_COMEvent(TIM4); }
INLINE void TIM5_COMEvent(void) { TIMx_COMEvent(TIM5); }
INLINE void TIM6_COMEvent(void) { TIMx_COMEvent(TIM6); }
INLINE void TIM7_COMEvent(void) { TIMx_COMEvent(TIM7); }
INLINE void TIM8_COMEvent(void) { TIMx_COMEvent(TIM8); }
INLINE void TIM9_COMEvent(void) { TIMx_COMEvent(TIM9); }
INLINE void TIM10_COMEvent(void) { TIMx_COMEvent(TIM10); }

// Generate trigger event (register SWEVGR.TG)
INLINE void TIMx_TRGEvent(TIM_t* tim) { tim->SWEVGR |= B6; }
INLINE void TIM1_TRGEvent(void) { TIMx_TRGEvent(TIM1); }
INLINE void TIM2_TRGEvent(void) { TIMx_TRGEvent(TIM2); }
INLINE void TIM3_TRGEvent(void) { TIMx_TRGEvent(TIM3); }
INLINE void TIM4_TRGEvent(void) { TIMx_TRGEvent(TIM4); }
INLINE void TIM5_TRGEvent(void) { TIMx_TRGEvent(TIM5); }
INLINE void TIM6_TRGEvent(void) { TIMx_TRGEvent(TIM6); }
INLINE void TIM7_TRGEvent(void) { TIMx_TRGEvent(TIM7); }
INLINE void TIM8_TRGEvent(void) { TIMx_TRGEvent(TIM8); }
INLINE void TIM9_TRGEvent(void) { TIMx_TRGEvent(TIM9); }
INLINE void TIM10_TRGEvent(void) { TIMx_TRGEvent(TIM10); }

// Generate break event (register SWEVGR.BG)
INLINE void TIMx_BrkEvent(TIM_t* tim) { tim->SWEVGR |= B7; }
INLINE void TIM1_BrkEvent(void) { TIMx_BrkEvent(TIM1); }
INLINE void TIM2_BrkEvent(void) { TIMx_BrkEvent(TIM2); }
INLINE void TIM3_BrkEvent(void) { TIMx_BrkEvent(TIM3); }
INLINE void TIM4_BrkEvent(void) { TIMx_BrkEvent(TIM4); }
INLINE void TIM5_BrkEvent(void) { TIMx_BrkEvent(TIM5); }
INLINE void TIM6_BrkEvent(void) { TIMx_BrkEvent(TIM6); }
INLINE void TIM7_BrkEvent(void) { TIMx_BrkEvent(TIM7); }
INLINE void TIM8_BrkEvent(void) { TIMx_BrkEvent(TIM8); }
INLINE void TIM9_BrkEvent(void) { TIMx_BrkEvent(TIM9); }
INLINE void TIM10_BrkEvent(void) { TIMx_BrkEvent(TIM10); }

// === Compare and capture setup
// Compare: pin direction it output, capture: pin direction is input

// Select input/output mode of compare capture channel 1 TIM_CCSEL_* (default output; register CHCTLR1.CC1S[2:0])
INLINE void TIMx_CC1Sel(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~3) | mode; }
INLINE void TIM1_CC1Sel(int mode) { TIMx_CC1Sel(TIM1, mode); }
INLINE void TIM2_CC1Sel(int mode) { TIMx_CC1Sel(TIM2, mode); }
INLINE void TIM3_CC1Sel(int mode) { TIMx_CC1Sel(TIM3, mode); }
INLINE void TIM4_CC1Sel(int mode) { TIMx_CC1Sel(TIM4, mode); }
INLINE void TIM5_CC1Sel(int mode) { TIMx_CC1Sel(TIM5, mode); }
INLINE void TIM6_CC1Sel(int mode) { TIMx_CC1Sel(TIM6, mode); }
INLINE void TIM7_CC1Sel(int mode) { TIMx_CC1Sel(TIM7, mode); }
INLINE void TIM8_CC1Sel(int mode) { TIMx_CC1Sel(TIM8, mode); }
INLINE void TIM9_CC1Sel(int mode) { TIMx_CC1Sel(TIM9, mode); }
INLINE void TIM10_CC1Sel(int mode) { TIMx_CC1Sel(TIM10, mode); }

// Compare mode: Enable/Disable fast compare of channel 1 (default disable; register CHCTLR1.OC1FE)
INLINE void TIMx_OC1FastEnable(TIM_t* tim) { tim->CHCTLR1 |= B2; }
INLINE void TIM1_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM1); }
INLINE void TIM2_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM2); }
INLINE void TIM3_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM3); }
INLINE void TIM4_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM4); }
INLINE void TIM5_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM5); }
INLINE void TIM6_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM6); }
INLINE void TIM7_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM7); }
INLINE void TIM8_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM8); }
INLINE void TIM9_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM9); }
INLINE void TIM10_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM10); }

INLINE void TIMx_OC1FastDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B2; }
INLINE void TIM1_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM1); }
INLINE void TIM2_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM2); }
INLINE void TIM3_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM3); }
INLINE void TIM4_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM4); }
INLINE void TIM5_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM5); }
INLINE void TIM6_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM6); }
INLINE void TIM7_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM7); }
INLINE void TIM8_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM8); }
INLINE void TIM9_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM9); }
INLINE void TIM10_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM10); }

// Compare mode: Enable/Disable preload compare register 1 (default disable; register CHCTLR1.OC1PE)
INLINE void TIMx_OC1PreEnable(TIM_t* tim) { tim->CHCTLR1 |= B3; }
INLINE void TIM1_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM1); }
INLINE void TIM2_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM2); }
INLINE void TIM3_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM3); }
INLINE void TIM4_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM4); }
INLINE void TIM5_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM5); }
INLINE void TIM6_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM6); }
INLINE void TIM7_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM7); }
INLINE void TIM8_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM8); }
INLINE void TIM9_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM9); }
INLINE void TIM10_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM10); }

INLINE void TIMx_OC1PreDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B3; }
INLINE void TIM1_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM1); }
INLINE void TIM2_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM2); }
INLINE void TIM3_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM3); }
INLINE void TIM4_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM4); }
INLINE void TIM5_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM5); }
INLINE void TIM6_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM6); }
INLINE void TIM7_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM7); }
INLINE void TIM8_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM8); }
INLINE void TIM9_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM9); }
INLINE void TIM10_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM10); }

// Compare mode: Set compare mode of channel 1 TIM_COMP_* (default freeze; register CHCTLR1.OC1M[2:0])
INLINE void TIMx_OC1Mode(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(7<<4)) | (mode<<4); }
INLINE void TIM1_OC1Mode(int mode) { TIMx_OC1Mode(TIM1, mode); }
INLINE void TIM2_OC1Mode(int mode) { TIMx_OC1Mode(TIM2, mode); }
INLINE void TIM3_OC1Mode(int mode) { TIMx_OC1Mode(TIM3, mode); }
INLINE void TIM4_OC1Mode(int mode) { TIMx_OC1Mode(TIM4, mode); }
INLINE void TIM5_OC1Mode(int mode) { TIMx_OC1Mode(TIM5, mode); }
INLINE void TIM6_OC1Mode(int mode) { TIMx_OC1Mode(TIM6, mode); }
INLINE void TIM7_OC1Mode(int mode) { TIMx_OC1Mode(TIM7, mode); }
INLINE void TIM8_OC1Mode(int mode) { TIMx_OC1Mode(TIM8, mode); }
INLINE void TIM9_OC1Mode(int mode) { TIMx_OC1Mode(TIM9, mode); }
INLINE void TIM10_OC1Mode(int mode) { TIMx_OC1Mode(TIM10, mode); }

// Compare mode: Enable/Disable compare channel 1 clear by ETRF input (default disabled; register CHCTLR1.OC1CE)
INLINE void TIMx_OC1ClrEnable(TIM_t* tim) { tim->CHCTLR1 |= B7; }
INLINE void TIM1_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM1); }
INLINE void TIM2_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM2); }
INLINE void TIM3_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM3); }
INLINE void TIM4_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM4); }
INLINE void TIM5_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM5); }
INLINE void TIM6_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM6); }
INLINE void TIM7_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM7); }
INLINE void TIM8_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM8); }
INLINE void TIM9_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM9); }
INLINE void TIM10_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM10); }

INLINE void TIMx_OC1ClrDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B7; }
INLINE void TIM1_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM1); }
INLINE void TIM2_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM2); }
INLINE void TIM3_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM3); }
INLINE void TIM4_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM4); }
INLINE void TIM5_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM5); }
INLINE void TIM6_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM6); }
INLINE void TIM7_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM7); }
INLINE void TIM8_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM8); }
INLINE void TIM9_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM9); }
INLINE void TIM10_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM10); }

// Capture mode: Set prescaler of capture channel 1 TIM_ETRPDIV_* (default prescaler off; register CHCTLR1.IC1PSC[1:0])
INLINE void TIMx_IC1Div(TIM_t* tim, int div) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<2)) | (div<<2); }
INLINE void TIM1_IC1Div(int div) { TIMx_IC1Div(TIM1, div); }
INLINE void TIM2_IC1Div(int div) { TIMx_IC1Div(TIM2, div); }
INLINE void TIM3_IC1Div(int div) { TIMx_IC1Div(TIM3, div); }
INLINE void TIM4_IC1Div(int div) { TIMx_IC1Div(TIM4, div); }
INLINE void TIM5_IC1Div(int div) { TIMx_IC1Div(TIM5, div); }
INLINE void TIM6_IC1Div(int div) { TIMx_IC1Div(TIM6, div); }
INLINE void TIM7_IC1Div(int div) { TIMx_IC1Div(TIM7, div); }
INLINE void TIM8_IC1Div(int div) { TIMx_IC1Div(TIM8, div); }
INLINE void TIM9_IC1Div(int div) { TIMx_IC1Div(TIM9, div); }
INLINE void TIM10_IC1Div(int div) { TIMx_IC1Div(TIM10, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 1 (record up to N events; default 0; register CHCTLR1.IC1F[3:0])
INLINE void TIMx_IC1Filter(TIM_t* tim, int filt) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(0x0f<<4)) | (filt<<4); }
INLINE void TIM1_IC1Filter(int filt) { TIMx_IC1Filter(TIM1, filt); }
INLINE void TIM2_IC1Filter(int filt) { TIMx_IC1Filter(TIM2, filt); }
INLINE void TIM3_IC1Filter(int filt) { TIMx_IC1Filter(TIM3, filt); }
INLINE void TIM4_IC1Filter(int filt) { TIMx_IC1Filter(TIM4, filt); }
INLINE void TIM5_IC1Filter(int filt) { TIMx_IC1Filter(TIM5, filt); }
INLINE void TIM6_IC1Filter(int filt) { TIMx_IC1Filter(TIM6, filt); }
INLINE void TIM7_IC1Filter(int filt) { TIMx_IC1Filter(TIM7, filt); }
INLINE void TIM8_IC1Filter(int filt) { TIMx_IC1Filter(TIM8, filt); }
INLINE void TIM9_IC1Filter(int filt) { TIMx_IC1Filter(TIM9, filt); }
INLINE void TIM10_IC1Filter(int filt) { TIMx_IC1Filter(TIM10, filt); }

// Select input/output mode of compare capture channel 2 TIM_CCSEL_* (default output; register CHCTLR1.CCS2S[1:0])
INLINE void TIMx_CC2Sel(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<8)) | (mode<<8); }
INLINE void TIM1_CC2Sel(int mode) { TIMx_CC2Sel(TIM1, mode); }
INLINE void TIM2_CC2Sel(int mode) { TIMx_CC2Sel(TIM2, mode); }
INLINE void TIM3_CC2Sel(int mode) { TIMx_CC2Sel(TIM3, mode); }
INLINE void TIM4_CC2Sel(int mode) { TIMx_CC2Sel(TIM4, mode); }
INLINE void TIM5_CC2Sel(int mode) { TIMx_CC2Sel(TIM5, mode); }
INLINE void TIM6_CC2Sel(int mode) { TIMx_CC2Sel(TIM6, mode); }
INLINE void TIM7_CC2Sel(int mode) { TIMx_CC2Sel(TIM7, mode); }
INLINE void TIM8_CC2Sel(int mode) { TIMx_CC2Sel(TIM8, mode); }
INLINE void TIM9_CC2Sel(int mode) { TIMx_CC2Sel(TIM9, mode); }
INLINE void TIM10_CC2Sel(int mode) { TIMx_CC2Sel(TIM10, mode); }

// Compare mode: Enable/Disable fast compare of channel 2 (default disable; register CHCTLR1.OC2FE)
INLINE void TIMx_OC2FastEnable(TIM_t* tim) { tim->CHCTLR1 |= B10; }
INLINE void TIM1_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM1); }
INLINE void TIM2_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM2); }
INLINE void TIM3_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM3); }
INLINE void TIM4_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM4); }
INLINE void TIM5_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM5); }
INLINE void TIM6_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM6); }
INLINE void TIM7_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM7); }
INLINE void TIM8_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM8); }
INLINE void TIM9_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM9); }
INLINE void TIM10_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM10); }

INLINE void TIMx_OC2FastDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B10; }
INLINE void TIM1_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM1); }
INLINE void TIM2_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM2); }
INLINE void TIM3_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM3); }
INLINE void TIM4_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM4); }
INLINE void TIM5_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM5); }
INLINE void TIM6_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM6); }
INLINE void TIM7_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM7); }
INLINE void TIM8_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM8); }
INLINE void TIM9_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM9); }
INLINE void TIM10_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM10); }

// Compare mode: Enable/Disable preload compare register 2 (default disable; register CHCTLR1.OC2PE)
INLINE void TIMx_OC2PreEnable(TIM_t* tim) { tim->CHCTLR1 |= B11; }
INLINE void TIM1_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM1); }
INLINE void TIM2_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM2); }
INLINE void TIM3_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM3); }
INLINE void TIM4_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM4); }
INLINE void TIM5_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM5); }
INLINE void TIM6_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM6); }
INLINE void TIM7_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM7); }
INLINE void TIM8_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM8); }
INLINE void TIM9_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM9); }
INLINE void TIM10_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM10); }

INLINE void TIMx_OC2PreDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B11; }
INLINE void TIM1_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM1); }
INLINE void TIM2_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM2); }
INLINE void TIM3_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM3); }
INLINE void TIM4_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM4); }
INLINE void TIM5_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM5); }
INLINE void TIM6_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM6); }
INLINE void TIM7_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM7); }
INLINE void TIM8_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM8); }
INLINE void TIM9_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM9); }
INLINE void TIM10_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM10); }

// Compare mode: Set compare mode of channel 2 TIM_COMP_* (default freeze; register CHCTLR1.OC2M[2:0])
INLINE void TIMx_OC2Mode(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(7<<12)) | (mode<<12); }
INLINE void TIM1_OC2Mode(int mode) { TIMx_OC2Mode(TIM1, mode); }
INLINE void TIM2_OC2Mode(int mode) { TIMx_OC2Mode(TIM2, mode); }
INLINE void TIM3_OC2Mode(int mode) { TIMx_OC2Mode(TIM3, mode); }
INLINE void TIM4_OC2Mode(int mode) { TIMx_OC2Mode(TIM4, mode); }
INLINE void TIM5_OC2Mode(int mode) { TIMx_OC2Mode(TIM5, mode); }
INLINE void TIM6_OC2Mode(int mode) { TIMx_OC2Mode(TIM6, mode); }
INLINE void TIM7_OC2Mode(int mode) { TIMx_OC2Mode(TIM7, mode); }
INLINE void TIM8_OC2Mode(int mode) { TIMx_OC2Mode(TIM8, mode); }
INLINE void TIM9_OC2Mode(int mode) { TIMx_OC2Mode(TIM9, mode); }
INLINE void TIM10_OC2Mode(int mode) { TIMx_OC2Mode(TIM10, mode); }

// Compare mode: Enable/Disable compare channel 2 clear by ETRF input (default disabled; register CHCTLR1.OC2CE)
INLINE void TIMx_OC2ClrEnable(TIM_t* tim) { tim->CHCTLR1 |= B15; }
INLINE void TIM1_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM1); }
INLINE void TIM2_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM2); }
INLINE void TIM3_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM3); }
INLINE void TIM4_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM4); }
INLINE void TIM5_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM5); }
INLINE void TIM6_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM6); }
INLINE void TIM7_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM7); }
INLINE void TIM8_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM8); }
INLINE void TIM9_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM9); }
INLINE void TIM10_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM10); }

INLINE void TIMx_OC2ClrDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B15; }
INLINE void TIM1_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM1); }
INLINE void TIM2_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM2); }
INLINE void TIM3_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM3); }
INLINE void TIM4_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM4); }
INLINE void TIM5_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM5); }
INLINE void TIM6_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM6); }
INLINE void TIM7_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM7); }
INLINE void TIM8_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM8); }
INLINE void TIM9_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM9); }
INLINE void TIM10_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM10); }

// Capture mode: Set prescaler of capture channel 2 TIM_ETRPDIV_* (default prescaler off; register CHCTLR1.IC2PSC[1:0])
INLINE void TIMx_IC2Div(TIM_t* tim, int div) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<10)) | (div<<10); }
INLINE void TIM1_IC2Div(int div) { TIMx_IC2Div(TIM1, div); }
INLINE void TIM2_IC2Div(int div) { TIMx_IC2Div(TIM2, div); }
INLINE void TIM3_IC2Div(int div) { TIMx_IC2Div(TIM3, div); }
INLINE void TIM4_IC2Div(int div) { TIMx_IC2Div(TIM4, div); }
INLINE void TIM5_IC2Div(int div) { TIMx_IC2Div(TIM5, div); }
INLINE void TIM6_IC2Div(int div) { TIMx_IC2Div(TIM6, div); }
INLINE void TIM7_IC2Div(int div) { TIMx_IC2Div(TIM7, div); }
INLINE void TIM8_IC2Div(int div) { TIMx_IC2Div(TIM8, div); }
INLINE void TIM9_IC2Div(int div) { TIMx_IC2Div(TIM9, div); }
INLINE void TIM10_IC2Div(int div) { TIMx_IC2Div(TIM10, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 2 (record up to N events; default 0; register CHCTLR1.IC2F[3:0])
INLINE void TIMx_IC2Filter(TIM_t* tim, int filt) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(0x0f<<12)) | (filt<<12); }
INLINE void TIM1_IC2Filter(int filt) { TIMx_IC2Filter(TIM1, filt); }
INLINE void TIM2_IC2Filter(int filt) { TIMx_IC2Filter(TIM2, filt); }
INLINE void TIM3_IC2Filter(int filt) { TIMx_IC2Filter(TIM3, filt); }
INLINE void TIM4_IC2Filter(int filt) { TIMx_IC2Filter(TIM4, filt); }
INLINE void TIM5_IC2Filter(int filt) { TIMx_IC2Filter(TIM5, filt); }
INLINE void TIM6_IC2Filter(int filt) { TIMx_IC2Filter(TIM6, filt); }
INLINE void TIM7_IC2Filter(int filt) { TIMx_IC2Filter(TIM7, filt); }
INLINE void TIM8_IC2Filter(int filt) { TIMx_IC2Filter(TIM8, filt); }
INLINE void TIM9_IC2Filter(int filt) { TIMx_IC2Filter(TIM9, filt); }
INLINE void TIM10_IC2Filter(int filt) { TIMx_IC2Filter(TIM10, filt); }

// Select input/output mode of compare capture channel 3 TIM_CCSEL_* (default output; register CHCTLR2.CC3S[2:0])
INLINE void TIMx_CC3Sel(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~3) | mode; }
INLINE void TIM1_CC3Sel(int mode) { TIMx_CC3Sel(TIM1, mode); }
INLINE void TIM2_CC3Sel(int mode) { TIMx_CC3Sel(TIM2, mode); }
INLINE void TIM3_CC3Sel(int mode) { TIMx_CC3Sel(TIM3, mode); }
INLINE void TIM4_CC3Sel(int mode) { TIMx_CC3Sel(TIM4, mode); }
INLINE void TIM5_CC3Sel(int mode) { TIMx_CC3Sel(TIM5, mode); }
INLINE void TIM6_CC3Sel(int mode) { TIMx_CC3Sel(TIM6, mode); }
INLINE void TIM7_CC3Sel(int mode) { TIMx_CC3Sel(TIM7, mode); }
INLINE void TIM8_CC3Sel(int mode) { TIMx_CC3Sel(TIM8, mode); }
INLINE void TIM9_CC3Sel(int mode) { TIMx_CC3Sel(TIM9, mode); }
INLINE void TIM10_CC3Sel(int mode) { TIMx_CC3Sel(TIM10, mode); }

// Compare mode: Enable/Disable fast compare of channel 3 (default disable; register CHCTLR2.OC3FE)
INLINE void TIMx_OC3FastEnable(TIM_t* tim) { tim->CHCTLR2 |= B2; }
INLINE void TIM1_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM1); }
INLINE void TIM2_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM2); }
INLINE void TIM3_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM3); }
INLINE void TIM4_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM4); }
INLINE void TIM5_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM5); }
INLINE void TIM6_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM6); }
INLINE void TIM7_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM7); }
INLINE void TIM8_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM8); }
INLINE void TIM9_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM9); }
INLINE void TIM10_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM10); }

INLINE void TIMx_OC3FastDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B2; }
INLINE void TIM1_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM1); }
INLINE void TIM2_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM2); }
INLINE void TIM3_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM3); }
INLINE void TIM4_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM4); }
INLINE void TIM5_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM5); }
INLINE void TIM6_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM6); }
INLINE void TIM7_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM7); }
INLINE void TIM8_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM8); }
INLINE void TIM9_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM9); }
INLINE void TIM10_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM10); }

// Compare mode: Enable/Disable preload compare register 3 (default disable; register CHCTLR2.OC3PE)
INLINE void TIMx_OC3PreEnable(TIM_t* tim) { tim->CHCTLR2 |= B3; }
INLINE void TIM1_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM1); }
INLINE void TIM2_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM2); }
INLINE void TIM3_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM3); }
INLINE void TIM4_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM4); }
INLINE void TIM5_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM5); }
INLINE void TIM6_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM6); }
INLINE void TIM7_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM7); }
INLINE void TIM8_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM8); }
INLINE void TIM9_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM9); }
INLINE void TIM10_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM10); }

INLINE void TIMx_OC3PreDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B3; }
INLINE void TIM1_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM1); }
INLINE void TIM2_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM2); }
INLINE void TIM3_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM3); }
INLINE void TIM4_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM4); }
INLINE void TIM5_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM5); }
INLINE void TIM6_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM6); }
INLINE void TIM7_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM7); }
INLINE void TIM8_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM8); }
INLINE void TIM9_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM9); }
INLINE void TIM10_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM10); }

// Compare mode: Set compare mode of channel 3 TIM_COMP_* (default freeze; register CHCTLR2.OC3M[2:0])
INLINE void TIMx_OC3Mode(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(7<<4)) | (mode<<4); }
INLINE void TIM1_OC3Mode(int mode) { TIMx_OC3Mode(TIM1, mode); }
INLINE void TIM2_OC3Mode(int mode) { TIMx_OC3Mode(TIM2, mode); }
INLINE void TIM3_OC3Mode(int mode) { TIMx_OC3Mode(TIM3, mode); }
INLINE void TIM4_OC3Mode(int mode) { TIMx_OC3Mode(TIM4, mode); }
INLINE void TIM5_OC3Mode(int mode) { TIMx_OC3Mode(TIM5, mode); }
INLINE void TIM6_OC3Mode(int mode) { TIMx_OC3Mode(TIM6, mode); }
INLINE void TIM7_OC3Mode(int mode) { TIMx_OC3Mode(TIM7, mode); }
INLINE void TIM8_OC3Mode(int mode) { TIMx_OC3Mode(TIM8, mode); }
INLINE void TIM9_OC3Mode(int mode) { TIMx_OC3Mode(TIM9, mode); }
INLINE void TIM10_OC3Mode(int mode) { TIMx_OC3Mode(TIM10, mode); }

// Compare mode: Enable/Disable compare channel 3 clear by ETRF input (default disabled; register CHCTLR2.OC3CE)
INLINE void TIMx_OC3ClrEnable(TIM_t* tim) { tim->CHCTLR2 |= B7; }
INLINE void TIM1_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM1); }
INLINE void TIM2_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM2); }
INLINE void TIM3_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM3); }
INLINE void TIM4_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM4); }
INLINE void TIM5_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM5); }
INLINE void TIM6_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM6); }
INLINE void TIM7_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM7); }
INLINE void TIM8_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM8); }
INLINE void TIM9_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM9); }
INLINE void TIM10_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM10); }

INLINE void TIMx_OC3ClrDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B7; }
INLINE void TIM1_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM1); }
INLINE void TIM2_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM2); }
INLINE void TIM3_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM3); }
INLINE void TIM4_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM4); }
INLINE void TIM5_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM5); }
INLINE void TIM6_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM6); }
INLINE void TIM7_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM7); }
INLINE void TIM8_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM8); }
INLINE void TIM9_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM9); }
INLINE void TIM10_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM10); }

// Capture mode: Set prescaler of capture channel 3 TIM_ETRPDIV_* (default prescaler off; register CHCTLR2.IC3PSC[1:0])
INLINE void TIMx_IC3Div(TIM_t* tim, int div) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<2)) | (div<<2); }
INLINE void TIM1_IC3Div(int div) { TIMx_IC3Div(TIM1, div); }
INLINE void TIM2_IC3Div(int div) { TIMx_IC3Div(TIM2, div); }
INLINE void TIM3_IC3Div(int div) { TIMx_IC3Div(TIM3, div); }
INLINE void TIM4_IC3Div(int div) { TIMx_IC3Div(TIM4, div); }
INLINE void TIM5_IC3Div(int div) { TIMx_IC3Div(TIM5, div); }
INLINE void TIM6_IC3Div(int div) { TIMx_IC3Div(TIM6, div); }
INLINE void TIM7_IC3Div(int div) { TIMx_IC3Div(TIM7, div); }
INLINE void TIM8_IC3Div(int div) { TIMx_IC3Div(TIM8, div); }
INLINE void TIM9_IC3Div(int div) { TIMx_IC3Div(TIM9, div); }
INLINE void TIM10_IC3Div(int div) { TIMx_IC3Div(TIM10, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 3 (record up to N events; default 0; register CHCTLR2.IC3F[3:0])
INLINE void TIMx_IC3Filter(TIM_t* tim, int filt) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(0x0f<<4)) | (filt<<4); }
INLINE void TIM1_IC3Filter(int filt) { TIMx_IC3Filter(TIM1, filt); }
INLINE void TIM2_IC3Filter(int filt) { TIMx_IC3Filter(TIM2, filt); }
INLINE void TIM3_IC3Filter(int filt) { TIMx_IC3Filter(TIM3, filt); }
INLINE void TIM4_IC3Filter(int filt) { TIMx_IC3Filter(TIM4, filt); }
INLINE void TIM5_IC3Filter(int filt) { TIMx_IC3Filter(TIM5, filt); }
INLINE void TIM6_IC3Filter(int filt) { TIMx_IC3Filter(TIM6, filt); }
INLINE void TIM7_IC3Filter(int filt) { TIMx_IC3Filter(TIM7, filt); }
INLINE void TIM8_IC3Filter(int filt) { TIMx_IC3Filter(TIM8, filt); }
INLINE void TIM9_IC3Filter(int filt) { TIMx_IC3Filter(TIM9, filt); }
INLINE void TIM10_IC3Filter(int filt) { TIMx_IC3Filter(TIM10, filt); }

// Select input/output mode of compare capture channel 4 TIM_CCSEL_* (default output; register CHCTLR2.CCS4S[1:0])
INLINE void TIMx_CC4Sel(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<8)) | (mode<<8); }
INLINE void TIM1_CC4Sel(int mode) { TIMx_CC4Sel(TIM1, mode); }
INLINE void TIM2_CC4Sel(int mode) { TIMx_CC4Sel(TIM2, mode); }
INLINE void TIM3_CC4Sel(int mode) { TIMx_CC4Sel(TIM3, mode); }
INLINE void TIM4_CC4Sel(int mode) { TIMx_CC4Sel(TIM4, mode); }
INLINE void TIM5_CC4Sel(int mode) { TIMx_CC4Sel(TIM5, mode); }
INLINE void TIM6_CC4Sel(int mode) { TIMx_CC4Sel(TIM6, mode); }
INLINE void TIM7_CC4Sel(int mode) { TIMx_CC4Sel(TIM7, mode); }
INLINE void TIM8_CC4Sel(int mode) { TIMx_CC4Sel(TIM8, mode); }
INLINE void TIM9_CC4Sel(int mode) { TIMx_CC4Sel(TIM9, mode); }
INLINE void TIM10_CC4Sel(int mode) { TIMx_CC4Sel(TIM10, mode); }

// Compare mode: Enable/Disable fast compare of channel 4 (default disable; register CHCTLR2.OC4FE)
INLINE void TIMx_OC4FastEnable(TIM_t* tim) { tim->CHCTLR2 |= B10; }
INLINE void TIM1_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM1); }
INLINE void TIM2_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM2); }
INLINE void TIM3_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM3); }
INLINE void TIM4_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM4); }
INLINE void TIM5_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM5); }
INLINE void TIM6_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM6); }
INLINE void TIM7_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM7); }
INLINE void TIM8_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM8); }
INLINE void TIM9_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM9); }
INLINE void TIM10_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM10); }

INLINE void TIMx_OC4FastDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B10; }
INLINE void TIM1_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM1); }
INLINE void TIM2_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM2); }
INLINE void TIM3_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM3); }
INLINE void TIM4_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM4); }
INLINE void TIM5_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM5); }
INLINE void TIM6_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM6); }
INLINE void TIM7_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM7); }
INLINE void TIM8_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM8); }
INLINE void TIM9_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM9); }
INLINE void TIM10_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM10); }

// Compare mode: Enable/Disable preload compare register 4 (default disable; register CHCTLR2.OC4PE)
INLINE void TIMx_OC4PreEnable(TIM_t* tim) { tim->CHCTLR2 |= B11; }
INLINE void TIM1_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM1); }
INLINE void TIM2_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM2); }
INLINE void TIM3_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM3); }
INLINE void TIM4_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM4); }
INLINE void TIM5_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM5); }
INLINE void TIM6_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM6); }
INLINE void TIM7_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM7); }
INLINE void TIM8_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM8); }
INLINE void TIM9_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM9); }
INLINE void TIM10_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM10); }

INLINE void TIMx_OC4PreDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B11; }
INLINE void TIM1_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM1); }
INLINE void TIM2_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM2); }
INLINE void TIM3_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM3); }
INLINE void TIM4_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM4); }
INLINE void TIM5_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM5); }
INLINE void TIM6_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM6); }
INLINE void TIM7_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM7); }
INLINE void TIM8_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM8); }
INLINE void TIM9_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM9); }
INLINE void TIM10_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM10); }

// Compare mode: Set compare mode of channel 4 TIM_COMP_* (default freeze; register CHCTLR2.OC4M[2:0])
INLINE void TIMx_OC4Mode(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(7<<12)) | (mode<<12); }
INLINE void TIM1_OC4Mode(int mode) { TIMx_OC4Mode(TIM1, mode); }
INLINE void TIM2_OC4Mode(int mode) { TIMx_OC4Mode(TIM2, mode); }
INLINE void TIM3_OC4Mode(int mode) { TIMx_OC4Mode(TIM3, mode); }
INLINE void TIM4_OC4Mode(int mode) { TIMx_OC4Mode(TIM4, mode); }
INLINE void TIM5_OC4Mode(int mode) { TIMx_OC4Mode(TIM5, mode); }
INLINE void TIM6_OC4Mode(int mode) { TIMx_OC4Mode(TIM6, mode); }
INLINE void TIM7_OC4Mode(int mode) { TIMx_OC4Mode(TIM7, mode); }
INLINE void TIM8_OC4Mode(int mode) { TIMx_OC4Mode(TIM8, mode); }
INLINE void TIM9_OC4Mode(int mode) { TIMx_OC4Mode(TIM9, mode); }
INLINE void TIM10_OC4Mode(int mode) { TIMx_OC4Mode(TIM10, mode); }

// Compare mode: Enable/Disable compare channel 4 clear by ETRF input (default disabled; register CHCTLR2.OC4CE)
INLINE void TIMx_OC4ClrEnable(TIM_t* tim) { tim->CHCTLR2 |= B15; }
INLINE void TIM1_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM1); }
INLINE void TIM2_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM2); }
INLINE void TIM3_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM3); }
INLINE void TIM4_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM4); }
INLINE void TIM5_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM5); }
INLINE void TIM6_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM6); }
INLINE void TIM7_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM7); }
INLINE void TIM8_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM8); }
INLINE void TIM9_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM9); }
INLINE void TIM10_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM10); }

INLINE void TIMx_OC4ClrDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B15; }
INLINE void TIM1_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM1); }
INLINE void TIM2_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM2); }
INLINE void TIM3_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM3); }
INLINE void TIM4_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM4); }
INLINE void TIM5_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM5); }
INLINE void TIM6_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM6); }
INLINE void TIM7_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM7); }
INLINE void TIM8_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM8); }
INLINE void TIM9_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM9); }
INLINE void TIM10_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM10); }

// Capture mode: Set prescaler of capture channel 4 TIM_ETRPDIV_* (default prescaler off; register CHCTLR2.IC4PSC[1:0])
INLINE void TIMx_IC4Div(TIM_t* tim, int div) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<10)) | (div<<10); }
INLINE void TIM1_IC4Div(int div) { TIMx_IC4Div(TIM1, div); }
INLINE void TIM2_IC4Div(int div) { TIMx_IC4Div(TIM2, div); }
INLINE void TIM3_IC4Div(int div) { TIMx_IC4Div(TIM3, div); }
INLINE void TIM4_IC4Div(int div) { TIMx_IC4Div(TIM4, div); }
INLINE void TIM5_IC4Div(int div) { TIMx_IC4Div(TIM5, div); }
INLINE void TIM6_IC4Div(int div) { TIMx_IC4Div(TIM6, div); }
INLINE void TIM7_IC4Div(int div) { TIMx_IC4Div(TIM7, div); }
INLINE void TIM8_IC4Div(int div) { TIMx_IC4Div(TIM8, div); }
INLINE void TIM9_IC4Div(int div) { TIMx_IC4Div(TIM9, div); }
INLINE void TIM10_IC4Div(int div) { TIMx_IC4Div(TIM10, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 4 (record up to N events; default 0; default 0; register CHCTLR2.IC4F[3:0])
INLINE void TIMx_IC4Filter(TIM_t* tim, int filt) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(0x0f<<12)) | (filt<<12); }
INLINE void TIM1_IC4Filter(int filt) { TIMx_IC4Filter(TIM1, filt); }
INLINE void TIM2_IC4Filter(int filt) { TIMx_IC4Filter(TIM2, filt); }
INLINE void TIM3_IC4Filter(int filt) { TIMx_IC4Filter(TIM3, filt); }
INLINE void TIM4_IC4Filter(int filt) { TIMx_IC4Filter(TIM4, filt); }
INLINE void TIM5_IC4Filter(int filt) { TIMx_IC4Filter(TIM5, filt); }
INLINE void TIM6_IC4Filter(int filt) { TIMx_IC4Filter(TIM6, filt); }
INLINE void TIM7_IC4Filter(int filt) { TIMx_IC4Filter(TIM7, filt); }
INLINE void TIM8_IC4Filter(int filt) { TIMx_IC4Filter(TIM8, filt); }
INLINE void TIM9_IC4Filter(int filt) { TIMx_IC4Filter(TIM9, filt); }
INLINE void TIM10_IC4Filter(int filt) { TIMx_IC4Filter(TIM10, filt); }

// === Compare capture enable

// Enable/Disable compare capture channel 1 (default disable; register CCER.CC1E)
INLINE void TIMx_CC1Enable(TIM_t* tim) { tim->CCER |= B0; }
INLINE void TIM1_CC1Enable(void) { TIMx_CC1Enable(TIM1); }
INLINE void TIM2_CC1Enable(void) { TIMx_CC1Enable(TIM2); }
INLINE void TIM3_CC1Enable(void) { TIMx_CC1Enable(TIM3); }
INLINE void TIM4_CC1Enable(void) { TIMx_CC1Enable(TIM4); }
INLINE void TIM5_CC1Enable(void) { TIMx_CC1Enable(TIM5); }
INLINE void TIM6_CC1Enable(void) { TIMx_CC1Enable(TIM6); }
INLINE void TIM7_CC1Enable(void) { TIMx_CC1Enable(TIM7); }
INLINE void TIM8_CC1Enable(void) { TIMx_CC1Enable(TIM8); }
INLINE void TIM9_CC1Enable(void) { TIMx_CC1Enable(TIM9); }
INLINE void TIM10_CC1Enable(void) { TIMx_CC1Enable(TIM10); }

INLINE void TIMx_CC1Disable(TIM_t* tim) { tim->CCER &= ~B0; }
INLINE void TIM1_CC1Disable(void) { TIMx_CC1Disable(TIM1); }
INLINE void TIM2_CC1Disable(void) { TIMx_CC1Disable(TIM2); }
INLINE void TIM3_CC1Disable(void) { TIMx_CC1Disable(TIM3); }
INLINE void TIM4_CC1Disable(void) { TIMx_CC1Disable(TIM4); }
INLINE void TIM5_CC1Disable(void) { TIMx_CC1Disable(TIM5); }
INLINE void TIM6_CC1Disable(void) { TIMx_CC1Disable(TIM6); }
INLINE void TIM7_CC1Disable(void) { TIMx_CC1Disable(TIM7); }
INLINE void TIM8_CC1Disable(void) { TIMx_CC1Disable(TIM8); }
INLINE void TIM9_CC1Disable(void) { TIMx_CC1Disable(TIM9); }
INLINE void TIM10_CC1Disable(void) { TIMx_CC1Disable(TIM10); }

// Compare mode: Set compare channel 1 output polarity to active LOW or HIGH (default high; register CCER.CC1P)
INLINE void TIMx_OC1Low(TIM_t* tim) { tim->CCER |= B1; }
INLINE void TIM1_OC1Low(void) { TIMx_OC1Low(TIM1); }
INLINE void TIM2_OC1Low(void) { TIMx_OC1Low(TIM2); }
INLINE void TIM3_OC1Low(void) { TIMx_OC1Low(TIM3); }
INLINE void TIM4_OC1Low(void) { TIMx_OC1Low(TIM4); }
INLINE void TIM5_OC1Low(void) { TIMx_OC1Low(TIM5); }
INLINE void TIM6_OC1Low(void) { TIMx_OC1Low(TIM6); }
INLINE void TIM7_OC1Low(void) { TIMx_OC1Low(TIM7); }
INLINE void TIM8_OC1Low(void) { TIMx_OC1Low(TIM8); }
INLINE void TIM9_OC1Low(void) { TIMx_OC1Low(TIM9); }
INLINE void TIM10_OC1Low(void) { TIMx_OC1Low(TIM10); }

INLINE void TIMx_OC1High(TIM_t* tim) { tim->CCER &= ~B1; }
INLINE void TIM1_OC1High(void) { TIMx_OC1High(TIM1); }
INLINE void TIM2_OC1High(void) { TIMx_OC1High(TIM2); }
INLINE void TIM3_OC1High(void) { TIMx_OC1High(TIM3); }
INLINE void TIM4_OC1High(void) { TIMx_OC1High(TIM4); }
INLINE void TIM5_OC1High(void) { TIMx_OC1High(TIM5); }
INLINE void TIM6_OC1High(void) { TIMx_OC1High(TIM6); }
INLINE void TIM7_OC1High(void) { TIMx_OC1High(TIM7); }
INLINE void TIM8_OC1High(void) { TIMx_OC1High(TIM8); }
INLINE void TIM9_OC1High(void) { TIMx_OC1High(TIM9); }
INLINE void TIM10_OC1High(void) { TIMx_OC1High(TIM10); }

// Capture mode: Set capture channel 1 input polarity to falling or rising edge (default rising; register CCER.CC1P)
INLINE void TIMx_IC1Fall(TIM_t* tim) { tim->CCER |= B1; }
INLINE void TIM1_IC1Fall(void) { TIMx_IC1Fall(TIM1); }
INLINE void TIM2_IC1Fall(void) { TIMx_IC1Fall(TIM2); }
INLINE void TIM3_IC1Fall(void) { TIMx_IC1Fall(TIM3); }
INLINE void TIM4_IC1Fall(void) { TIMx_IC1Fall(TIM4); }
INLINE void TIM5_IC1Fall(void) { TIMx_IC1Fall(TIM5); }
INLINE void TIM6_IC1Fall(void) { TIMx_IC1Fall(TIM6); }
INLINE void TIM7_IC1Fall(void) { TIMx_IC1Fall(TIM7); }
INLINE void TIM8_IC1Fall(void) { TIMx_IC1Fall(TIM8); }
INLINE void TIM9_IC1Fall(void) { TIMx_IC1Fall(TIM9); }
INLINE void TIM10_IC1Fall(void) { TIMx_IC1Fall(TIM10); }

INLINE void TIMx_IC1Rise(TIM_t* tim) { tim->CCER &= ~B1; }
INLINE void TIM1_IC1Rise(void) { TIMx_IC1Rise(TIM1); }
INLINE void TIM2_IC1Rise(void) { TIMx_IC1Rise(TIM2); }
INLINE void TIM3_IC1Rise(void) { TIMx_IC1Rise(TIM3); }
INLINE void TIM4_IC1Rise(void) { TIMx_IC1Rise(TIM4); }
INLINE void TIM5_IC1Rise(void) { TIMx_IC1Rise(TIM5); }
INLINE void TIM6_IC1Rise(void) { TIMx_IC1Rise(TIM6); }
INLINE void TIM7_IC1Rise(void) { TIMx_IC1Rise(TIM7); }
INLINE void TIM8_IC1Rise(void) { TIMx_IC1Rise(TIM8); }
INLINE void TIM9_IC1Rise(void) { TIMx_IC1Rise(TIM9); }
INLINE void TIM10_IC1Rise(void) { TIMx_IC1Rise(TIM10); }

// Enable/Disable compare capture channel 1 complementary (default disable; register CCER.CC1NE)
INLINE void TIMx_CC1NEnable(TIM_t* tim) { tim->CCER |= B2; }
INLINE void TIM1_CC1NEnable(void) { TIMx_CC1NEnable(TIM1); }
INLINE void TIM2_CC1NEnable(void) { TIMx_CC1NEnable(TIM2); }
INLINE void TIM3_CC1NEnable(void) { TIMx_CC1NEnable(TIM3); }
INLINE void TIM4_CC1NEnable(void) { TIMx_CC1NEnable(TIM4); }
INLINE void TIM5_CC1NEnable(void) { TIMx_CC1NEnable(TIM5); }
INLINE void TIM6_CC1NEnable(void) { TIMx_CC1NEnable(TIM6); }
INLINE void TIM7_CC1NEnable(void) { TIMx_CC1NEnable(TIM7); }
INLINE void TIM8_CC1NEnable(void) { TIMx_CC1NEnable(TIM8); }
INLINE void TIM9_CC1NEnable(void) { TIMx_CC1NEnable(TIM9); }
INLINE void TIM10_CC1NEnable(void) { TIMx_CC1NEnable(TIM10); }

INLINE void TIMx_CC1NDisable(TIM_t* tim) { tim->CCER &= ~B2; }
INLINE void TIM1_CC1NDisable(void) { TIMx_CC1NDisable(TIM1); }
INLINE void TIM2_CC1NDisable(void) { TIMx_CC1NDisable(TIM2); }
INLINE void TIM3_CC1NDisable(void) { TIMx_CC1NDisable(TIM3); }
INLINE void TIM4_CC1NDisable(void) { TIMx_CC1NDisable(TIM4); }
INLINE void TIM5_CC1NDisable(void) { TIMx_CC1NDisable(TIM5); }
INLINE void TIM6_CC1NDisable(void) { TIMx_CC1NDisable(TIM6); }
INLINE void TIM7_CC1NDisable(void) { TIMx_CC1NDisable(TIM7); }
INLINE void TIM8_CC1NDisable(void) { TIMx_CC1NDisable(TIM8); }
INLINE void TIM9_CC1NDisable(void) { TIMx_CC1NDisable(TIM9); }
INLINE void TIM10_CC1NDisable(void) { TIMx_CC1NDisable(TIM10); }

// Compare mode: Set compare channel 1 complementary output polarity to active LOW or HIGH (default high; register CCER.CC1NP)
INLINE void TIMx_OC1NLow(TIM_t* tim) { tim->CCER |= B3; }
INLINE void TIM1_OC1NLow(void) { TIMx_OC1NLow(TIM1); }
INLINE void TIM2_OC1NLow(void) { TIMx_OC1NLow(TIM2); }
INLINE void TIM3_OC1NLow(void) { TIMx_OC1NLow(TIM3); }
INLINE void TIM4_OC1NLow(void) { TIMx_OC1NLow(TIM4); }
INLINE void TIM5_OC1NLow(void) { TIMx_OC1NLow(TIM5); }
INLINE void TIM6_OC1NLow(void) { TIMx_OC1NLow(TIM6); }
INLINE void TIM7_OC1NLow(void) { TIMx_OC1NLow(TIM7); }
INLINE void TIM8_OC1NLow(void) { TIMx_OC1NLow(TIM8); }
INLINE void TIM9_OC1NLow(void) { TIMx_OC1NLow(TIM9); }
INLINE void TIM10_OC1NLow(void) { TIMx_OC1NLow(TIM10); }

INLINE void TIMx_OC1NHigh(TIM_t* tim) { tim->CCER &= ~B3; }
INLINE void TIM1_OC1NHigh(void) { TIMx_OC1NHigh(TIM1); }
INLINE void TIM2_OC1NHigh(void) { TIMx_OC1NHigh(TIM2); }
INLINE void TIM3_OC1NHigh(void) { TIMx_OC1NHigh(TIM3); }
INLINE void TIM4_OC1NHigh(void) { TIMx_OC1NHigh(TIM4); }
INLINE void TIM5_OC1NHigh(void) { TIMx_OC1NHigh(TIM5); }
INLINE void TIM6_OC1NHigh(void) { TIMx_OC1NHigh(TIM6); }
INLINE void TIM7_OC1NHigh(void) { TIMx_OC1NHigh(TIM7); }
INLINE void TIM8_OC1NHigh(void) { TIMx_OC1NHigh(TIM8); }
INLINE void TIM9_OC1NHigh(void) { TIMx_OC1NHigh(TIM9); }
INLINE void TIM10_OC1NHigh(void) { TIMx_OC1NHigh(TIM10); }

// Capture mode: Set capture channel 1 complementary input polarity to falling or rising edge (default rising; register CCER.CC1NP)
INLINE void TIMx_IC1NFall(TIM_t* tim) { tim->CCER |= B3; }
INLINE void TIM1_IC1NFall(void) { TIMx_IC1NFall(TIM1); }
INLINE void TIM2_IC1NFall(void) { TIMx_IC1NFall(TIM2); }
INLINE void TIM3_IC1NFall(void) { TIMx_IC1NFall(TIM3); }
INLINE void TIM4_IC1NFall(void) { TIMx_IC1NFall(TIM4); }
INLINE void TIM5_IC1NFall(void) { TIMx_IC1NFall(TIM5); }
INLINE void TIM6_IC1NFall(void) { TIMx_IC1NFall(TIM6); }
INLINE void TIM7_IC1NFall(void) { TIMx_IC1NFall(TIM7); }
INLINE void TIM8_IC1NFall(void) { TIMx_IC1NFall(TIM8); }
INLINE void TIM9_IC1NFall(void) { TIMx_IC1NFall(TIM9); }
INLINE void TIM10_IC1NFall(void) { TIMx_IC1NFall(TIM10); }

INLINE void TIMx_IC1NRise(TIM_t* tim) { tim->CCER &= ~B3; }
INLINE void TIM1_IC1NRise(void) { TIMx_IC1NRise(TIM1); }
INLINE void TIM2_IC1NRise(void) { TIMx_IC1NRise(TIM2); }
INLINE void TIM3_IC1NRise(void) { TIMx_IC1NRise(TIM3); }
INLINE void TIM4_IC1NRise(void) { TIMx_IC1NRise(TIM4); }
INLINE void TIM5_IC1NRise(void) { TIMx_IC1NRise(TIM5); }
INLINE void TIM6_IC1NRise(void) { TIMx_IC1NRise(TIM6); }
INLINE void TIM7_IC1NRise(void) { TIMx_IC1NRise(TIM7); }
INLINE void TIM8_IC1NRise(void) { TIMx_IC1NRise(TIM8); }
INLINE void TIM9_IC1NRise(void) { TIMx_IC1NRise(TIM9); }
INLINE void TIM10_IC1NRise(void) { TIMx_IC1NRise(TIM10); }

// Enable/Disable compare capture channel 2 (default disable; register CCER.CC2E)
INLINE void TIMx_CC2Enable(TIM_t* tim) { tim->CCER |= B4; }
INLINE void TIM1_CC2Enable(void) { TIMx_CC2Enable(TIM1); }
INLINE void TIM2_CC2Enable(void) { TIMx_CC2Enable(TIM2); }
INLINE void TIM3_CC2Enable(void) { TIMx_CC2Enable(TIM3); }
INLINE void TIM4_CC2Enable(void) { TIMx_CC2Enable(TIM4); }
INLINE void TIM5_CC2Enable(void) { TIMx_CC2Enable(TIM5); }
INLINE void TIM6_CC2Enable(void) { TIMx_CC2Enable(TIM6); }
INLINE void TIM7_CC2Enable(void) { TIMx_CC2Enable(TIM7); }
INLINE void TIM8_CC2Enable(void) { TIMx_CC2Enable(TIM8); }
INLINE void TIM9_CC2Enable(void) { TIMx_CC2Enable(TIM9); }
INLINE void TIM10_CC2Enable(void) { TIMx_CC2Enable(TIM10); }

INLINE void TIMx_CC2Disable(TIM_t* tim) { tim->CCER &= ~B4; }
INLINE void TIM1_CC2Disable(void) { TIMx_CC2Disable(TIM1); }
INLINE void TIM2_CC2Disable(void) { TIMx_CC2Disable(TIM2); }
INLINE void TIM3_CC2Disable(void) { TIMx_CC2Disable(TIM3); }
INLINE void TIM4_CC2Disable(void) { TIMx_CC2Disable(TIM4); }
INLINE void TIM5_CC2Disable(void) { TIMx_CC2Disable(TIM5); }
INLINE void TIM6_CC2Disable(void) { TIMx_CC2Disable(TIM6); }
INLINE void TIM7_CC2Disable(void) { TIMx_CC2Disable(TIM7); }
INLINE void TIM8_CC2Disable(void) { TIMx_CC2Disable(TIM8); }
INLINE void TIM9_CC2Disable(void) { TIMx_CC2Disable(TIM9); }
INLINE void TIM10_CC2Disable(void) { TIMx_CC2Disable(TIM10); }

// Compare mode: Set compare channel 2 output polarity to active LOW or HIGH (default high; register CCER.CC2P)
INLINE void TIMx_OC2Low(TIM_t* tim) { tim->CCER |= B5; }
INLINE void TIM1_OC2Low(void) { TIMx_OC2Low(TIM1); }
INLINE void TIM2_OC2Low(void) { TIMx_OC2Low(TIM2); }
INLINE void TIM3_OC2Low(void) { TIMx_OC2Low(TIM3); }
INLINE void TIM4_OC2Low(void) { TIMx_OC2Low(TIM4); }
INLINE void TIM5_OC2Low(void) { TIMx_OC2Low(TIM5); }
INLINE void TIM6_OC2Low(void) { TIMx_OC2Low(TIM6); }
INLINE void TIM7_OC2Low(void) { TIMx_OC2Low(TIM7); }
INLINE void TIM8_OC2Low(void) { TIMx_OC2Low(TIM8); }
INLINE void TIM9_OC2Low(void) { TIMx_OC2Low(TIM9); }
INLINE void TIM10_OC2Low(void) { TIMx_OC2Low(TIM10); }

INLINE void TIMx_OC2High(TIM_t* tim) { tim->CCER &= ~B5; }
INLINE void TIM1_OC2High(void) { TIMx_OC2High(TIM1); }
INLINE void TIM2_OC2High(void) { TIMx_OC2High(TIM2); }
INLINE void TIM3_OC2High(void) { TIMx_OC2High(TIM3); }
INLINE void TIM4_OC2High(void) { TIMx_OC2High(TIM4); }
INLINE void TIM5_OC2High(void) { TIMx_OC2High(TIM5); }
INLINE void TIM6_OC2High(void) { TIMx_OC2High(TIM6); }
INLINE void TIM7_OC2High(void) { TIMx_OC2High(TIM7); }
INLINE void TIM8_OC2High(void) { TIMx_OC2High(TIM8); }
INLINE void TIM9_OC2High(void) { TIMx_OC2High(TIM9); }
INLINE void TIM10_OC2High(void) { TIMx_OC2High(TIM10); }

// Capture mode: Set capture channel 2 input polarity to falling or rising edge (default rising; register CCER.CC2P)
INLINE void TIMx_IC2Fall(TIM_t* tim) { tim->CCER |= B5; }
INLINE void TIM1_IC2Fall(void) { TIMx_IC2Fall(TIM1); }
INLINE void TIM2_IC2Fall(void) { TIMx_IC2Fall(TIM2); }
INLINE void TIM3_IC2Fall(void) { TIMx_IC2Fall(TIM3); }
INLINE void TIM4_IC2Fall(void) { TIMx_IC2Fall(TIM4); }
INLINE void TIM5_IC2Fall(void) { TIMx_IC2Fall(TIM5); }
INLINE void TIM6_IC2Fall(void) { TIMx_IC2Fall(TIM6); }
INLINE void TIM7_IC2Fall(void) { TIMx_IC2Fall(TIM7); }
INLINE void TIM8_IC2Fall(void) { TIMx_IC2Fall(TIM8); }
INLINE void TIM9_IC2Fall(void) { TIMx_IC2Fall(TIM9); }
INLINE void TIM10_IC2Fall(void) { TIMx_IC2Fall(TIM10); }

INLINE void TIMx_IC2Rise(TIM_t* tim) { tim->CCER &= ~B5; }
INLINE void TIM1_IC2Rise(void) { TIMx_IC2Rise(TIM1); }
INLINE void TIM2_IC2Rise(void) { TIMx_IC2Rise(TIM2); }
INLINE void TIM3_IC2Rise(void) { TIMx_IC2Rise(TIM3); }
INLINE void TIM4_IC2Rise(void) { TIMx_IC2Rise(TIM4); }
INLINE void TIM5_IC2Rise(void) { TIMx_IC2Rise(TIM5); }
INLINE void TIM6_IC2Rise(void) { TIMx_IC2Rise(TIM6); }
INLINE void TIM7_IC2Rise(void) { TIMx_IC2Rise(TIM7); }
INLINE void TIM8_IC2Rise(void) { TIMx_IC2Rise(TIM8); }
INLINE void TIM9_IC2Rise(void) { TIMx_IC2Rise(TIM9); }
INLINE void TIM10_IC2Rise(void) { TIMx_IC2Rise(TIM10); }

// Enable/Disable compare capture channel 2 complementary (default disable; register CCER.CC2NE)
INLINE void TIMx_CC2NEnable(TIM_t* tim) { tim->CCER |= B6; }
INLINE void TIM1_CC2NEnable(void) { TIMx_CC2NEnable(TIM1); }
INLINE void TIM2_CC2NEnable(void) { TIMx_CC2NEnable(TIM2); }
INLINE void TIM3_CC2NEnable(void) { TIMx_CC2NEnable(TIM3); }
INLINE void TIM4_CC2NEnable(void) { TIMx_CC2NEnable(TIM4); }
INLINE void TIM5_CC2NEnable(void) { TIMx_CC2NEnable(TIM5); }
INLINE void TIM6_CC2NEnable(void) { TIMx_CC2NEnable(TIM6); }
INLINE void TIM7_CC2NEnable(void) { TIMx_CC2NEnable(TIM7); }
INLINE void TIM8_CC2NEnable(void) { TIMx_CC2NEnable(TIM8); }
INLINE void TIM9_CC2NEnable(void) { TIMx_CC2NEnable(TIM9); }
INLINE void TIM10_CC2NEnable(void) { TIMx_CC2NEnable(TIM10); }

INLINE void TIMx_CC2NDisable(TIM_t* tim) { tim->CCER &= ~B6; }
INLINE void TIM1_CC2NDisable(void) { TIMx_CC2NDisable(TIM1); }
INLINE void TIM2_CC2NDisable(void) { TIMx_CC2NDisable(TIM2); }
INLINE void TIM3_CC2NDisable(void) { TIMx_CC2NDisable(TIM3); }
INLINE void TIM4_CC2NDisable(void) { TIMx_CC2NDisable(TIM4); }
INLINE void TIM5_CC2NDisable(void) { TIMx_CC2NDisable(TIM5); }
INLINE void TIM6_CC2NDisable(void) { TIMx_CC2NDisable(TIM6); }
INLINE void TIM7_CC2NDisable(void) { TIMx_CC2NDisable(TIM7); }
INLINE void TIM8_CC2NDisable(void) { TIMx_CC2NDisable(TIM8); }
INLINE void TIM9_CC2NDisable(void) { TIMx_CC2NDisable(TIM9); }
INLINE void TIM10_CC2NDisable(void) { TIMx_CC2NDisable(TIM10); }

// Compare mode: Set compare channel 2 complementary output polarity to active LOW or HIGH (default high; register CCER.CC2NP)
INLINE void TIMx_OC2NLow(TIM_t* tim) { tim->CCER |= B7; }
INLINE void TIM1_OC2NLow(void) { TIMx_OC2NLow(TIM1); }
INLINE void TIM2_OC2NLow(void) { TIMx_OC2NLow(TIM2); }
INLINE void TIM3_OC2NLow(void) { TIMx_OC2NLow(TIM3); }
INLINE void TIM4_OC2NLow(void) { TIMx_OC2NLow(TIM4); }
INLINE void TIM5_OC2NLow(void) { TIMx_OC2NLow(TIM5); }
INLINE void TIM6_OC2NLow(void) { TIMx_OC2NLow(TIM6); }
INLINE void TIM7_OC2NLow(void) { TIMx_OC2NLow(TIM7); }
INLINE void TIM8_OC2NLow(void) { TIMx_OC2NLow(TIM8); }
INLINE void TIM9_OC2NLow(void) { TIMx_OC2NLow(TIM9); }
INLINE void TIM10_OC2NLow(void) { TIMx_OC2NLow(TIM10); }

INLINE void TIMx_OC2NHigh(TIM_t* tim) { tim->CCER &= ~B7; }
INLINE void TIM1_OC2NHigh(void) { TIMx_OC2NHigh(TIM1); }
INLINE void TIM2_OC2NHigh(void) { TIMx_OC2NHigh(TIM2); }
INLINE void TIM3_OC2NHigh(void) { TIMx_OC2NHigh(TIM3); }
INLINE void TIM4_OC2NHigh(void) { TIMx_OC2NHigh(TIM4); }
INLINE void TIM5_OC2NHigh(void) { TIMx_OC2NHigh(TIM5); }
INLINE void TIM6_OC2NHigh(void) { TIMx_OC2NHigh(TIM6); }
INLINE void TIM7_OC2NHigh(void) { TIMx_OC2NHigh(TIM7); }
INLINE void TIM8_OC2NHigh(void) { TIMx_OC2NHigh(TIM8); }
INLINE void TIM9_OC2NHigh(void) { TIMx_OC2NHigh(TIM9); }
INLINE void TIM10_OC2NHigh(void) { TIMx_OC2NHigh(TIM10); }

// Capture mode: Set capture channel 2 complementary input polarity to falling or rising edge (default rising; register CCER.CC2NP)
INLINE void TIMx_IC2NFall(TIM_t* tim) { tim->CCER |= B7; }
INLINE void TIM1_IC2NFall(void) { TIMx_IC2NFall(TIM1); }
INLINE void TIM2_IC2NFall(void) { TIMx_IC2NFall(TIM2); }
INLINE void TIM3_IC2NFall(void) { TIMx_IC2NFall(TIM3); }
INLINE void TIM4_IC2NFall(void) { TIMx_IC2NFall(TIM4); }
INLINE void TIM5_IC2NFall(void) { TIMx_IC2NFall(TIM5); }
INLINE void TIM6_IC2NFall(void) { TIMx_IC2NFall(TIM6); }
INLINE void TIM7_IC2NFall(void) { TIMx_IC2NFall(TIM7); }
INLINE void TIM8_IC2NFall(void) { TIMx_IC2NFall(TIM8); }
INLINE void TIM9_IC2NFall(void) { TIMx_IC2NFall(TIM9); }
INLINE void TIM10_IC2NFall(void) { TIMx_IC2NFall(TIM10); }

INLINE void TIMx_IC2NRise(TIM_t* tim) { tim->CCER &= ~B7; }
INLINE void TIM1_IC2NRise(void) { TIMx_IC2NRise(TIM1); }
INLINE void TIM2_IC2NRise(void) { TIMx_IC2NRise(TIM2); }
INLINE void TIM3_IC2NRise(void) { TIMx_IC2NRise(TIM3); }
INLINE void TIM4_IC2NRise(void) { TIMx_IC2NRise(TIM4); }
INLINE void TIM5_IC2NRise(void) { TIMx_IC2NRise(TIM5); }
INLINE void TIM6_IC2NRise(void) { TIMx_IC2NRise(TIM6); }
INLINE void TIM7_IC2NRise(void) { TIMx_IC2NRise(TIM7); }
INLINE void TIM8_IC2NRise(void) { TIMx_IC2NRise(TIM8); }
INLINE void TIM9_IC2NRise(void) { TIMx_IC2NRise(TIM9); }
INLINE void TIM10_IC2NRise(void) { TIMx_IC2NRise(TIM10); }

// Enable/Disable compare capture channel 3 (default disable; register CCER.CC3E)
INLINE void TIMx_CC3Enable(TIM_t* tim) { tim->CCER |= B8; }
INLINE void TIM1_CC3Enable(void) { TIMx_CC3Enable(TIM1); }
INLINE void TIM2_CC3Enable(void) { TIMx_CC3Enable(TIM2); }
INLINE void TIM3_CC3Enable(void) { TIMx_CC3Enable(TIM3); }
INLINE void TIM4_CC3Enable(void) { TIMx_CC3Enable(TIM4); }
INLINE void TIM5_CC3Enable(void) { TIMx_CC3Enable(TIM5); }
INLINE void TIM6_CC3Enable(void) { TIMx_CC3Enable(TIM6); }
INLINE void TIM7_CC3Enable(void) { TIMx_CC3Enable(TIM7); }
INLINE void TIM8_CC3Enable(void) { TIMx_CC3Enable(TIM8); }
INLINE void TIM9_CC3Enable(void) { TIMx_CC3Enable(TIM9); }
INLINE void TIM10_CC3Enable(void) { TIMx_CC3Enable(TIM10); }

INLINE void TIMx_CC3Disable(TIM_t* tim) { tim->CCER &= ~B8; }
INLINE void TIM1_CC3Disable(void) { TIMx_CC3Disable(TIM1); }
INLINE void TIM2_CC3Disable(void) { TIMx_CC3Disable(TIM2); }
INLINE void TIM3_CC3Disable(void) { TIMx_CC3Disable(TIM3); }
INLINE void TIM4_CC3Disable(void) { TIMx_CC3Disable(TIM4); }
INLINE void TIM5_CC3Disable(void) { TIMx_CC3Disable(TIM5); }
INLINE void TIM6_CC3Disable(void) { TIMx_CC3Disable(TIM6); }
INLINE void TIM7_CC3Disable(void) { TIMx_CC3Disable(TIM7); }
INLINE void TIM8_CC3Disable(void) { TIMx_CC3Disable(TIM8); }
INLINE void TIM9_CC3Disable(void) { TIMx_CC3Disable(TIM9); }
INLINE void TIM10_CC3Disable(void) { TIMx_CC3Disable(TIM10); }

// Compare mode: Set compare channel 3 output polarity to active LOW or HIGH (default high; register CCER.CC3P)
INLINE void TIMx_OC3Low(TIM_t* tim) { tim->CCER |= B9; }
INLINE void TIM1_OC3Low(void) { TIMx_OC3Low(TIM1); }
INLINE void TIM2_OC3Low(void) { TIMx_OC3Low(TIM2); }
INLINE void TIM3_OC3Low(void) { TIMx_OC3Low(TIM3); }
INLINE void TIM4_OC3Low(void) { TIMx_OC3Low(TIM4); }
INLINE void TIM5_OC3Low(void) { TIMx_OC3Low(TIM5); }
INLINE void TIM6_OC3Low(void) { TIMx_OC3Low(TIM6); }
INLINE void TIM7_OC3Low(void) { TIMx_OC3Low(TIM7); }
INLINE void TIM8_OC3Low(void) { TIMx_OC3Low(TIM8); }
INLINE void TIM9_OC3Low(void) { TIMx_OC3Low(TIM9); }
INLINE void TIM10_OC3Low(void) { TIMx_OC3Low(TIM10); }

INLINE void TIMx_OC3High(TIM_t* tim) { tim->CCER &= ~B9; }
INLINE void TIM1_OC3High(void) { TIMx_OC3High(TIM1); }
INLINE void TIM2_OC3High(void) { TIMx_OC3High(TIM2); }
INLINE void TIM3_OC3High(void) { TIMx_OC3High(TIM3); }
INLINE void TIM4_OC3High(void) { TIMx_OC3High(TIM4); }
INLINE void TIM5_OC3High(void) { TIMx_OC3High(TIM5); }
INLINE void TIM6_OC3High(void) { TIMx_OC3High(TIM6); }
INLINE void TIM7_OC3High(void) { TIMx_OC3High(TIM7); }
INLINE void TIM8_OC3High(void) { TIMx_OC3High(TIM8); }
INLINE void TIM9_OC3High(void) { TIMx_OC3High(TIM9); }
INLINE void TIM10_OC3High(void) { TIMx_OC3High(TIM10); }

// Capture mode: Set capture channel 3 input polarity to falling or rising edge (default rising; register CCER.CC3P)
INLINE void TIMx_IC3Fall(TIM_t* tim) { tim->CCER |= B9; }
INLINE void TIM1_IC3Fall(void) { TIMx_IC3Fall(TIM1); }
INLINE void TIM2_IC3Fall(void) { TIMx_IC3Fall(TIM2); }
INLINE void TIM3_IC3Fall(void) { TIMx_IC3Fall(TIM3); }
INLINE void TIM4_IC3Fall(void) { TIMx_IC3Fall(TIM4); }
INLINE void TIM5_IC3Fall(void) { TIMx_IC3Fall(TIM5); }
INLINE void TIM6_IC3Fall(void) { TIMx_IC3Fall(TIM6); }
INLINE void TIM7_IC3Fall(void) { TIMx_IC3Fall(TIM7); }
INLINE void TIM8_IC3Fall(void) { TIMx_IC3Fall(TIM8); }
INLINE void TIM9_IC3Fall(void) { TIMx_IC3Fall(TIM9); }
INLINE void TIM10_IC3Fall(void) { TIMx_IC3Fall(TIM10); }

INLINE void TIMx_IC3Rise(TIM_t* tim) { tim->CCER &= ~B9; }
INLINE void TIM1_IC3Rise(void) { TIMx_IC3Rise(TIM1); }
INLINE void TIM2_IC3Rise(void) { TIMx_IC3Rise(TIM2); }
INLINE void TIM3_IC3Rise(void) { TIMx_IC3Rise(TIM3); }
INLINE void TIM4_IC3Rise(void) { TIMx_IC3Rise(TIM4); }
INLINE void TIM5_IC3Rise(void) { TIMx_IC3Rise(TIM5); }
INLINE void TIM6_IC3Rise(void) { TIMx_IC3Rise(TIM6); }
INLINE void TIM7_IC3Rise(void) { TIMx_IC3Rise(TIM7); }
INLINE void TIM8_IC3Rise(void) { TIMx_IC3Rise(TIM8); }
INLINE void TIM9_IC3Rise(void) { TIMx_IC3Rise(TIM9); }
INLINE void TIM10_IC3Rise(void) { TIMx_IC3Rise(TIM10); }

// Enable/Disable compare capture channel 3 complementary (default disable; register CCER.CC3NE)
INLINE void TIMx_CC3NEnable(TIM_t* tim) { tim->CCER |= B10; }
INLINE void TIM1_CC3NEnable(void) { TIMx_CC3NEnable(TIM1); }
INLINE void TIM2_CC3NEnable(void) { TIMx_CC3NEnable(TIM2); }
INLINE void TIM3_CC3NEnable(void) { TIMx_CC3NEnable(TIM3); }
INLINE void TIM4_CC3NEnable(void) { TIMx_CC3NEnable(TIM4); }
INLINE void TIM5_CC3NEnable(void) { TIMx_CC3NEnable(TIM5); }
INLINE void TIM6_CC3NEnable(void) { TIMx_CC3NEnable(TIM6); }
INLINE void TIM7_CC3NEnable(void) { TIMx_CC3NEnable(TIM7); }
INLINE void TIM8_CC3NEnable(void) { TIMx_CC3NEnable(TIM8); }
INLINE void TIM9_CC3NEnable(void) { TIMx_CC3NEnable(TIM9); }
INLINE void TIM10_CC3NEnable(void) { TIMx_CC3NEnable(TIM10); }

INLINE void TIMx_CC3NDisable(TIM_t* tim) { tim->CCER &= ~B10; }
INLINE void TIM1_CC3NDisable(void) { TIMx_CC3NDisable(TIM1); }
INLINE void TIM2_CC3NDisable(void) { TIMx_CC3NDisable(TIM2); }
INLINE void TIM3_CC3NDisable(void) { TIMx_CC3NDisable(TIM3); }
INLINE void TIM4_CC3NDisable(void) { TIMx_CC3NDisable(TIM4); }
INLINE void TIM5_CC3NDisable(void) { TIMx_CC3NDisable(TIM5); }
INLINE void TIM6_CC3NDisable(void) { TIMx_CC3NDisable(TIM6); }
INLINE void TIM7_CC3NDisable(void) { TIMx_CC3NDisable(TIM7); }
INLINE void TIM8_CC3NDisable(void) { TIMx_CC3NDisable(TIM8); }
INLINE void TIM9_CC3NDisable(void) { TIMx_CC3NDisable(TIM9); }
INLINE void TIM10_CC3NDisable(void) { TIMx_CC3NDisable(TIM10); }

// Compare mode: Set compare channel 3 complementary output polarity to active LOW or HIGH (default high; register CCER.CC3NP)
INLINE void TIMx_OC3NLow(TIM_t* tim) { tim->CCER |= B11; }
INLINE void TIM1_OC3NLow(void) { TIMx_OC3NLow(TIM1); }
INLINE void TIM2_OC3NLow(void) { TIMx_OC3NLow(TIM2); }
INLINE void TIM3_OC3NLow(void) { TIMx_OC3NLow(TIM3); }
INLINE void TIM4_OC3NLow(void) { TIMx_OC3NLow(TIM4); }
INLINE void TIM5_OC3NLow(void) { TIMx_OC3NLow(TIM5); }
INLINE void TIM6_OC3NLow(void) { TIMx_OC3NLow(TIM6); }
INLINE void TIM7_OC3NLow(void) { TIMx_OC3NLow(TIM7); }
INLINE void TIM8_OC3NLow(void) { TIMx_OC3NLow(TIM8); }
INLINE void TIM9_OC3NLow(void) { TIMx_OC3NLow(TIM9); }
INLINE void TIM10_OC3NLow(void) { TIMx_OC3NLow(TIM10); }

INLINE void TIMx_OC3NHigh(TIM_t* tim) { tim->CCER &= ~B11; }
INLINE void TIM1_OC3NHigh(void) { TIMx_OC3NHigh(TIM1); }
INLINE void TIM2_OC3NHigh(void) { TIMx_OC3NHigh(TIM2); }
INLINE void TIM3_OC3NHigh(void) { TIMx_OC3NHigh(TIM3); }
INLINE void TIM4_OC3NHigh(void) { TIMx_OC3NHigh(TIM4); }
INLINE void TIM5_OC3NHigh(void) { TIMx_OC3NHigh(TIM5); }
INLINE void TIM6_OC3NHigh(void) { TIMx_OC3NHigh(TIM6); }
INLINE void TIM7_OC3NHigh(void) { TIMx_OC3NHigh(TIM7); }
INLINE void TIM8_OC3NHigh(void) { TIMx_OC3NHigh(TIM8); }
INLINE void TIM9_OC3NHigh(void) { TIMx_OC3NHigh(TIM9); }
INLINE void TIM10_OC3NHigh(void) { TIMx_OC3NHigh(TIM10); }

// Capture mode: Set capture channel 3 complementary input polarity to falling or rising edge (default rising; register CCER.CC2NP)
INLINE void TIMx_IC3NFall(TIM_t* tim) { tim->CCER |= B11; }
INLINE void TIM1_IC3NFall(void) { TIMx_IC3NFall(TIM1); }
INLINE void TIM2_IC3NFall(void) { TIMx_IC3NFall(TIM2); }
INLINE void TIM3_IC3NFall(void) { TIMx_IC3NFall(TIM3); }
INLINE void TIM4_IC3NFall(void) { TIMx_IC3NFall(TIM4); }
INLINE void TIM5_IC3NFall(void) { TIMx_IC3NFall(TIM5); }
INLINE void TIM6_IC3NFall(void) { TIMx_IC3NFall(TIM6); }
INLINE void TIM7_IC3NFall(void) { TIMx_IC3NFall(TIM7); }
INLINE void TIM8_IC3NFall(void) { TIMx_IC3NFall(TIM8); }
INLINE void TIM9_IC3NFall(void) { TIMx_IC3NFall(TIM9); }
INLINE void TIM10_IC3NFall(void) { TIMx_IC3NFall(TIM10); }

INLINE void TIMx_IC3NRise(TIM_t* tim) { tim->CCER &= ~B11; }
INLINE void TIM1_IC3NRise(void) { TIMx_IC3NRise(TIM1); }
INLINE void TIM2_IC3NRise(void) { TIMx_IC3NRise(TIM2); }
INLINE void TIM3_IC3NRise(void) { TIMx_IC3NRise(TIM3); }
INLINE void TIM4_IC3NRise(void) { TIMx_IC3NRise(TIM4); }
INLINE void TIM5_IC3NRise(void) { TIMx_IC3NRise(TIM5); }
INLINE void TIM6_IC3NRise(void) { TIMx_IC3NRise(TIM6); }
INLINE void TIM7_IC3NRise(void) { TIMx_IC3NRise(TIM7); }
INLINE void TIM8_IC3NRise(void) { TIMx_IC3NRise(TIM8); }
INLINE void TIM9_IC3NRise(void) { TIMx_IC3NRise(TIM9); }
INLINE void TIM10_IC3NRise(void) { TIMx_IC3NRise(TIM10); }

// Enable/Disable compare capture channel 4 (default disable; register CCER.CC4E)
INLINE void TIMx_CC4Enable(TIM_t* tim) { tim->CCER |= B12; }
INLINE void TIM1_CC4Enable(void) { TIMx_CC4Enable(TIM1); }
INLINE void TIM2_CC4Enable(void) { TIMx_CC4Enable(TIM2); }
INLINE void TIM3_CC4Enable(void) { TIMx_CC4Enable(TIM3); }
INLINE void TIM4_CC4Enable(void) { TIMx_CC4Enable(TIM4); }
INLINE void TIM5_CC4Enable(void) { TIMx_CC4Enable(TIM5); }
INLINE void TIM6_CC4Enable(void) { TIMx_CC4Enable(TIM6); }
INLINE void TIM7_CC4Enable(void) { TIMx_CC4Enable(TIM7); }
INLINE void TIM8_CC4Enable(void) { TIMx_CC4Enable(TIM8); }
INLINE void TIM9_CC4Enable(void) { TIMx_CC4Enable(TIM9); }
INLINE void TIM10_CC4Enable(void) { TIMx_CC4Enable(TIM10); }

INLINE void TIMx_CC4Disable(TIM_t* tim) { tim->CCER &= ~B12; }
INLINE void TIM1_CC4Disable(void) { TIMx_CC4Disable(TIM1); }
INLINE void TIM2_CC4Disable(void) { TIMx_CC4Disable(TIM2); }
INLINE void TIM3_CC4Disable(void) { TIMx_CC4Disable(TIM3); }
INLINE void TIM4_CC4Disable(void) { TIMx_CC4Disable(TIM4); }
INLINE void TIM5_CC4Disable(void) { TIMx_CC4Disable(TIM5); }
INLINE void TIM6_CC4Disable(void) { TIMx_CC4Disable(TIM6); }
INLINE void TIM7_CC4Disable(void) { TIMx_CC4Disable(TIM7); }
INLINE void TIM8_CC4Disable(void) { TIMx_CC4Disable(TIM8); }
INLINE void TIM9_CC4Disable(void) { TIMx_CC4Disable(TIM9); }
INLINE void TIM10_CC4Disable(void) { TIMx_CC4Disable(TIM10); }

// Compare mode: Set compare channel 4 output polarity to active LOW or HIGH (default high; register CCER.CC4P)
INLINE void TIMx_OC4Low(TIM_t* tim) { tim->CCER |= B13; }
INLINE void TIM1_OC4Low(void) { TIMx_OC4Low(TIM1); }
INLINE void TIM2_OC4Low(void) { TIMx_OC4Low(TIM2); }
INLINE void TIM3_OC4Low(void) { TIMx_OC4Low(TIM3); }
INLINE void TIM4_OC4Low(void) { TIMx_OC4Low(TIM4); }
INLINE void TIM5_OC4Low(void) { TIMx_OC4Low(TIM5); }
INLINE void TIM6_OC4Low(void) { TIMx_OC4Low(TIM6); }
INLINE void TIM7_OC4Low(void) { TIMx_OC4Low(TIM7); }
INLINE void TIM8_OC4Low(void) { TIMx_OC4Low(TIM8); }
INLINE void TIM9_OC4Low(void) { TIMx_OC4Low(TIM9); }
INLINE void TIM10_OC4Low(void) { TIMx_OC4Low(TIM10); }

INLINE void TIMx_OC4High(TIM_t* tim) { tim->CCER &= ~B13; }
INLINE void TIM1_OC4High(void) { TIMx_OC4High(TIM1); }
INLINE void TIM2_OC4High(void) { TIMx_OC4High(TIM2); }
INLINE void TIM3_OC4High(void) { TIMx_OC4High(TIM3); }
INLINE void TIM4_OC4High(void) { TIMx_OC4High(TIM4); }
INLINE void TIM5_OC4High(void) { TIMx_OC4High(TIM5); }
INLINE void TIM6_OC4High(void) { TIMx_OC4High(TIM6); }
INLINE void TIM7_OC4High(void) { TIMx_OC4High(TIM7); }
INLINE void TIM8_OC4High(void) { TIMx_OC4High(TIM8); }
INLINE void TIM9_OC4High(void) { TIMx_OC4High(TIM9); }
INLINE void TIM10_OC4High(void) { TIMx_OC4High(TIM10); }

// Capture mode: Set capture channel 4 input polarity to falling or rising edge (default rising; register CCER.CC4P)
INLINE void TIMx_IC4Fall(TIM_t* tim) { tim->CCER |= B13; }
INLINE void TIM1_IC4Fall(void) { TIMx_IC4Fall(TIM1); }
INLINE void TIM2_IC4Fall(void) { TIMx_IC4Fall(TIM2); }
INLINE void TIM3_IC4Fall(void) { TIMx_IC4Fall(TIM3); }
INLINE void TIM4_IC4Fall(void) { TIMx_IC4Fall(TIM4); }
INLINE void TIM5_IC4Fall(void) { TIMx_IC4Fall(TIM5); }
INLINE void TIM6_IC4Fall(void) { TIMx_IC4Fall(TIM6); }
INLINE void TIM7_IC4Fall(void) { TIMx_IC4Fall(TIM7); }
INLINE void TIM8_IC4Fall(void) { TIMx_IC4Fall(TIM8); }
INLINE void TIM9_IC4Fall(void) { TIMx_IC4Fall(TIM9); }
INLINE void TIM10_IC4Fall(void) { TIMx_IC4Fall(TIM10); }

INLINE void TIMx_IC4Rise(TIM_t* tim) { tim->CCER &= ~B13; }
INLINE void TIM1_IC4Rise(void) { TIMx_IC4Rise(TIM1); }
INLINE void TIM2_IC4Rise(void) { TIMx_IC4Rise(TIM2); }
INLINE void TIM3_IC4Rise(void) { TIMx_IC4Rise(TIM3); }
INLINE void TIM4_IC4Rise(void) { TIMx_IC4Rise(TIM4); }
INLINE void TIM5_IC4Rise(void) { TIMx_IC4Rise(TIM5); }
INLINE void TIM6_IC4Rise(void) { TIMx_IC4Rise(TIM6); }
INLINE void TIM7_IC4Rise(void) { TIMx_IC4Rise(TIM7); }
INLINE void TIM8_IC4Rise(void) { TIMx_IC4Rise(TIM8); }
INLINE void TIM9_IC4Rise(void) { TIMx_IC4Rise(TIM9); }
INLINE void TIM10_IC4Rise(void) { TIMx_IC4Rise(TIM10); }

// === Counter

// Set/Get counter 0..65535 (default 0; register CNT)
INLINE void TIMx_Cnt(TIM_t* tim, int cnt) { tim->CNT = cnt; }
INLINE void TIM1_Cnt(int cnt) { TIMx_Cnt(TIM1, cnt); }
INLINE void TIM2_Cnt(int cnt) { TIMx_Cnt(TIM2, cnt); }
INLINE void TIM3_Cnt(int cnt) { TIMx_Cnt(TIM3, cnt); }
INLINE void TIM4_Cnt(int cnt) { TIMx_Cnt(TIM4, cnt); }
INLINE void TIM5_Cnt(int cnt) { TIMx_Cnt(TIM5, cnt); }
INLINE void TIM6_Cnt(int cnt) { TIMx_Cnt(TIM6, cnt); }
INLINE void TIM7_Cnt(int cnt) { TIMx_Cnt(TIM7, cnt); }
INLINE void TIM8_Cnt(int cnt) { TIMx_Cnt(TIM8, cnt); }
INLINE void TIM9_Cnt(int cnt) { TIMx_Cnt(TIM9, cnt); }
INLINE void TIM10_Cnt(int cnt) { TIMx_Cnt(TIM10, cnt); }

INLINE u16 TIMx_GetCnt(TIM_t* tim) { return (u16)tim->CNT; }
INLINE u16 TIM1_GetCnt(void) { return TIMx_GetCnt(TIM1); }
INLINE u16 TIM2_GetCnt(void) { return TIMx_GetCnt(TIM2); }
INLINE u16 TIM3_GetCnt(void) { return TIMx_GetCnt(TIM3); }
INLINE u16 TIM4_GetCnt(void) { return TIMx_GetCnt(TIM4); }
//INLINE u16 TIM5_GetCnt(void) { return TIMx_GetCnt(TIM5); }
INLINE int TIM5_GetCnt(void) { return TIM5->CNT; }
INLINE u16 TIM6_GetCnt(void) { return TIMx_GetCnt(TIM6); }
INLINE u16 TIM7_GetCnt(void) { return TIMx_GetCnt(TIM7); }
INLINE u16 TIM8_GetCnt(void) { return TIMx_GetCnt(TIM8); }
INLINE u16 TIM9_GetCnt(void) { return TIMx_GetCnt(TIM9); }
INLINE u16 TIM10_GetCnt(void) { return TIMx_GetCnt(TIM10); }

// Set/Get prescaler 0..65535 (divide ratio = presc+1; default 0 = no divide; register PSC)
INLINE void TIMx_Presc(TIM_t* tim, int presc) { tim->PSC = presc; }
INLINE void TIM1_Presc(int presc) { TIMx_Presc(TIM1, presc); }
INLINE void TIM2_Presc(int presc) { TIMx_Presc(TIM2, presc); }
INLINE void TIM3_Presc(int presc) { TIMx_Presc(TIM3, presc); }
INLINE void TIM4_Presc(int presc) { TIMx_Presc(TIM4, presc); }
INLINE void TIM5_Presc(int presc) { TIMx_Presc(TIM5, presc); }
INLINE void TIM6_Presc(int presc) { TIMx_Presc(TIM6, presc); }
INLINE void TIM7_Presc(int presc) { TIMx_Presc(TIM7, presc); }
INLINE void TIM8_Presc(int presc) { TIMx_Presc(TIM8, presc); }
INLINE void TIM9_Presc(int presc) { TIMx_Presc(TIM9, presc); }
INLINE void TIM10_Presc(int presc) { TIMx_Presc(TIM10, presc); }

INLINE u16 TIMx_GetPresc(TIM_t* tim) { return (u16)tim->PSC; }
INLINE u16 TIM1_GetPresc(void) { return TIMx_GetPresc(TIM1); }
INLINE u16 TIM2_GetPresc(void) { return TIMx_GetPresc(TIM2); }
INLINE u16 TIM3_GetPresc(void) { return TIMx_GetPresc(TIM3); }
INLINE u16 TIM4_GetPresc(void) { return TIMx_GetPresc(TIM4); }
//INLINE u16 TIM5_GetPresc(void) { return TIMx_GetPresc(TIM5); }
INLINE int TIM5_GetPresc(void) { return TIM5->PSC; }
INLINE u16 TIM6_GetPresc(void) { return TIMx_GetPresc(TIM6); }
INLINE u16 TIM7_GetPresc(void) { return TIMx_GetPresc(TIM7); }
INLINE u16 TIM8_GetPresc(void) { return TIMx_GetPresc(TIM8); }
INLINE u16 TIM9_GetPresc(void) { return TIMx_GetPresc(TIM9); }
INLINE u16 TIM10_GetPresc(void) { return TIMx_GetPresc(TIM10); }

// Set/Get timer divider 1..65536 (divide ratio = prescaler+1; default 1 = no divide)
INLINE void TIMx_Div(TIM_t* tim, int div) { TIMx_Presc(tim, div-1); }
INLINE void TIM1_Div(int div) { TIMx_Div(TIM1, div); }
INLINE void TIM2_Div(int div) { TIMx_Div(TIM2, div); }
INLINE void TIM3_Div(int div) { TIMx_Div(TIM3, div); }
INLINE void TIM4_Div(int div) { TIMx_Div(TIM4, div); }
INLINE void TIM5_Div(int div) { TIMx_Div(TIM5, div); }
INLINE void TIM6_Div(int div) { TIMx_Div(TIM6, div); }
INLINE void TIM7_Div(int div) { TIMx_Div(TIM7, div); }
INLINE void TIM8_Div(int div) { TIMx_Div(TIM8, div); }
INLINE void TIM9_Div(int div) { TIMx_Div(TIM9, div); }
INLINE void TIM10_Div(int div) { TIMx_Div(TIM10, div); }

INLINE int TIMx_GetDiv(TIM_t* tim) { return TIMx_GetPresc(tim)+1; }
INLINE int TIM1_GetDiv(void) { return TIMx_GetDiv(TIM1); }
INLINE int TIM2_GetDiv(void) { return TIMx_GetDiv(TIM2); }
INLINE int TIM3_GetDiv(void) { return TIMx_GetDiv(TIM3); }
INLINE int TIM4_GetDiv(void) { return TIMx_GetDiv(TIM4); }
INLINE int TIM5_GetDiv(void) { return TIMx_GetDiv(TIM5); }
INLINE int TIM6_GetDiv(void) { return TIMx_GetDiv(TIM6); }
INLINE int TIM7_GetDiv(void) { return TIMx_GetDiv(TIM7); }
INLINE int TIM8_GetDiv(void) { return TIMx_GetDiv(TIM8); }
INLINE int TIM9_GetDiv(void) { return TIMx_GetDiv(TIM9); }
INLINE int TIM10_GetDiv(void) { return TIMx_GetDiv(TIM10); }

// Set/Get auto-reload value 0..65535 (timer period = load+1; default 0 = 1 tick; register ATRLR)
INLINE void TIMx_Load(TIM_t* tim, int val) { tim->ATRLR = val; }
INLINE void TIM1_Load(int val) { TIMx_Load(TIM1, val); }
INLINE void TIM2_Load(int val) { TIMx_Load(TIM2, val); }
INLINE void TIM3_Load(int val) { TIMx_Load(TIM3, val); }
INLINE void TIM4_Load(int val) { TIMx_Load(TIM4, val); }
INLINE void TIM5_Load(int val) { TIMx_Load(TIM5, val); }
INLINE void TIM6_Load(int val) { TIMx_Load(TIM6, val); }
INLINE void TIM7_Load(int val) { TIMx_Load(TIM7, val); }
INLINE void TIM8_Load(int val) { TIMx_Load(TIM8, val); }
INLINE void TIM9_Load(int val) { TIMx_Load(TIM9, val); }
INLINE void TIM10_Load(int val) { TIMx_Load(TIM10, val); }

INLINE u16 TIMx_GetLoad(TIM_t* tim) { return (u16)tim->ATRLR; }
INLINE u16 TIM1_GetLoad(void) { return TIMx_GetLoad(TIM1); }
INLINE u16 TIM2_GetLoad(void) { return TIMx_GetLoad(TIM2); }
INLINE u16 TIM3_GetLoad(void) { return TIMx_GetLoad(TIM3); }
INLINE u16 TIM4_GetLoad(void) { return TIMx_GetLoad(TIM4); }
//INLINE u16 TIM5_GetLoad(void) { return TIMx_GetLoad(TIM5); }
INLINE int TIM5_GetLoad(void) { return TIM5->ATRLR; }
INLINE u16 TIM6_GetLoad(void) { return TIMx_GetLoad(TIM6); }
INLINE u16 TIM7_GetLoad(void) { return TIMx_GetLoad(TIM7); }
INLINE u16 TIM8_GetLoad(void) { return TIMx_GetLoad(TIM8); }
INLINE u16 TIM9_GetLoad(void) { return TIMx_GetLoad(TIM9); }
INLINE u16 TIM10_GetLoad(void) { return TIMx_GetLoad(TIM10); }

// Set/Get repeat count value 0..65535 (default 0; register RPTCR)
INLINE void TIMx_Repeat(TIM_t* tim, int val) { tim->RPTCR = val; }
INLINE void TIM1_Repeat(int val) { TIMx_Repeat(TIM1, val); }
INLINE void TIM2_Repeat(int val) { TIMx_Repeat(TIM2, val); }
INLINE void TIM3_Repeat(int val) { TIMx_Repeat(TIM3, val); }
INLINE void TIM4_Repeat(int val) { TIMx_Repeat(TIM4, val); }
INLINE void TIM5_Repeat(int val) { TIMx_Repeat(TIM5, val); }
INLINE void TIM6_Repeat(int val) { TIMx_Repeat(TIM6, val); }
INLINE void TIM7_Repeat(int val) { TIMx_Repeat(TIM7, val); }
INLINE void TIM8_Repeat(int val) { TIMx_Repeat(TIM8, val); }
INLINE void TIM9_Repeat(int val) { TIMx_Repeat(TIM9, val); }
INLINE void TIM10_Repeat(int val) { TIMx_Repeat(TIM10, val); }

INLINE u16 TIMx_GetRepeat(TIM_t* tim) { return (u16)tim->RPTCR; }
INLINE u16 TIM1_GetRepeat(void) { return TIMx_GetRepeat(TIM1); }
INLINE u16 TIM2_GetRepeat(void) { return TIMx_GetRepeat(TIM2); }
INLINE u16 TIM3_GetRepeat(void) { return TIMx_GetRepeat(TIM3); }
INLINE u16 TIM4_GetRepeat(void) { return TIMx_GetRepeat(TIM4); }
//INLINE u16 TIM5_GetRepeat(void) { return TIMx_GetRepeat(TIM5); }
INLINE int TIM5_GetRepeat(void) { return TIM5->RPTCR; }
INLINE u16 TIM6_GetRepeat(void) { return TIMx_GetRepeat(TIM6); }
INLINE u16 TIM7_GetRepeat(void) { return TIMx_GetRepeat(TIM7); }
INLINE u16 TIM8_GetRepeat(void) { return TIMx_GetRepeat(TIM8); }
INLINE u16 TIM9_GetRepeat(void) { return TIMx_GetRepeat(TIM9); }
INLINE u16 TIM10_GetRepeat(void) { return TIMx_GetRepeat(TIM10); }

// Set/Get compare value of channel 1, 0..65535 (default 0; register CH1CVR.CH1CVR)
INLINE void TIMx_Comp1(TIM_t* tim, int val) { tim->CH1CVR = val; }
INLINE void TIM1_Comp1(int val) { TIMx_Comp1(TIM1, val); }
INLINE void TIM2_Comp1(int val) { TIMx_Comp1(TIM2, val); }
INLINE void TIM3_Comp1(int val) { TIMx_Comp1(TIM3, val); }
INLINE void TIM4_Comp1(int val) { TIMx_Comp1(TIM4, val); }
INLINE void TIM5_Comp1(int val) { TIMx_Comp1(TIM5, val); }
INLINE void TIM6_Comp1(int val) { TIMx_Comp1(TIM6, val); }
INLINE void TIM7_Comp1(int val) { TIMx_Comp1(TIM7, val); }
INLINE void TIM8_Comp1(int val) { TIMx_Comp1(TIM8, val); }
INLINE void TIM9_Comp1(int val) { TIMx_Comp1(TIM9, val); }
INLINE void TIM10_Comp1(int val) { TIMx_Comp1(TIM10, val); }

INLINE u16 TIMx_GetComp1(TIM_t* tim) { return (u16)tim->CH1CVR; }
INLINE u16 TIM1_GetComp1(void) { return TIMx_GetComp1(TIM1); }
INLINE u16 TIM2_GetComp1(void) { return TIMx_GetComp1(TIM2); }
INLINE u16 TIM3_GetComp1(void) { return TIMx_GetComp1(TIM3); }
INLINE u16 TIM4_GetComp1(void) { return TIMx_GetComp1(TIM4); }
//INLINE u16 TIM5_GetComp1(void) { return TIMx_GetComp1(TIM5); }
INLINE int TIM5_GetComp1(void) { return TIM5->CH1CVR; }
INLINE u16 TIM6_GetComp1(void) { return TIMx_GetComp1(TIM6); }
INLINE u16 TIM7_GetComp1(void) { return TIMx_GetComp1(TIM7); }
INLINE u16 TIM8_GetComp1(void) { return TIMx_GetComp1(TIM8); }
INLINE u16 TIM9_GetComp1(void) { return TIMx_GetComp1(TIM9); }
INLINE u16 TIM10_GetComp1(void) { return TIMx_GetComp1(TIM10); }

// Check level flag of captured value channel 1 (register CH1CVR.LEVEL1)
INLINE Bool TIMx_GetLevel1(TIM_t* tim) { return (tim->CH1CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel1(void) { return TIMx_GetLevel1(TIM1); }
INLINE Bool TIM2_GetLevel1(void) { return TIMx_GetLevel1(TIM2); }
INLINE Bool TIM3_GetLevel1(void) { return TIMx_GetLevel1(TIM3); }
INLINE Bool TIM4_GetLevel1(void) { return TIMx_GetLevel1(TIM4); }
INLINE Bool TIM5_GetLevel1(void) { return TIMx_GetLevel1(TIM5); }
INLINE Bool TIM6_GetLevel1(void) { return TIMx_GetLevel1(TIM6); }
INLINE Bool TIM7_GetLevel1(void) { return TIMx_GetLevel1(TIM7); }
INLINE Bool TIM8_GetLevel1(void) { return TIMx_GetLevel1(TIM8); }
INLINE Bool TIM9_GetLevel1(void) { return TIMx_GetLevel1(TIM9); }
INLINE Bool TIM10_GetLevel1(void) { return TIMx_GetLevel1(TIM10); }

// Set/Get compare value of channel 2, 0..65535 (default 0; register CH2CVR.CH2CVR)
INLINE void TIMx_Comp2(TIM_t* tim, int val) { tim->CH2CVR = val; }
INLINE void TIM1_Comp2(int val) { TIMx_Comp2(TIM1, val); }
INLINE void TIM2_Comp2(int val) { TIMx_Comp2(TIM2, val); }
INLINE void TIM3_Comp2(int val) { TIMx_Comp2(TIM3, val); }
INLINE void TIM4_Comp2(int val) { TIMx_Comp2(TIM4, val); }
INLINE void TIM5_Comp2(int val) { TIMx_Comp2(TIM5, val); }
INLINE void TIM6_Comp2(int val) { TIMx_Comp2(TIM6, val); }
INLINE void TIM7_Comp2(int val) { TIMx_Comp2(TIM7, val); }
INLINE void TIM8_Comp2(int val) { TIMx_Comp2(TIM8, val); }
INLINE void TIM9_Comp2(int val) { TIMx_Comp2(TIM9, val); }
INLINE void TIM10_Comp2(int val) { TIMx_Comp2(TIM10, val); }

INLINE u16 TIMx_GetComp2(TIM_t* tim) { return (u16)tim->CH2CVR; }
INLINE u16 TIM1_GetComp2(void) { return TIMx_GetComp2(TIM1); }
INLINE u16 TIM2_GetComp2(void) { return TIMx_GetComp2(TIM2); }
INLINE u16 TIM3_GetComp2(void) { return TIMx_GetComp2(TIM3); }
INLINE u16 TIM4_GetComp2(void) { return TIMx_GetComp2(TIM4); }
//INLINE u16 TIM5_GetComp2(void) { return TIMx_GetComp2(TIM5); }
INLINE int TIM5_GetComp2(void) { return TIM5->CH2CVR; }
INLINE u16 TIM6_GetComp2(void) { return TIMx_GetComp2(TIM6); }
INLINE u16 TIM7_GetComp2(void) { return TIMx_GetComp2(TIM7); }
INLINE u16 TIM8_GetComp2(void) { return TIMx_GetComp2(TIM8); }
INLINE u16 TIM9_GetComp2(void) { return TIMx_GetComp2(TIM9); }
INLINE u16 TIM10_GetComp2(void) { return TIMx_GetComp2(TIM10); }

// Check level flag of captured value channel 2 (register CH2CVR.LEVEL2)
INLINE Bool TIMx_GetLevel2(TIM_t* tim) { return (tim->CH2CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel2(void) { return TIMx_GetLevel2(TIM1); }
INLINE Bool TIM2_GetLevel2(void) { return TIMx_GetLevel2(TIM2); }
INLINE Bool TIM3_GetLevel2(void) { return TIMx_GetLevel2(TIM3); }
INLINE Bool TIM4_GetLevel2(void) { return TIMx_GetLevel2(TIM4); }
INLINE Bool TIM5_GetLevel2(void) { return TIMx_GetLevel2(TIM5); }
INLINE Bool TIM6_GetLevel2(void) { return TIMx_GetLevel2(TIM6); }
INLINE Bool TIM7_GetLevel2(void) { return TIMx_GetLevel2(TIM7); }
INLINE Bool TIM8_GetLevel2(void) { return TIMx_GetLevel2(TIM8); }
INLINE Bool TIM9_GetLevel2(void) { return TIMx_GetLevel2(TIM9); }
INLINE Bool TIM10_GetLevel2(void) { return TIMx_GetLevel2(TIM10); }

// Set/Get compare value of channel 3, 0..65535 (default 0; register CH3CVR.CH3CVR)
INLINE void TIMx_Comp3(TIM_t* tim, int val) { tim->CH3CVR = val; }
INLINE void TIM1_Comp3(int val) { TIMx_Comp3(TIM1, val); }
INLINE void TIM2_Comp3(int val) { TIMx_Comp3(TIM2, val); }
INLINE void TIM3_Comp3(int val) { TIMx_Comp3(TIM3, val); }
INLINE void TIM4_Comp3(int val) { TIMx_Comp3(TIM4, val); }
INLINE void TIM5_Comp3(int val) { TIMx_Comp3(TIM5, val); }
INLINE void TIM6_Comp3(int val) { TIMx_Comp3(TIM6, val); }
INLINE void TIM7_Comp3(int val) { TIMx_Comp3(TIM7, val); }
INLINE void TIM8_Comp3(int val) { TIMx_Comp3(TIM8, val); }
INLINE void TIM9_Comp3(int val) { TIMx_Comp3(TIM9, val); }
INLINE void TIM10_Comp3(int val) { TIMx_Comp3(TIM10, val); }

INLINE u16 TIMx_GetComp3(TIM_t* tim) { return (u16)tim->CH3CVR; }
INLINE u16 TIM1_GetComp3(void) { return TIMx_GetComp3(TIM1); }
INLINE u16 TIM2_GetComp3(void) { return TIMx_GetComp3(TIM2); }
INLINE u16 TIM3_GetComp3(void) { return TIMx_GetComp3(TIM3); }
INLINE u16 TIM4_GetComp3(void) { return TIMx_GetComp3(TIM4); }
//INLINE u16 TIM5_GetComp3(void) { return TIMx_GetComp3(TIM5); }
INLINE int TIM5_GetComp3(void) { return TIM5->CH3CVR; }
INLINE u16 TIM6_GetComp3(void) { return TIMx_GetComp3(TIM6); }
INLINE u16 TIM7_GetComp3(void) { return TIMx_GetComp3(TIM7); }
INLINE u16 TIM8_GetComp3(void) { return TIMx_GetComp3(TIM8); }
INLINE u16 TIM9_GetComp3(void) { return TIMx_GetComp3(TIM9); }
INLINE u16 TIM10_GetComp3(void) { return TIMx_GetComp3(TIM10); }

// Check level flag of captured value channel 3 (register CH3CVR.LEVEL3)
INLINE Bool TIMx_GetLevel3(TIM_t* tim) { return (tim->CH3CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel3(void) { return TIMx_GetLevel3(TIM1); }
INLINE Bool TIM2_GetLevel3(void) { return TIMx_GetLevel3(TIM2); }
INLINE Bool TIM3_GetLevel3(void) { return TIMx_GetLevel3(TIM3); }
INLINE Bool TIM4_GetLevel3(void) { return TIMx_GetLevel3(TIM4); }
INLINE Bool TIM5_GetLevel3(void) { return TIMx_GetLevel3(TIM5); }
INLINE Bool TIM6_GetLevel3(void) { return TIMx_GetLevel3(TIM6); }
INLINE Bool TIM7_GetLevel3(void) { return TIMx_GetLevel3(TIM7); }
INLINE Bool TIM8_GetLevel3(void) { return TIMx_GetLevel3(TIM8); }
INLINE Bool TIM9_GetLevel3(void) { return TIMx_GetLevel3(TIM9); }
INLINE Bool TIM10_GetLevel3(void) { return TIMx_GetLevel3(TIM10); }

// Set/Get compare value of channel 4, 0..65535 (default 0; register CH4CVR.CH4CVR)
INLINE void TIMx_Comp4(TIM_t* tim, int val) { tim->CH4CVR = val; }
INLINE void TIM1_Comp4(int val) { TIMx_Comp4(TIM1, val); }
INLINE void TIM2_Comp4(int val) { TIMx_Comp4(TIM2, val); }
INLINE void TIM3_Comp4(int val) { TIMx_Comp4(TIM3, val); }
INLINE void TIM4_Comp4(int val) { TIMx_Comp4(TIM4, val); }
INLINE void TIM5_Comp4(int val) { TIMx_Comp4(TIM5, val); }
INLINE void TIM6_Comp4(int val) { TIMx_Comp4(TIM6, val); }
INLINE void TIM7_Comp4(int val) { TIMx_Comp4(TIM7, val); }
INLINE void TIM8_Comp4(int val) { TIMx_Comp4(TIM8, val); }
INLINE void TIM9_Comp4(int val) { TIMx_Comp4(TIM9, val); }
INLINE void TIM10_Comp4(int val) { TIMx_Comp4(TIM10, val); }

INLINE u16 TIMx_GetComp4(TIM_t* tim) { return (u16)tim->CH4CVR; }
INLINE u16 TIM1_GetComp4(void) { return TIMx_GetComp4(TIM1); }
INLINE u16 TIM2_GetComp4(void) { return TIMx_GetComp4(TIM2); }
INLINE u16 TIM3_GetComp4(void) { return TIMx_GetComp4(TIM3); }
INLINE u16 TIM4_GetComp4(void) { return TIMx_GetComp4(TIM4); }
//INLINE u16 TIM5_GetComp4(void) { return TIMx_GetComp4(TIM5); }
INLINE int TIM5_GetComp4(void) { return TIM5->CH4CVR; }
INLINE u16 TIM6_GetComp4(void) { return TIMx_GetComp4(TIM6); }
INLINE u16 TIM7_GetComp4(void) { return TIMx_GetComp4(TIM7); }
INLINE u16 TIM8_GetComp4(void) { return TIMx_GetComp4(TIM8); }
INLINE u16 TIM9_GetComp4(void) { return TIMx_GetComp4(TIM9); }
INLINE u16 TIM10_GetComp4(void) { return TIMx_GetComp4(TIM10); }

// Check level flag of captured value channel 4 (register CH4CVR.LEVEL4)
INLINE Bool TIMx_GetLevel4(TIM_t* tim) { return (tim->CH4CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel4(void) { return TIMx_GetLevel4(TIM1); }
INLINE Bool TIM2_GetLevel4(void) { return TIMx_GetLevel4(TIM2); }
INLINE Bool TIM3_GetLevel4(void) { return TIMx_GetLevel4(TIM3); }
INLINE Bool TIM4_GetLevel4(void) { return TIMx_GetLevel4(TIM4); }
INLINE Bool TIM5_GetLevel4(void) { return TIMx_GetLevel4(TIM5); }
INLINE Bool TIM6_GetLevel4(void) { return TIMx_GetLevel4(TIM6); }
INLINE Bool TIM7_GetLevel4(void) { return TIMx_GetLevel4(TIM7); }
INLINE Bool TIM8_GetLevel4(void) { return TIMx_GetLevel4(TIM8); }
INLINE Bool TIM9_GetLevel4(void) { return TIMx_GetLevel4(TIM9); }
INLINE Bool TIM10_GetLevel4(void) { return TIMx_GetLevel4(TIM10); }

// === Brake and deadtime

// Set/Get deadband time 0..255 (default 0; register BDTR.DTG[7:0])
//  dt=0..127 -> DT=dt*Tdtg, Tdtg=TDTS
//  dt=128..191 -> DT=(64 + dt-128)*Tdtg, Tdtg=2*TDTS
//  dt=192..223 -> DT=(32 + dt-192)*Tdtg, Tdtg=8*TDTS
//  dt=224..255 -> DT=(32 + dt-224)*Tdtg, Tdtg=16*TDTS
INLINE void TIMx_DTG(TIM_t* tim, int dt) { tim->BDTR = (tim->BDTR & ~0xff) | dt; }
INLINE void TIM1_DTG(int dt) { TIMx_DTG(TIM1, dt); }
INLINE void TIM2_DTG(int dt) { TIMx_DTG(TIM2, dt); }
INLINE void TIM3_DTG(int dt) { TIMx_DTG(TIM3, dt); }
INLINE void TIM4_DTG(int dt) { TIMx_DTG(TIM4, dt); }
INLINE void TIM5_DTG(int dt) { TIMx_DTG(TIM5, dt); }
INLINE void TIM6_DTG(int dt) { TIMx_DTG(TIM6, dt); }
INLINE void TIM7_DTG(int dt) { TIMx_DTG(TIM7, dt); }
INLINE void TIM8_DTG(int dt) { TIMx_DTG(TIM8, dt); }
INLINE void TIM9_DTG(int dt) { TIMx_DTG(TIM9, dt); }
INLINE void TIM10_DTG(int dt) { TIMx_DTG(TIM10, dt); }

INLINE u8 TIMx_GetDTG(TIM_t* tim) { return (u8)tim->BDTR; }
INLINE u8 TIM1_GetDTG(void) { return TIMx_GetDTG(TIM1); }
INLINE u8 TIM2_GetDTG(void) { return TIMx_GetDTG(TIM2); }
INLINE u8 TIM3_GetDTG(void) { return TIMx_GetDTG(TIM3); }
INLINE u8 TIM4_GetDTG(void) { return TIMx_GetDTG(TIM4); }
INLINE u8 TIM5_GetDTG(void) { return TIMx_GetDTG(TIM5); }
INLINE u8 TIM6_GetDTG(void) { return TIMx_GetDTG(TIM6); }
INLINE u8 TIM7_GetDTG(void) { return TIMx_GetDTG(TIM7); }
INLINE u8 TIM8_GetDTG(void) { return TIMx_GetDTG(TIM8); }
INLINE u8 TIM9_GetDTG(void) { return TIMx_GetDTG(TIM9); }
INLINE u8 TIM10_GetDTG(void) { return TIMx_GetDTG(TIM10); }

// Set lock function TIM_LOCK_* (default no lock; register BDTR.LOCK[1:0])
// - Lock bits can only be written once and cannot be modified again until reset.
INLINE void TIMx_Lock(TIM_t* tim, int lock) { tim->BDTR = (tim->BDTR & ~(3<<8)) | (lock<<8); }
INLINE void TIM1_Lock(int lock) { TIMx_Lock(TIM1, lock); }
INLINE void TIM2_Lock(int lock) { TIMx_Lock(TIM2, lock); }
INLINE void TIM3_Lock(int lock) { TIMx_Lock(TIM3, lock); }
INLINE void TIM4_Lock(int lock) { TIMx_Lock(TIM4, lock); }
INLINE void TIM5_Lock(int lock) { TIMx_Lock(TIM5, lock); }
INLINE void TIM6_Lock(int lock) { TIMx_Lock(TIM6, lock); }
INLINE void TIM7_Lock(int lock) { TIMx_Lock(TIM7, lock); }
INLINE void TIM8_Lock(int lock) { TIMx_Lock(TIM8, lock); }
INLINE void TIM9_Lock(int lock) { TIMx_Lock(TIM9, lock); }
INLINE void TIM10_Lock(int lock) { TIMx_Lock(TIM10, lock); }

// Enable/Disable idle level of OC outputs (default outputs are disabled when timer is off; register BDTR.OSSI)
INLINE void TIMx_OSSIEnable(TIM_t* tim) { tim->BDTR |= B10; }
INLINE void TIM1_OSSIEnable(void) { TIMx_OSSIEnable(TIM1); }
INLINE void TIM2_OSSIEnable(void) { TIMx_OSSIEnable(TIM2); }
INLINE void TIM3_OSSIEnable(void) { TIMx_OSSIEnable(TIM3); }
INLINE void TIM4_OSSIEnable(void) { TIMx_OSSIEnable(TIM4); }
INLINE void TIM5_OSSIEnable(void) { TIMx_OSSIEnable(TIM5); }
INLINE void TIM6_OSSIEnable(void) { TIMx_OSSIEnable(TIM6); }
INLINE void TIM7_OSSIEnable(void) { TIMx_OSSIEnable(TIM7); }
INLINE void TIM8_OSSIEnable(void) { TIMx_OSSIEnable(TIM8); }
INLINE void TIM9_OSSIEnable(void) { TIMx_OSSIEnable(TIM9); }
INLINE void TIM10_OSSIEnable(void) { TIMx_OSSIEnable(TIM10); }

INLINE void TIMx_OSSIDisable(TIM_t* tim) { tim->BDTR &= ~B10; }
INLINE void TIM1_OSSIDisable(void) { TIMx_OSSIDisable(TIM1); }
INLINE void TIM2_OSSIDisable(void) { TIMx_OSSIDisable(TIM2); }
INLINE void TIM3_OSSIDisable(void) { TIMx_OSSIDisable(TIM3); }
INLINE void TIM4_OSSIDisable(void) { TIMx_OSSIDisable(TIM4); }
INLINE void TIM5_OSSIDisable(void) { TIMx_OSSIDisable(TIM5); }
INLINE void TIM6_OSSIDisable(void) { TIMx_OSSIDisable(TIM6); }
INLINE void TIM7_OSSIDisable(void) { TIMx_OSSIDisable(TIM7); }
INLINE void TIM8_OSSIDisable(void) { TIMx_OSSIDisable(TIM8); }
INLINE void TIM9_OSSIDisable(void) { TIMx_OSSIDisable(TIM9); }
INLINE void TIM10_OSSIDisable(void) { TIMx_OSSIDisable(TIM10); }

// Enable/Disable invalid level of OC outputs (default outputs are disabled when timer is off; register BDTR.OSSR)
INLINE void TIMx_OSSREnable(TIM_t* tim) { tim->BDTR |= B11; }
INLINE void TIM1_OSSREnable(void) { TIMx_OSSREnable(TIM1); }
INLINE void TIM2_OSSREnable(void) { TIMx_OSSREnable(TIM2); }
INLINE void TIM3_OSSREnable(void) { TIMx_OSSREnable(TIM3); }
INLINE void TIM4_OSSREnable(void) { TIMx_OSSREnable(TIM4); }
INLINE void TIM5_OSSREnable(void) { TIMx_OSSREnable(TIM5); }
INLINE void TIM6_OSSREnable(void) { TIMx_OSSREnable(TIM6); }
INLINE void TIM7_OSSREnable(void) { TIMx_OSSREnable(TIM7); }
INLINE void TIM8_OSSREnable(void) { TIMx_OSSREnable(TIM8); }
INLINE void TIM9_OSSREnable(void) { TIMx_OSSREnable(TIM9); }
INLINE void TIM10_OSSREnable(void) { TIMx_OSSREnable(TIM10); }

INLINE void TIMx_OSSRDisable(TIM_t* tim) { tim->BDTR &= ~B11; }
INLINE void TIM1_OSSRDisable(void) { TIMx_OSSRDisable(TIM1); }
INLINE void TIM2_OSSRDisable(void) { TIMx_OSSRDisable(TIM2); }
INLINE void TIM3_OSSRDisable(void) { TIMx_OSSRDisable(TIM3); }
INLINE void TIM4_OSSRDisable(void) { TIMx_OSSRDisable(TIM4); }
INLINE void TIM5_OSSRDisable(void) { TIMx_OSSRDisable(TIM5); }
INLINE void TIM6_OSSRDisable(void) { TIMx_OSSRDisable(TIM6); }
INLINE void TIM7_OSSRDisable(void) { TIMx_OSSRDisable(TIM7); }
INLINE void TIM8_OSSRDisable(void) { TIMx_OSSRDisable(TIM8); }
INLINE void TIM9_OSSRDisable(void) { TIMx_OSSRDisable(TIM9); }
INLINE void TIM10_OSSRDisable(void) { TIMx_OSSRDisable(TIM10); }

// Enable/Disable brake input (default disabled; register BDTR.BKE)
INLINE void TIMx_BrkEnable(TIM_t* tim) { tim->BDTR |= B12; }
INLINE void TIM1_BrkEnable(void) { TIMx_BrkEnable(TIM1); }
INLINE void TIM2_BrkEnable(void) { TIMx_BrkEnable(TIM2); }
INLINE void TIM3_BrkEnable(void) { TIMx_BrkEnable(TIM3); }
INLINE void TIM4_BrkEnable(void) { TIMx_BrkEnable(TIM4); }
INLINE void TIM5_BrkEnable(void) { TIMx_BrkEnable(TIM5); }
INLINE void TIM6_BrkEnable(void) { TIMx_BrkEnable(TIM6); }
INLINE void TIM7_BrkEnable(void) { TIMx_BrkEnable(TIM7); }
INLINE void TIM8_BrkEnable(void) { TIMx_BrkEnable(TIM8); }
INLINE void TIM9_BrkEnable(void) { TIMx_BrkEnable(TIM9); }
INLINE void TIM10_BrkEnable(void) { TIMx_BrkEnable(TIM10); }

INLINE void TIMx_BrkDisable(TIM_t* tim) { tim->BDTR &= ~B12; }
INLINE void TIM1_BrkDisable(void) { TIMx_BrkDisable(TIM1); }
INLINE void TIM2_BrkDisable(void) { TIMx_BrkDisable(TIM2); }
INLINE void TIM3_BrkDisable(void) { TIMx_BrkDisable(TIM3); }
INLINE void TIM4_BrkDisable(void) { TIMx_BrkDisable(TIM4); }
INLINE void TIM5_BrkDisable(void) { TIMx_BrkDisable(TIM5); }
INLINE void TIM6_BrkDisable(void) { TIMx_BrkDisable(TIM6); }
INLINE void TIM7_BrkDisable(void) { TIMx_BrkDisable(TIM7); }
INLINE void TIM8_BrkDisable(void) { TIMx_BrkDisable(TIM8); }
INLINE void TIM9_BrkDisable(void) { TIMx_BrkDisable(TIM9); }
INLINE void TIM10_BrkDisable(void) { TIMx_BrkDisable(TIM10); }

// Set brake input polarity to high or low (default low; register BDTR.BKP)
INLINE void TIMx_BrkHigh(TIM_t* tim) { tim->BDTR |= B13; }
INLINE void TIM1_BrkHigh(void) { TIMx_BrkHigh(TIM1); }
INLINE void TIM2_BrkHigh(void) { TIMx_BrkHigh(TIM2); }
INLINE void TIM3_BrkHigh(void) { TIMx_BrkHigh(TIM3); }
INLINE void TIM4_BrkHigh(void) { TIMx_BrkHigh(TIM4); }
INLINE void TIM5_BrkHigh(void) { TIMx_BrkHigh(TIM5); }
INLINE void TIM6_BrkHigh(void) { TIMx_BrkHigh(TIM6); }
INLINE void TIM7_BrkHigh(void) { TIMx_BrkHigh(TIM7); }
INLINE void TIM8_BrkHigh(void) { TIMx_BrkHigh(TIM8); }
INLINE void TIM9_BrkHigh(void) { TIMx_BrkHigh(TIM9); }
INLINE void TIM10_BrkHigh(void) { TIMx_BrkHigh(TIM10); }

INLINE void TIMx_BrkLow(TIM_t* tim) { tim->BDTR &= ~B13; }
INLINE void TIM1_BrkLow(void) { TIMx_BrkLow(TIM1); }
INLINE void TIM2_BrkLow(void) { TIMx_BrkLow(TIM2); }
INLINE void TIM3_BrkLow(void) { TIMx_BrkLow(TIM3); }
INLINE void TIM4_BrkLow(void) { TIMx_BrkLow(TIM4); }
INLINE void TIM5_BrkLow(void) { TIMx_BrkLow(TIM5); }
INLINE void TIM6_BrkLow(void) { TIMx_BrkLow(TIM6); }
INLINE void TIM7_BrkLow(void) { TIMx_BrkLow(TIM7); }
INLINE void TIM8_BrkLow(void) { TIMx_BrkLow(TIM8); }
INLINE void TIM9_BrkLow(void) { TIMx_BrkLow(TIM9); }
INLINE void TIM10_BrkLow(void) { TIMx_BrkLow(TIM10); }

// Enable/Disable auto OC output (default disable; register BDTR.AOE)
INLINE void TIMx_AutoOCEnable(TIM_t* tim) { tim->BDTR |= B14; }
INLINE void TIM1_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM1); }
INLINE void TIM2_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM2); }
INLINE void TIM3_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM3); }
INLINE void TIM4_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM4); }
INLINE void TIM5_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM5); }
INLINE void TIM6_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM6); }
INLINE void TIM7_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM7); }
INLINE void TIM8_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM8); }
INLINE void TIM9_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM9); }
INLINE void TIM10_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM10); }

INLINE void TIMx_AutoOCDisable(TIM_t* tim) { tim->BDTR &= ~B14; }
INLINE void TIM1_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM1); }
INLINE void TIM2_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM2); }
INLINE void TIM3_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM3); }
INLINE void TIM4_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM4); }
INLINE void TIM5_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM5); }
INLINE void TIM6_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM6); }
INLINE void TIM7_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM7); }
INLINE void TIM8_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM8); }
INLINE void TIM9_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM9); }
INLINE void TIM10_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM10); }

// Enable/Disable OC output (default disable; register BDTR.MOE)
INLINE void TIMx_OCEnable(TIM_t* tim) { tim->BDTR |= B15; }
INLINE void TIM1_OCEnable(void) { TIMx_OCEnable(TIM1); }
INLINE void TIM2_OCEnable(void) { TIMx_OCEnable(TIM2); }
INLINE void TIM3_OCEnable(void) { TIMx_OCEnable(TIM3); }
INLINE void TIM4_OCEnable(void) { TIMx_OCEnable(TIM4); }
INLINE void TIM5_OCEnable(void) { TIMx_OCEnable(TIM5); }
INLINE void TIM6_OCEnable(void) { TIMx_OCEnable(TIM6); }
INLINE void TIM7_OCEnable(void) { TIMx_OCEnable(TIM7); }
INLINE void TIM8_OCEnable(void) { TIMx_OCEnable(TIM8); }
INLINE void TIM9_OCEnable(void) { TIMx_OCEnable(TIM9); }
INLINE void TIM10_OCEnable(void) { TIMx_OCEnable(TIM10); }

INLINE void TIMx_OCDisable(TIM_t* tim) { tim->BDTR &= ~B15; }
INLINE void TIM1_OCDisable(void) { TIMx_OCDisable(TIM1); }
INLINE void TIM2_OCDisable(void) { TIMx_OCDisable(TIM2); }
INLINE void TIM3_OCDisable(void) { TIMx_OCDisable(TIM3); }
INLINE void TIM4_OCDisable(void) { TIMx_OCDisable(TIM4); }
INLINE void TIM5_OCDisable(void) { TIMx_OCDisable(TIM5); }
INLINE void TIM6_OCDisable(void) { TIMx_OCDisable(TIM6); }
INLINE void TIM7_OCDisable(void) { TIMx_OCDisable(TIM7); }
INLINE void TIM8_OCDisable(void) { TIMx_OCDisable(TIM8); }
INLINE void TIM9_OCDisable(void) { TIMx_OCDisable(TIM9); }
INLINE void TIM10_OCDisable(void) { TIMx_OCDisable(TIM10); }

// === DMA setup

// Set offset address of DMA in continuous mode 0..31 (default 0; register DMACFGR.DBA[4:0])
INLINE void TIMx_DMAOff(TIM_t* tim, int off) { tim->DMACFGR = (tim->DMACFGR & ~0x1f) | off; }
INLINE void TIM1_DMAOff(int off) { TIMx_DMAOff(TIM1, off); }
INLINE void TIM2_DMAOff(int off) { TIMx_DMAOff(TIM2, off); }
INLINE void TIM3_DMAOff(int off) { TIMx_DMAOff(TIM3, off); }
INLINE void TIM4_DMAOff(int off) { TIMx_DMAOff(TIM4, off); }
INLINE void TIM5_DMAOff(int off) { TIMx_DMAOff(TIM5, off); }
INLINE void TIM6_DMAOff(int off) { TIMx_DMAOff(TIM6, off); }
INLINE void TIM7_DMAOff(int off) { TIMx_DMAOff(TIM7, off); }
INLINE void TIM8_DMAOff(int off) { TIMx_DMAOff(TIM8, off); }
INLINE void TIM9_DMAOff(int off) { TIMx_DMAOff(TIM9, off); }
INLINE void TIM10_DMAOff(int off) { TIMx_DMAOff(TIM10, off); }

// Set length of DMA continuous transmission 0..31 (default 0; register DMACFGR.DBL[4:0])
INLINE void TIMx_DMALen(TIM_t* tim, int len) { tim->DMACFGR = (tim->DMACFGR & ~(0x1f<<8)) | (len<<8); }
INLINE void TIM1_DMALen(int len) { TIMx_DMALen(TIM1, len); }
INLINE void TIM2_DMALen(int len) { TIMx_DMALen(TIM2, len); }
INLINE void TIM3_DMALen(int len) { TIMx_DMALen(TIM3, len); }
INLINE void TIM4_DMALen(int len) { TIMx_DMALen(TIM4, len); }
INLINE void TIM5_DMALen(int len) { TIMx_DMALen(TIM5, len); }
INLINE void TIM6_DMALen(int len) { TIMx_DMALen(TIM6, len); }
INLINE void TIM7_DMALen(int len) { TIMx_DMALen(TIM7, len); }
INLINE void TIM8_DMALen(int len) { TIMx_DMALen(TIM8, len); }
INLINE void TIM9_DMALen(int len) { TIMx_DMALen(TIM9, len); }
INLINE void TIM10_DMALen(int len) { TIMx_DMALen(TIM10, len); }

// Set base address of DMA in continuous mode 0..65535 (default 0; register DMAADR.DMAB[15:0])
INLINE void TIMx_DMABase(TIM_t* tim, int base) { tim->DMAADR = base; }
INLINE void TIM1_DMABase(int base) { TIMx_DMABase(TIM1, base); }
INLINE void TIM2_DMABase(int base) { TIMx_DMABase(TIM2, base); }
INLINE void TIM3_DMABase(int base) { TIMx_DMABase(TIM3, base); }
INLINE void TIM4_DMABase(int base) { TIMx_DMABase(TIM4, base); }
INLINE void TIM5_DMABase(int base) { TIMx_DMABase(TIM5, base); }
INLINE void TIM6_DMABase(int base) { TIMx_DMABase(TIM6, base); }
INLINE void TIM7_DMABase(int base) { TIMx_DMABase(TIM7, base); }
INLINE void TIM8_DMABase(int base) { TIMx_DMABase(TIM8, base); }
INLINE void TIM9_DMABase(int base) { TIMx_DMABase(TIM9, base); }
INLINE void TIM10_DMABase(int base) { TIMx_DMABase(TIM10, base); }

// === Dual-edge capture

// Enable/Disable dual-edge for channel 2 (default disable; register AUX.CAP_ED_CH2)
INLINE void TIMx_Dual2Enable(TIM_t* tim) { tim->AUX |= B0; }
INLINE void TIM1_Dual2Enable(void) { TIMx_Dual2Enable(TIM1); }
INLINE void TIM2_Dual2Enable(void) { TIMx_Dual2Enable(TIM2); }
INLINE void TIM3_Dual2Enable(void) { TIMx_Dual2Enable(TIM3); }
INLINE void TIM4_Dual2Enable(void) { TIMx_Dual2Enable(TIM4); }
INLINE void TIM5_Dual2Enable(void) { TIMx_Dual2Enable(TIM5); }
INLINE void TIM6_Dual2Enable(void) { TIMx_Dual2Enable(TIM6); }
INLINE void TIM7_Dual2Enable(void) { TIMx_Dual2Enable(TIM7); }
INLINE void TIM8_Dual2Enable(void) { TIMx_Dual2Enable(TIM8); }
INLINE void TIM9_Dual2Enable(void) { TIMx_Dual2Enable(TIM9); }
INLINE void TIM10_Dual2Enable(void) { TIMx_Dual2Enable(TIM10); }

INLINE void TIMx_Dual2Disable(TIM_t* tim) { tim->AUX &= ~B0; }
INLINE void TIM1_Dual2Disable(void) { TIMx_Dual2Disable(TIM1); }
INLINE void TIM2_Dual2Disable(void) { TIMx_Dual2Disable(TIM2); }
INLINE void TIM3_Dual2Disable(void) { TIMx_Dual2Disable(TIM3); }
INLINE void TIM4_Dual2Disable(void) { TIMx_Dual2Disable(TIM4); }
INLINE void TIM5_Dual2Disable(void) { TIMx_Dual2Disable(TIM5); }
INLINE void TIM6_Dual2Disable(void) { TIMx_Dual2Disable(TIM6); }
INLINE void TIM7_Dual2Disable(void) { TIMx_Dual2Disable(TIM7); }
INLINE void TIM8_Dual2Disable(void) { TIMx_Dual2Disable(TIM8); }
INLINE void TIM9_Dual2Disable(void) { TIMx_Dual2Disable(TIM9); }
INLINE void TIM10_Dual2Disable(void) { TIMx_Dual2Disable(TIM10); }

// Enable/Disable dual-edge for channel 3 (default disable; register AUX.CAP_ED_CH3)
INLINE void TIMx_Dual3Enable(TIM_t* tim) { tim->AUX |= B1; }
INLINE void TIM1_Dual3Enable(void) { TIMx_Dual3Enable(TIM1); }
INLINE void TIM2_Dual3Enable(void) { TIMx_Dual3Enable(TIM2); }
INLINE void TIM3_Dual3Enable(void) { TIMx_Dual3Enable(TIM3); }
INLINE void TIM4_Dual3Enable(void) { TIMx_Dual3Enable(TIM4); }
INLINE void TIM5_Dual3Enable(void) { TIMx_Dual3Enable(TIM5); }
INLINE void TIM6_Dual3Enable(void) { TIMx_Dual3Enable(TIM6); }
INLINE void TIM7_Dual3Enable(void) { TIMx_Dual3Enable(TIM7); }
INLINE void TIM8_Dual3Enable(void) { TIMx_Dual3Enable(TIM8); }
INLINE void TIM9_Dual3Enable(void) { TIMx_Dual3Enable(TIM9); }
INLINE void TIM10_Dual3Enable(void) { TIMx_Dual3Enable(TIM10); }

INLINE void TIMx_Dual3Disable(TIM_t* tim) { tim->AUX &= ~B1; }
INLINE void TIM1_Dual3Disable(void) { TIMx_Dual3Disable(TIM1); }
INLINE void TIM2_Dual3Disable(void) { TIMx_Dual3Disable(TIM2); }
INLINE void TIM3_Dual3Disable(void) { TIMx_Dual3Disable(TIM3); }
INLINE void TIM4_Dual3Disable(void) { TIMx_Dual3Disable(TIM4); }
INLINE void TIM5_Dual3Disable(void) { TIMx_Dual3Disable(TIM5); }
INLINE void TIM6_Dual3Disable(void) { TIMx_Dual3Disable(TIM6); }
INLINE void TIM7_Dual3Disable(void) { TIMx_Dual3Disable(TIM7); }
INLINE void TIM8_Dual3Disable(void) { TIMx_Dual3Disable(TIM8); }
INLINE void TIM9_Dual3Disable(void) { TIMx_Dual3Disable(TIM9); }
INLINE void TIM10_Dual3Disable(void) { TIMx_Dual3Disable(TIM10); }

// Enable/Disable dual-edge for channel 4 (default disable; register AUX.CAP_ED_CH4)
INLINE void TIMx_Dual4Enable(TIM_t* tim) { tim->AUX |= B2; }
INLINE void TIM1_Dual4Enable(void) { TIMx_Dual4Enable(TIM1); }
INLINE void TIM2_Dual4Enable(void) { TIMx_Dual4Enable(TIM2); }
INLINE void TIM3_Dual4Enable(void) { TIMx_Dual4Enable(TIM3); }
INLINE void TIM4_Dual4Enable(void) { TIMx_Dual4Enable(TIM4); }
INLINE void TIM5_Dual4Enable(void) { TIMx_Dual4Enable(TIM5); }
INLINE void TIM6_Dual4Enable(void) { TIMx_Dual4Enable(TIM6); }
INLINE void TIM7_Dual4Enable(void) { TIMx_Dual4Enable(TIM7); }
INLINE void TIM8_Dual4Enable(void) { TIMx_Dual4Enable(TIM8); }
INLINE void TIM9_Dual4Enable(void) { TIMx_Dual4Enable(TIM9); }
INLINE void TIM10_Dual4Enable(void) { TIMx_Dual4Enable(TIM10); }

INLINE void TIMx_Dual4Disable(TIM_t* tim) { tim->AUX &= ~B2; }
INLINE void TIM1_Dual4Disable(void) { TIMx_Dual4Disable(TIM1); }
INLINE void TIM2_Dual4Disable(void) { TIMx_Dual4Disable(TIM2); }
INLINE void TIM3_Dual4Disable(void) { TIMx_Dual4Disable(TIM3); }
INLINE void TIM4_Dual4Disable(void) { TIMx_Dual4Disable(TIM4); }
INLINE void TIM5_Dual4Disable(void) { TIMx_Dual4Disable(TIM5); }
INLINE void TIM6_Dual4Disable(void) { TIMx_Dual4Disable(TIM6); }
INLINE void TIM7_Dual4Disable(void) { TIMx_Dual4Disable(TIM7); }
INLINE void TIM8_Dual4Disable(void) { TIMx_Dual4Disable(TIM8); }
INLINE void TIM9_Dual4Disable(void) { TIMx_Dual4Disable(TIM9); }
INLINE void TIM10_Dual4Disable(void) { TIMx_Dual4Disable(TIM10); }

// === Setup

// Reset timer to default setup
void TIMx_Reset(TIM_t* tim);
INLINE void TIM1_Reset(void) { RCC_TIM1Reset(); }
INLINE void TIM2_Reset(void) { RCC_TIM2Reset(); }
INLINE void TIM3_Reset(void) { RCC_TIM3Reset(); }
INLINE void TIM4_Reset(void) { RCC_TIM4Reset(); }
INLINE void TIM5_Reset(void) { RCC_TIM5Reset(); }
INLINE void TIM6_Reset(void) { RCC_TIM6Reset(); }
INLINE void TIM7_Reset(void) { RCC_TIM7Reset(); }
INLINE void TIM8_Reset(void) { RCC_TIM8Reset(); }
INLINE void TIM9_Reset(void) { RCC_TIM9Reset(); }
INLINE void TIM10_Reset(void) { RCC_TIM10Reset(); }

// Enable timer clock source
void TIMx_ClkEnable(TIM_t* tim);
INLINE void TIM1_ClkEnable(void) { RCC_TIM1ClkEnable(); }
INLINE void TIM2_ClkEnable(void) { RCC_TIM2ClkEnable(); }
INLINE void TIM3_ClkEnable(void) { RCC_TIM3ClkEnable(); }
INLINE void TIM4_ClkEnable(void) { RCC_TIM4ClkEnable(); }
INLINE void TIM5_ClkEnable(void) { RCC_TIM5ClkEnable(); }
INLINE void TIM6_ClkEnable(void) { RCC_TIM6ClkEnable(); }
INLINE void TIM7_ClkEnable(void) { RCC_TIM7ClkEnable(); }
INLINE void TIM8_ClkEnable(void) { RCC_TIM8ClkEnable(); }
INLINE void TIM9_ClkEnable(void) { RCC_TIM9ClkEnable(); }
INLINE void TIM10_ClkEnable(void) { RCC_TIM10ClkEnable(); }

// Initialize simple timer with interrupt
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
// You also need to do:
// - Enable IRQ interrupt with NVIC_IRQEnable(IRQ_TIMx_UP).
// - Create interrupt service "HANDLER void TIMx_UP_IRQHandler()".
// - In interrupt service clear interrupt flag TIMx_UpIntClr().
void TIMx_InitInt(TIM_t* tim, int div, int reload);
INLINE void TIM1_InitInt(int div, int reload) { TIMx_InitInt(TIM1, div, reload); }
INLINE void TIM2_InitInt(int div, int reload) { TIMx_InitInt(TIM2, div, reload); }
INLINE void TIM3_InitInt(int div, int reload) { TIMx_InitInt(TIM3, div, reload); }
INLINE void TIM4_InitInt(int div, int reload) { TIMx_InitInt(TIM4, div, reload); }
INLINE void TIM5_InitInt(int div, int reload) { TIMx_InitInt(TIM5, div, reload); }
INLINE void TIM6_InitInt(int div, int reload) { TIMx_InitInt(TIM6, div, reload); }
INLINE void TIM7_InitInt(int div, int reload) { TIMx_InitInt(TIM7, div, reload); }
INLINE void TIM8_InitInt(int div, int reload) { TIMx_InitInt(TIM8, div, reload); }
INLINE void TIM9_InitInt(int div, int reload) { TIMx_InitInt(TIM9, div, reload); }
INLINE void TIM10_InitInt(int div, int reload) { TIMx_InitInt(TIM10, div, reload); }

// Initialize simple PWM output from channel
//   chan ... channel 1..4
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
//   comp ... compare value 0..reload
//   high ... direction HIGH->LOW (or LOW->HIGH otherwise)
// You also need to do:
// - See GPIO_Remap_TIMx() function to which pin the channel is mapped.
// - Set pin function to GPIO_Mode(pin, GPIO_MODE_AF).
void TIMx_InitPWM(TIM_t* tim, int chan, int div, int reload, int comp, Bool high);
INLINE void TIM1_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM1, chan, div, reload, comp, high); }
INLINE void TIM2_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM2, chan, div, reload, comp, high); }
INLINE void TIM3_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM3, chan, div, reload, comp, high); }
INLINE void TIM4_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM4, chan, div, reload, comp, high); }
INLINE void TIM5_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM5, chan, div, reload, comp, high); }
INLINE void TIM6_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM6, chan, div, reload, comp, high); }
INLINE void TIM7_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM7, chan, div, reload, comp, high); }
INLINE void TIM8_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM8, chan, div, reload, comp, high); }
INLINE void TIM9_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM9, chan, div, reload, comp, high); }
INLINE void TIM10_InitPWM(int chan, int div, int reload, int comp, Bool high) { TIMx_InitPWM(TIM10, chan, div, reload, comp, high); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_TIM_H

#endif // USE_TIM
