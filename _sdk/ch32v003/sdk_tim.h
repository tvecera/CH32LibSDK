
// ****************************************************************************
//
//                                Timer
//
// ****************************************************************************
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
} TIM_t;
STATIC_ASSERT(sizeof(TIM_t) == 0x50, "Incorrect TIM_t!");
#define TIM1	((TIM_t*)TIM1_BASE)	// 0x40012C00
#define TIM2	((TIM_t*)TIM2_BASE)	// 0x40000000

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

INLINE void TIMx_Disable(TIM_t* tim) { tim->CTLR1 &= ~B0; }
INLINE void TIM1_Disable(void) { TIMx_Disable(TIM1); }
INLINE void TIM2_Disable(void) { TIMx_Disable(TIM2); }

// Enable/Disable generation of UEV update events (default enabled; register CTLR1.UDIS)
INLINE void TIMx_UEVEnable(TIM_t* tim) { tim->CTLR1 &= ~B1; }
INLINE void TIM1_UEVEnable(void) { TIMx_UEVEnable(TIM1); }
INLINE void TIM2_UEVEnable(void) { TIMx_UEVEnable(TIM2); }

INLINE void TIMx_UEVDisable(TIM_t* tim) { tim->CTLR1 |= B1; }
INLINE void TIM1_UEVDisable(void) { TIMx_UEVDisable(TIM1); }
INLINE void TIM2_UEVDisable(void) { TIMx_UEVDisable(TIM2); }

// Select UEV update source to all events or to overflow only (default all events; register CTLR1.URS)
INLINE void TIMx_UEVSrcAll(TIM_t* tim) { tim->CTLR1 &= ~B2; }
INLINE void TIM1_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM1); }
INLINE void TIM2_UEVSrcAll(void) { TIMx_UEVSrcAll(TIM2); }

INLINE void TIMx_UEVSrcOver(TIM_t* tim) { tim->CTLR1 |= B2; }
INLINE void TIM1_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM1); }
INLINE void TIM2_UEVSrcOver(void) { TIMx_UEVSrcOver(TIM2); }

// Select pulse mode (single run) or repeating mode (default repeating mode; register CTLR1.OPM)
INLINE void TIMx_PulseMode(TIM_t* tim) { tim->CTLR1 |= B3; }
INLINE void TIM1_PulseMode(void) { TIMx_PulseMode(TIM1); }
INLINE void TIM2_PulseMode(void) { TIMx_PulseMode(TIM2); }

INLINE void TIMx_RepeatMode(TIM_t* tim) { tim->CTLR1 &= ~B3; }
INLINE void TIM1_RepeatMode(void) { TIMx_RepeatMode(TIM1); }
INLINE void TIM2_RepeatMode(void) { TIMx_RepeatMode(TIM2); }

// Select counting direction up or down (default up direction; register CTLR1.DIR)
INLINE void TIMx_DirUp(TIM_t* tim) { tim->CTLR1 &= ~B4; }
INLINE void TIM1_DirUp(void) { TIMx_DirUp(TIM1); }
INLINE void TIM2_DirUp(void) { TIMx_DirUp(TIM2); }

INLINE void TIMx_DirDown(TIM_t* tim) { tim->CTLR1 |= B4; }
INLINE void TIM1_DirDown(void) { TIMx_DirDown(TIM1); }
INLINE void TIM2_DirDown(void) { TIMx_DirDown(TIM2); }

// Set alignment mode selection TIM_ALIGN_* (default edge alignment; register CTLR1.CMS[1:0])
INLINE void TIMx_Align(TIM_t* tim, int align) { tim->CTLR1 = (tim->CTLR1 & ~(3<<5)) | (align<<5); }
INLINE void TIM1_Align(int align) { TIMx_Align(TIM1, align); }
INLINE void TIM2_Align(int align) { TIMx_Align(TIM2, align); }

// Enable/Disable auto-reload (default disabled; register CTLR1.ARPE)
INLINE void TIMx_AutoReloadEnable(TIM_t* tim) { tim->CTLR1 |= B7; }
INLINE void TIM1_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM1); }
INLINE void TIM2_AutoReloadEnable(void) { TIMx_AutoReloadEnable(TIM2); }

INLINE void TIMx_AutoReloadDisable(TIM_t* tim) { tim->CTLR1 &= ~B7; }
INLINE void TIM1_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM1); }
INLINE void TIM2_AutoReloadDisable(void) { TIMx_AutoReloadDisable(TIM2); }

// Set clock division ratio of dead time generator TIM_CKD_* (default no division; register CTLR1.CKD[1:0])
INLINE void TIMx_CKDDiv(TIM_t* tim, int div) { tim->CTLR1 = (tim->CTLR1 & ~(3<<8)) | (div<<8); }
INLINE void TIM1_CKDDiv(int div) { TIMx_CKDDiv(TIM1, div); }
INLINE void TIM2_CKDDiv(int div) { TIMx_CKDDiv(TIM2, div); }

// Select capture value to current counter or to 0xffff when counter overflow (default current counter; register CTLR1.CAPOV)
INLINE void TIMx_CapCnt(TIM_t* tim) { tim->CTLR1 &= ~B14; }
INLINE void TIM1_CapCnt(void) { TIMx_CapCnt(TIM1); }
INLINE void TIM2_CapCnt(void) { TIMx_CapCnt(TIM2); }

INLINE void TIMx_CapOver(TIM_t* tim) { tim->CTLR1 |= B14; }
INLINE void TIM1_CapOver(void) { TIMx_CapOver(TIM1); }
INLINE void TIM2_CapOver(void) { TIMx_CapOver(TIM2); }

// Enable/Disable capture level indication function (default disabled; register CTLR1.CAPLVL)
INLINE void TIMx_CapLvlEnable(TIM_t* tim) { tim->CTLR1 |= B15; }
INLINE void TIM1_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM1); }
INLINE void TIM2_CapLvlEnable(void) { TIMx_CapLvlEnable(TIM2); }

INLINE void TIMx_CapLvlDisable(TIM_t* tim) { tim->CTLR1 &= ~B15; }
INLINE void TIM1_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM1); }
INLINE void TIM2_CapLvlDisable(void) { TIMx_CapLvlDisable(TIM2); }

// Enable/Disable preloading compare capture bits CCxE, CCxNE and OCxM (default disabled; register CTLR2.CCPC)
// - Only for channels with complementary outputs
INLINE void TIMx_CCPreEnable(TIM_t* tim) { tim->CTLR2 |= B0; }
INLINE void TIM1_CCPreEnable(void) { TIMx_CCPreEnable(TIM1); }
INLINE void TIM2_CCPreEnable(void) { TIMx_CCPreEnable(TIM2); }

INLINE void TIMx_CCPreDisable(TIM_t* tim) { tim->CTLR2 &= ~B0; }
INLINE void TIM1_CCPreDisable(void) { TIMx_CCPreDisable(TIM1); }
INLINE void TIM2_CCPreDisable(void) { TIMx_CCPreDisable(TIM2); }

// Enable/Disable update compare capture also with rising edge on TRGI (default disabled; register CTLR2.CCUS)
// - Only for channels with complementary outputs
INLINE void TIMx_CCTrgiEnable(TIM_t* tim) { tim->CTLR2 |= B2; }
INLINE void TIM1_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM1); }
INLINE void TIM2_CCTrgiEnable(void) { TIMx_CCTrgiEnable(TIM2); }

INLINE void TIMx_CCTrgiDisable(TIM_t* tim) { tim->CTLR2 &= ~B2; }
INLINE void TIM1_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM1); }
INLINE void TIM2_CCTrgiDisable(void) { TIMx_CCTrgiDisable(TIM2); }

// Select capture DMA request on update or on compare (default compare; register CTLR2.CCDS)
INLINE void TIMx_CCDMAOnUpdate(TIM_t* tim) { tim->CTLR2 |= B3; }
INLINE void TIM1_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM1); }
INLINE void TIM2_CCDMAOnUpdate(void) { TIMx_CCDMAOnUpdate(TIM2); }

INLINE void TIMx_CCDMAOnComp(TIM_t* tim) { tim->CTLR2 &= ~B3; }
INLINE void TIM1_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM1); }
INLINE void TIM2_CCDMAOnComp(void) { TIMx_CCDMAOnComp(TIM2); }

// Select master to slave synchronization mode TIM_MSSYNC_* (select trigger output signal; default reset; register CTLR2.MMS[2:0])
INLINE void TIMx_MSSync(TIM_t* tim, int sync) { tim->CTLR2 = (tim->CTLR1 & ~(7<<4)) | (sync<<4); }
INLINE void TIM1_MSSync(int sync) { TIMx_MSSync(TIM1, sync); }
INLINE void TIM2_MSSync(int sync) { TIMx_MSSync(TIM2, sync); }

// Select TI1 input to CH1 or to heterodyn of CH1/CH2/CH3 (default CH1; register CTLR2.TI1S)
INLINE void TIMx_TI1CH1(TIM_t* tim) { tim->CTLR2 &= ~B7; }
INLINE void TIM1_TI1CH1(void) { TIMx_TI1CH1(TIM1); }
INLINE void TIM2_TI1CH1(void) { TIMx_TI1CH1(TIM2); }

INLINE void TIMx_TI1CH123(TIM_t* tim) { tim->CTLR2 |= B7; }
INLINE void TIM1_TI1CH123(void) { TIMx_TI1CH123(TIM1); }
INLINE void TIM2_TI1CH123(void) { TIMx_TI1CH123(TIM2); }

// Select output idle state of OC1 after deadband to 0 or 1 (default 0; register CTLR2.OIS1)
INLINE void TIMx_OI1_0(TIM_t* tim) { tim->CTLR2 &= ~B8; }
INLINE void TIM1_OI1_0(void) { TIMx_OI1_0(TIM1); }
INLINE void TIM2_OI1_0(void) { TIMx_OI1_0(TIM2); }

INLINE void TIMx_OI1_1(TIM_t* tim) { tim->CTLR2 |= B8; }
INLINE void TIM1_OI1_1(void) { TIMx_OI1_1(TIM1); }
INLINE void TIM2_OI1_1(void) { TIMx_OI1_1(TIM2); }

// Select output idle state of OC1N after deadband to 0 or 1 (default 0; register CTLR2.OIS1N)
INLINE void TIMx_OI1N_0(TIM_t* tim) { tim->CTLR2 &= ~B9; }
INLINE void TIM1_OI1N_0(void) { TIMx_OI1N_0(TIM1); }
INLINE void TIM2_OI1N_0(void) { TIMx_OI1N_0(TIM2); }

INLINE void TIMx_OI1N_1(TIM_t* tim) { tim->CTLR2 |= B9; }
INLINE void TIM1_OI1N_1(void) { TIMx_OI1N_1(TIM1); }
INLINE void TIM2_OI1N_1(void) { TIMx_OI1N_1(TIM2); }

// Select output idle state of OC2 after deadband to 0 or 1 (default 0; register CTLR2.OIS2)
INLINE void TIMx_OI2_0(TIM_t* tim) { tim->CTLR2 &= ~B10; }
INLINE void TIM1_OI2_0(void) { TIMx_OI2_0(TIM1); }
INLINE void TIM2_OI2_0(void) { TIMx_OI2_0(TIM2); }

INLINE void TIMx_OI2_1(TIM_t* tim) { tim->CTLR2 |= B10; }
INLINE void TIM1_OI2_1(void) { TIMx_OI2_1(TIM1); }
INLINE void TIM2_OI2_1(void) { TIMx_OI2_1(TIM2); }

// Select output idle state of OC2N after deadband to 0 or 1 (default 0; register CTLR2.OIS2N)
INLINE void TIMx_OI2N_0(TIM_t* tim) { tim->CTLR2 &= ~B11; }
INLINE void TIM1_OI2N_0(void) { TIMx_OI2N_0(TIM1); }
INLINE void TIM2_OI2N_0(void) { TIMx_OI2N_0(TIM2); }

INLINE void TIMx_OI2N_1(TIM_t* tim) { tim->CTLR2 |= B11; }
INLINE void TIM1_OI2N_1(void) { TIMx_OI2N_1(TIM1); }
INLINE void TIM2_OI2N_1(void) { TIMx_OI2N_1(TIM2); }

// Select output idle state of OC3 after deadband to 0 or 1 (default 0; register CTLR2.OIS3)
INLINE void TIMx_OI3_0(TIM_t* tim) { tim->CTLR2 &= ~B12; }
INLINE void TIM1_OI3_0(void) { TIMx_OI3_0(TIM1); }
INLINE void TIM2_OI3_0(void) { TIMx_OI3_0(TIM2); }

INLINE void TIMx_OI3_1(TIM_t* tim) { tim->CTLR2 |= B12; }
INLINE void TIM1_OI3_1(void) { TIMx_OI3_1(TIM1); }
INLINE void TIM2_OI3_1(void) { TIMx_OI3_1(TIM2); }

// Select output idle state of OC3N after deadband to 0 or 1 (default 0; register CTLR2.OIS3N)
INLINE void TIMx_OI3N_0(TIM_t* tim) { tim->CTLR2 &= ~B13; }
INLINE void TIM1_OI3N_0(void) { TIMx_OI3N_0(TIM1); }
INLINE void TIM2_OI3N_0(void) { TIMx_OI3N_0(TIM2); }

INLINE void TIMx_OI3N_1(TIM_t* tim) { tim->CTLR2 |= B13; }
INLINE void TIM1_OI3N_1(void) { TIMx_OI3N_1(TIM1); }
INLINE void TIM2_OI3N_1(void) { TIMx_OI3N_1(TIM2); }

// Select output idle state of OC4 after deadband to 0 or 1 (default 0; register CTLR2.OIS4)
INLINE void TIMx_OI4_0(TIM_t* tim) { tim->CTLR2 &= ~B14; }
INLINE void TIM1_OI4_0(void) { TIMx_OI4_0(TIM1); }
INLINE void TIM2_OI4_0(void) { TIMx_OI4_0(TIM2); }

INLINE void TIMx_OI4_1(TIM_t* tim) { tim->CTLR2 |= B14; }
INLINE void TIM1_OI4_1(void) { TIMx_OI4_1(TIM1); }
INLINE void TIM2_OI4_1(void) { TIMx_OI4_1(TIM2); }

// === Slave mode

// Select input mode TIM_INMODE_* (default internal clock CK_INT; register SMCFGR.SMS[2:0])
INLINE void TIMx_InMode(TIM_t* tim, int inmode) { tim->SMCFGR = (tim->SMCFGR & ~7) | inmode; }
INLINE void TIM1_InMode(int inmode) { TIMx_InMode(TIM1, inmode); }
INLINE void TIM2_InMode(int inmode) { TIMx_InMode(TIM2, inmode); }

// Select trigger selection TIM_TRIG_* (input mode must be internal clock; default internal trigger 0 ITR0; register SMCFGR.TS[2:0])
INLINE void TIMx_Trig(TIM_t* tim, int trig) { tim->SMCFGR = (tim->SMCFGR & ~(7<<4)) | (trig<<4); }
INLINE void TIM1_Trig(int trig) { TIMx_Trig(TIM1, trig); }
INLINE void TIM2_Trig(int trig) { TIMx_Trig(TIM2, trig); }

// Enable/Disable delay event of trigger input TRGI (default disabled; register SMCFGR.MSM)
INLINE void TIMx_TRGIDelayEnable(TIM_t* tim) { tim->SMCFGR |= B7; }
INLINE void TIM1_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM1); }
INLINE void TIM2_TRGIDelayEnable(void) { TIMx_TRGIDelayEnable(TIM2); }

INLINE void TIMx_TRGIDelayDisable(TIM_t* tim) { tim->SMCFGR &= ~B7; }
INLINE void TIM1_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM1); }
INLINE void TIM2_TRGIDelayDisable(void) { TIMx_TRGIDelayDisable(TIM2); }

// Select external filtering TIM_FILTER_* (record up to N events; default 0; register SMCFGR.ETF[3:0])
INLINE void TIMx_Filter(TIM_t* tim, int filt) { tim->SMCFGR = (tim->SMCFGR & ~(0x0f<<8)) | (filt<<8); }
INLINE void TIM1_Filter(int filt) { TIMx_Filter(TIM1, filt); }
INLINE void TIM2_Filter(int filt) { TIMx_Filter(TIM2, filt); }

// Set prescaler of external trigger signal TIM_ETRPDIV_* (default off; register SMCFGR.ETPS[1:0])
INLINE void TIMx_ETRPDiv(TIM_t* tim, int div) { tim->SMCFGR = (tim->SMCFGR & ~(3<<12)) | (div<<12); }
INLINE void TIM1_ETRPDiv(int div) { TIMx_ETRPDiv(TIM1, div); }
INLINE void TIM2_ETRPDiv(int div) { TIMx_ETRPDiv(TIM2, div); }

// Enable/Disable external clock mode 2 (default disabled; register SMCFGR.ECE)
INLINE void TIMx_EC2Enable(TIM_t* tim) { tim->SMCFGR |= B14; }
INLINE void TIM1_EC2Enable(void) { TIMx_EC2Enable(TIM1); }
INLINE void TIM2_EC2Enable(void) { TIMx_EC2Enable(TIM2); }

INLINE void TIMx_EC2Disable(TIM_t* tim) { tim->SMCFGR &= ~B14; }
INLINE void TIM1_EC2Disable(void) { TIMx_EC2Disable(TIM1); }
INLINE void TIM2_EC2Disable(void) { TIMx_EC2Disable(TIM2); }

// Select ETR trigger polarity to HIGH/Rising or LOW/Falling (default HIGH/Rising; register SMCFGR.ETP)
INLINE void TIMx_ETRHighRise(TIM_t* tim) { tim->SMCFGR &= ~B15; }
INLINE void TIM1_ETRHighRise(void) { TIMx_ETRHighRise(TIM1); }
INLINE void TIM2_ETRHighRise(void) { TIMx_ETRHighRise(TIM2); }

INLINE void TIMx_ETRLowFall(TIM_t* tim) { tim->SMCFGR |= B15; }
INLINE void TIM1_ETRLowFall(void) { TIMx_ETRLowFall(TIM1); }
INLINE void TIM2_ETRLowFall(void) { TIMx_ETRLowFall(TIM2); }

// === DMA and Interrupt

// Enable/Disable update interrupt (default disabled; register DMAINTENR.UIE)
INLINE void TIMx_UpIntEnable(TIM_t* tim) { tim->DMAINTENR |= B0; }
INLINE void TIM1_UpIntEnable(void) { TIMx_UpIntEnable(TIM1); }
INLINE void TIM2_UpIntEnable(void) { TIMx_UpIntEnable(TIM2); }

INLINE void TIMx_UpIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B0; }
INLINE void TIM1_UpIntDisable(void) { TIMx_UpIntDisable(TIM1); }
INLINE void TIM2_UpIntDisable(void) { TIMx_UpIntDisable(TIM2); }

// Check/Clear update interrupt flag (register INTFR.UIF)
INLINE Bool TIMx_UpInt(TIM_t* tim) { return (tim->INTFR & B0) != 0; }
INLINE Bool TIM1_UpInt(void) { return TIMx_UpInt(TIM1); }
INLINE Bool TIM2_UpInt(void) { return TIMx_UpInt(TIM2); }

INLINE void TIMx_UpIntClr(TIM_t* tim) { tim->INTFR &= ~B0; }
INLINE void TIM1_UpIntClr(void) { TIMx_UpIntClr(TIM1); }
INLINE void TIM2_UpIntClr(void) { TIMx_UpIntClr(TIM2); }

// Enable/Disable compare capture channel 1 interrupt (default disabled; register DMAINTENR.CC1IE)
INLINE void TIMx_CC1IntEnable(TIM_t* tim) { tim->DMAINTENR |= B1; }
INLINE void TIM1_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM1); }
INLINE void TIM2_CC1IntEnable(void) { TIMx_CC1IntEnable(TIM2); }

INLINE void TIMx_CC1IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B1; }
INLINE void TIM1_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM1); }
INLINE void TIM2_CC1IntDisable(void) { TIMx_CC1IntDisable(TIM2); }

// Check/Clear compare capture channel 1 interrupt flag (register INTFR.CC1IF)
INLINE Bool TIMx_CC1Int(TIM_t* tim) { return (tim->INTFR & B1) != 0; }
INLINE Bool TIM1_CC1Int(void) { return TIMx_CC1Int(TIM1); }
INLINE Bool TIM2_CC1Int(void) { return TIMx_CC1Int(TIM2); }

INLINE void TIMx_CC1IntClr(TIM_t* tim) { tim->INTFR &= ~B1; }
INLINE void TIM1_CC1IntClr(void) { TIMx_CC1IntClr(TIM1); }
INLINE void TIM2_CC1IntClr(void) { TIMx_CC1IntClr(TIM2); }

// Enable/Disable compare capture channel 2 interrupt (default disabled; register DMAINTENR.CC2IE)
INLINE void TIMx_CC2IntEnable(TIM_t* tim) { tim->DMAINTENR |= B2; }
INLINE void TIM1_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM1); }
INLINE void TIM2_CC2IntEnable(void) { TIMx_CC2IntEnable(TIM2); }

INLINE void TIMx_CC2IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B2; }
INLINE void TIM1_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM1); }
INLINE void TIM2_CC2IntDisable(void) { TIMx_CC2IntDisable(TIM2); }

// Check/Clear compare capture channel 2 interrupt flag (register INTFR.CC2IF)
INLINE Bool TIMx_CC2Int(TIM_t* tim) { return (tim->INTFR & B2) != 0; }
INLINE Bool TIM1_CC2Int(void) { return TIMx_CC2Int(TIM1); }
INLINE Bool TIM2_CC2Int(void) { return TIMx_CC2Int(TIM2); }

INLINE void TIMx_CC2IntClr(TIM_t* tim) { tim->INTFR &= ~B2; }
INLINE void TIM1_CC2IntClr(void) { TIMx_CC2IntClr(TIM1); }
INLINE void TIM2_CC2IntClr(void) { TIMx_CC2IntClr(TIM2); }

// Enable/Disable compare capture channel 3 interrupt (default disabled; register DMAINTENR.CC3IE)
INLINE void TIMx_CC3IntEnable(TIM_t* tim) { tim->DMAINTENR |= B3; }
INLINE void TIM1_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM1); }
INLINE void TIM2_CC3IntEnable(void) { TIMx_CC3IntEnable(TIM2); }

INLINE void TIMx_CC3IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B3; }
INLINE void TIM1_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM1); }
INLINE void TIM2_CC3IntDisable(void) { TIMx_CC3IntDisable(TIM2); }

// Check/Clear compare capture channel 3 interrupt flag (register INTFR.CC3IF)
INLINE Bool TIMx_CC3Int(TIM_t* tim) { return (tim->INTFR & B3) != 0; }
INLINE Bool TIM1_CC3Int(void) { return TIMx_CC3Int(TIM1); }
INLINE Bool TIM2_CC3Int(void) { return TIMx_CC3Int(TIM2); }

INLINE void TIMx_CC3IntClr(TIM_t* tim) { tim->INTFR &= ~B3; }
INLINE void TIM1_CC3IntClr(void) { TIMx_CC3IntClr(TIM1); }
INLINE void TIM2_CC3IntClr(void) { TIMx_CC3IntClr(TIM2); }

// Enable/Disable compare capture channel 4 interrupt (default disabled; register DMAINTENR.CC4IE)
INLINE void TIMx_CC4IntEnable(TIM_t* tim) { tim->DMAINTENR |= B4; }
INLINE void TIM1_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM1); }
INLINE void TIM2_CC4IntEnable(void) { TIMx_CC4IntEnable(TIM2); }

INLINE void TIMx_CC4IntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B4; }
INLINE void TIM1_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM1); }
INLINE void TIM2_CC4IntDisable(void) { TIMx_CC4IntDisable(TIM2); }

// Check/Clear compare capture channel 4 interrupt flag (register INTFR.CC4IF)
INLINE Bool TIMx_CC4Int(TIM_t* tim) { return (tim->INTFR & B4) != 0; }
INLINE Bool TIM1_CC4Int(void) { return TIMx_CC4Int(TIM1); }
INLINE Bool TIM2_CC4Int(void) { return TIMx_CC4Int(TIM2); }

INLINE void TIMx_CC4IntClr(TIM_t* tim) { tim->INTFR &= ~B4; }
INLINE void TIM1_CC4IntClr(void) { TIMx_CC4IntClr(TIM1); }
INLINE void TIM2_CC4IntClr(void) { TIMx_CC4IntClr(TIM2); }

// Enable/Disable COM interrupt (default disabled; register DMAINTENR.COMIE)
INLINE void TIMx_COMIntEnable(TIM_t* tim) { tim->DMAINTENR |= B5; }
INLINE void TIM1_COMIntEnable(void) { TIMx_COMIntEnable(TIM1); }
INLINE void TIM2_COMIntEnable(void) { TIMx_COMIntEnable(TIM2); }

INLINE void TIMx_COMIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B5; }
INLINE void TIM1_COMIntDisable(void) { TIMx_COMIntDisable(TIM1); }
INLINE void TIM2_COMIntDisable(void) { TIMx_COMIntDisable(TIM2); }

// Check/Clear COM interrupt flag (register INTFR.COMIF)
INLINE Bool TIMx_COMInt(TIM_t* tim) { return (tim->INTFR & B5) != 0; }
INLINE Bool TIM1_COMInt(void) { return TIMx_COMInt(TIM1); }
INLINE Bool TIM2_COMInt(void) { return TIMx_COMInt(TIM2); }

INLINE void TIMx_COMIntClr(TIM_t* tim) { tim->INTFR &= ~B5; }
INLINE void TIM1_COMIntClr(void) { TIMx_COMIntClr(TIM1); }
INLINE void TIM2_COMIntClr(void) { TIMx_COMIntClr(TIM2); }

// Enable/Disable trigger interrupt (default disabled; register DMAINTENR.TIE)
INLINE void TIMx_TRGIntEnable(TIM_t* tim) { tim->DMAINTENR |= B6; }
INLINE void TIM1_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM1); }
INLINE void TIM2_TRGIntEnable(void) { TIMx_TRGIntEnable(TIM2); }

INLINE void TIMx_TRGIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B6; }
INLINE void TIM1_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM1); }
INLINE void TIM2_TRGIntDisable(void) { TIMx_TRGIntDisable(TIM2); }

// Check/Clear TRG interrupt flag (register INTFR.TIF)
INLINE Bool TIMx_TRGInt(TIM_t* tim) { return (tim->INTFR & B6) != 0; }
INLINE Bool TIM1_TRGInt(void) { return TIMx_TRGInt(TIM1); }
INLINE Bool TIM2_TRGInt(void) { return TIMx_TRGInt(TIM2); }

INLINE void TIMx_TRGIntClr(TIM_t* tim) { tim->INTFR &= ~B6; }
INLINE void TIM1_TRGIntClr(void) { TIMx_TRGIntClr(TIM1); }
INLINE void TIM2_TRGIntClr(void) { TIMx_TRGIntClr(TIM2); }

// Enable/Disable break interrupt (default disabled; register DMAINTENR.BIE)
INLINE void TIMx_BrkIntEnable(TIM_t* tim) { tim->DMAINTENR |= B7; }
INLINE void TIM1_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM1); }
INLINE void TIM2_BrkIntEnable(void) { TIMx_BrkIntEnable(TIM2); }

INLINE void TIMx_BrkIntDisable(TIM_t* tim) { tim->DMAINTENR &= ~B7; }
INLINE void TIM1_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM1); }
INLINE void TIM2_BrkIntDisable(void) { TIMx_BrkIntDisable(TIM2); }

// Check/Clear Brk interrupt flag (register INTFR.BIF)
INLINE Bool TIMx_BrkInt(TIM_t* tim) { return (tim->INTFR & B7) != 0; }
INLINE Bool TIM1_BrkInt(void) { return TIMx_BrkInt(TIM1); }
INLINE Bool TIM2_BrkInt(void) { return TIMx_BrkInt(TIM2); }

INLINE void TIMx_BrkIntClr(TIM_t* tim) { tim->INTFR &= ~B7; }
INLINE void TIM1_BrkIntClr(void) { TIMx_BrkIntClr(TIM1); }
INLINE void TIM2_BrkIntClr(void) { TIMx_BrkIntClr(TIM2); }

// Enable/Disable update DMA request (default disabled; register DMAINTENR.UDE)
INLINE void TIMx_UDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B8; }
INLINE void TIM1_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM1); }
INLINE void TIM2_UDMAReqEnable(void) { TIMx_UDMAReqEnable(TIM2); }

INLINE void TIMx_UDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B8; }
INLINE void TIM1_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM1); }
INLINE void TIM2_UDMAReqDisable(void) { TIMx_UDMAReqDisable(TIM2); }

// Enable/Disable compare channel 1 DMA request (default disabled; register DMAINTENR.CC1DE)
INLINE void TIMx_CC1DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B9; }
INLINE void TIM1_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM1); }
INLINE void TIM2_CC1DMAReqEnable(void) { TIMx_CC1DMAReqEnable(TIM2); }

INLINE void TIMx_CC1DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B9; }
INLINE void TIM1_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM1); }
INLINE void TIM2_CC1DMAReqDisable(void) { TIMx_CC1DMAReqDisable(TIM2); }

// Enable/Disable compare channel 2 DMA request (default disabled; register DMAINTENR.CC2DE)
INLINE void TIMx_CC2DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B10; }
INLINE void TIM1_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM1); }
INLINE void TIM2_CC2DMAReqEnable(void) { TIMx_CC2DMAReqEnable(TIM2); }

INLINE void TIMx_CC2DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B10; }
INLINE void TIM1_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM1); }
INLINE void TIM2_CC2DMAReqDisable(void) { TIMx_CC2DMAReqDisable(TIM2); }

// Enable/Disable compare channel 3 DMA request (default disabled; register DMAINTENR.CC3DE)
INLINE void TIMx_CC3DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B11; }
INLINE void TIM1_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM1); }
INLINE void TIM2_CC3DMAReqEnable(void) { TIMx_CC3DMAReqEnable(TIM2); }

INLINE void TIMx_CC3DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B11; }
INLINE void TIM1_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM1); }
INLINE void TIM2_CC3DMAReqDisable(void) { TIMx_CC3DMAReqDisable(TIM2); }

// Enable/Disable compare channel 4 DMA request (default disabled; register DMAINTENR.CC4DE)
INLINE void TIMx_CC4DMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B12; }
INLINE void TIM1_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM1); }
INLINE void TIM2_CC4DMAReqEnable(void) { TIMx_CC4DMAReqEnable(TIM2); }

INLINE void TIMx_CC4DMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B12; }
INLINE void TIM1_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM1); }
INLINE void TIM2_CC4DMAReqDisable(void) { TIMx_CC4DMAReqDisable(TIM2); }

// Enable/Disable COM DMA request (default disabled; register DMAINTENR.COMDE)
INLINE void TIMx_COMDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B13; }
INLINE void TIM1_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM1); }
INLINE void TIM2_COMDMAReqEnable(void) { TIMx_COMDMAReqEnable(TIM2); }

INLINE void TIMx_COMDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B13; }
INLINE void TIM1_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM1); }
INLINE void TIM2_COMDMAReqDisable(void) { TIMx_COMDMAReqDisable(TIM2); }

// Enable/Disable trigger DMA request (default disabled; register DMAINTENR.TDE)
INLINE void TIMx_TRGDMAReqEnable(TIM_t* tim) { tim->DMAINTENR |= B14; }
INLINE void TIM1_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM1); }
INLINE void TIM2_TRGDMAReqEnable(void) { TIMx_TRGDMAReqEnable(TIM2); }

INLINE void TIMx_TRGDMAReqDisable(TIM_t* tim) { tim->DMAINTENR &= ~B14; }
INLINE void TIM1_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM1); }
INLINE void TIM2_TRGDMAReqDisable(void) { TIMx_TRGDMAReqDisable(TIM2); }

// Check/Clear compare capture 1 repeat flag (register INTFR.CC1OF)
INLINE Bool TIMx_CC1Rep(TIM_t* tim) { return (tim->INTFR & B9) != 0; }
INLINE Bool TIM1_CC1Rep(void) { return TIMx_CC1Rep(TIM1); }
INLINE Bool TIM2_CC1Rep(void) { return TIMx_CC1Rep(TIM2); }

INLINE void TIMx_CC1RepClr(TIM_t* tim) { tim->INTFR &= ~B9; }
INLINE void TIM1_CC1RepClr(void) { TIMx_CC1RepClr(TIM1); }
INLINE void TIM2_CC1RepClr(void) { TIMx_CC1RepClr(TIM2); }

// Check/Clear compare capture 2 repeat flag (register INTFR.CC2OF)
INLINE Bool TIMx_CC2Rep(TIM_t* tim) { return (tim->INTFR & B10) != 0; }
INLINE Bool TIM1_CC2Rep(void) { return TIMx_CC2Rep(TIM1); }
INLINE Bool TIM2_CC2Rep(void) { return TIMx_CC2Rep(TIM2); }

INLINE void TIMx_CC2RepClr(TIM_t* tim) { tim->INTFR &= ~B10; }
INLINE void TIM1_CC2RepClr(void) { TIMx_CC2RepClr(TIM1); }
INLINE void TIM2_CC2RepClr(void) { TIMx_CC2RepClr(TIM2); }

// Check/Clear compare capture 3 repeat flag (register INTFR.CC3OF)
INLINE Bool TIMx_CC3Rep(TIM_t* tim) { return (tim->INTFR & B11) != 0; }
INLINE Bool TIM1_CC3Rep(void) { return TIMx_CC3Rep(TIM1); }
INLINE Bool TIM2_CC3Rep(void) { return TIMx_CC3Rep(TIM2); }

INLINE void TIMx_CC3RepClr(TIM_t* tim) { tim->INTFR &= ~B11; }
INLINE void TIM1_CC3RepClr(void) { TIMx_CC3RepClr(TIM1); }
INLINE void TIM2_CC3RepClr(void) { TIMx_CC3RepClr(TIM2); }

// Check/Clear compare capture 4 repeat flag (register INTFR.CC4OF)
INLINE Bool TIMx_CC4Rep(TIM_t* tim) { return (tim->INTFR & B12) != 0; }
INLINE Bool TIM1_CC4Rep(void) { return TIMx_CC4Rep(TIM1); }
INLINE Bool TIM2_CC4Rep(void) { return TIMx_CC4Rep(TIM2); }

INLINE void TIMx_CC4RepClr(TIM_t* tim) { tim->INTFR &= ~B12; }
INLINE void TIM1_CC4RepClr(void) { TIMx_CC4RepClr(TIM1); }
INLINE void TIM2_CC4RepClr(void) { TIMx_CC4RepClr(TIM2); }

// === Generate event

// Generate update event (register SWEVGR.UG)
INLINE void TIMx_Update(TIM_t* tim) { tim->SWEVGR |= B0; }
INLINE void TIM1_Update(void) { TIMx_Update(TIM1); }
INLINE void TIM2_Update(void) { TIMx_Update(TIM2); }

// Generate compare capture 1 event (register SWEVGR.CC1G)
INLINE void TIMx_CC1Event(TIM_t* tim) { tim->SWEVGR |= B1; }
INLINE void TIM1_CC1Event(void) { TIMx_CC1Event(TIM1); }
INLINE void TIM2_CC1Event(void) { TIMx_CC1Event(TIM2); }

// Generate compare capture 2 event (register SWEVGR.CC2G)
INLINE void TIMx_CC2Event(TIM_t* tim) { tim->SWEVGR |= B2; }
INLINE void TIM1_CC2Event(void) { TIMx_CC2Event(TIM1); }
INLINE void TIM2_CC2Event(void) { TIMx_CC2Event(TIM2); }

// Generate compare capture 3 event (register SWEVGR.CC3G)
INLINE void TIMx_CC3Event(TIM_t* tim) { tim->SWEVGR |= B3; }
INLINE void TIM1_CC3Event(void) { TIMx_CC3Event(TIM1); }
INLINE void TIM2_CC3Event(void) { TIMx_CC3Event(TIM2); }

// Generate compare capture 4 event (register SWEVGR.CC4G)
INLINE void TIMx_CC4Event(TIM_t* tim) { tim->SWEVGR |= B4; }
INLINE void TIM1_CC4Event(void) { TIMx_CC4Event(TIM1); }
INLINE void TIM2_CC4Event(void) { TIMx_CC4Event(TIM2); }

// Generate COM update event (register SWEVGR.COMG)
INLINE void TIMx_COMEvent(TIM_t* tim) { tim->SWEVGR |= B5; }
INLINE void TIM1_COMEvent(void) { TIMx_COMEvent(TIM1); }
INLINE void TIM2_COMEvent(void) { TIMx_COMEvent(TIM2); }

// Generate trigger event (register SWEVGR.TG)
INLINE void TIMx_TRGEvent(TIM_t* tim) { tim->SWEVGR |= B6; }
INLINE void TIM1_TRGEvent(void) { TIMx_TRGEvent(TIM1); }
INLINE void TIM2_TRGEvent(void) { TIMx_TRGEvent(TIM2); }

// Generate break event (register SWEVGR.BG)
INLINE void TIMx_BrkEvent(TIM_t* tim) { tim->SWEVGR |= B7; }
INLINE void TIM1_BrkEvent(void) { TIMx_BrkEvent(TIM1); }
INLINE void TIM2_BrkEvent(void) { TIMx_BrkEvent(TIM2); }

// === Compare and capture setup
// Compare: pin direction it output, capture: pin direction is input

// Select input/output mode of compare capture channel 1 TIM_CCSEL_* (default output; register CHCTLR1.CC1S[2:0])
INLINE void TIMx_CC1Sel(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~3) | mode; }
INLINE void TIM1_CC1Sel(int mode) { TIMx_CC1Sel(TIM1, mode); }
INLINE void TIM2_CC1Sel(int mode) { TIMx_CC1Sel(TIM2, mode); }

// Compare mode: Enable/Disable fast compare of channel 1 (default disable; register CHCTLR1.OC1FE)
INLINE void TIMx_OC1FastEnable(TIM_t* tim) { tim->CHCTLR1 |= B2; }
INLINE void TIM1_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM1); }
INLINE void TIM2_OC1FastEnable(void) { TIMx_OC1FastEnable(TIM2); }

INLINE void TIMx_OC1FastDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B2; }
INLINE void TIM1_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM1); }
INLINE void TIM2_OC1FastDisable(void) { TIMx_OC1FastDisable(TIM2); }

// Compare mode: Enable/Disable preload compare register 1 (default disable; register CHCTLR1.OC1PE)
INLINE void TIMx_OC1PreEnable(TIM_t* tim) { tim->CHCTLR1 |= B3; }
INLINE void TIM1_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM1); }
INLINE void TIM2_OC1PreEnable(void) { TIMx_OC1PreEnable(TIM2); }

INLINE void TIMx_OC1PreDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B3; }
INLINE void TIM1_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM1); }
INLINE void TIM2_OC1PreDisable(void) { TIMx_OC1PreDisable(TIM2); }

// Compare mode: Set compare mode of channel 1 TIM_COMP_* (default freeze; register CHCTLR1.OC1M[2:0])
INLINE void TIMx_OC1Mode(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(7<<4)) | (mode<<4); }
INLINE void TIM1_OC1Mode(int mode) { TIMx_OC1Mode(TIM1, mode); }
INLINE void TIM2_OC1Mode(int mode) { TIMx_OC1Mode(TIM2, mode); }

// Compare mode: Enable/Disable compare channel 1 clear by ETRF input (default disabled; register CHCTLR1.OC1CE)
INLINE void TIMx_OC1ClrEnable(TIM_t* tim) { tim->CHCTLR1 |= B7; }
INLINE void TIM1_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM1); }
INLINE void TIM2_OC1ClrEnable(void) { TIMx_OC1ClrEnable(TIM2); }

INLINE void TIMx_OC1ClrDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B7; }
INLINE void TIM1_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM1); }
INLINE void TIM2_OC1ClrDisable(void) { TIMx_OC1ClrDisable(TIM2); }

// Capture mode: Set prescaler of capture channel 1 TIM_ETRPDIV_* (default prescaler off; register CHCTLR1.IC1PSC[1:0])
INLINE void TIMx_IC1Div(TIM_t* tim, int div) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<2)) | (div<<2); }
INLINE void TIM1_IC1Div(int div) { TIMx_IC1Div(TIM1, div); }
INLINE void TIM2_IC1Div(int div) { TIMx_IC1Div(TIM2, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 1 (record up to N events; default 0; register CHCTLR1.IC1F[3:0])
INLINE void TIMx_IC1Filter(TIM_t* tim, int filt) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(0x0f<<4)) | (filt<<4); }
INLINE void TIM1_IC1Filter(int filt) { TIMx_IC1Filter(TIM1, filt); }
INLINE void TIM2_IC1Filter(int filt) { TIMx_IC1Filter(TIM2, filt); }

// Select input/output mode of compare capture channel 2 TIM_CCSEL_* (default output; register CHCTLR1.CCS2S[1:0])
INLINE void TIMx_CC2Sel(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<8)) | (mode<<8); }
INLINE void TIM1_CC2Sel(int mode) { TIMx_CC2Sel(TIM1, mode); }
INLINE void TIM2_CC2Sel(int mode) { TIMx_CC2Sel(TIM2, mode); }

// Compare mode: Enable/Disable fast compare of channel 2 (default disable; register CHCTLR1.OC2FE)
INLINE void TIMx_OC2FastEnable(TIM_t* tim) { tim->CHCTLR1 |= B10; }
INLINE void TIM1_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM1); }
INLINE void TIM2_OC2FastEnable(void) { TIMx_OC2FastEnable(TIM2); }

INLINE void TIMx_OC2FastDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B10; }
INLINE void TIM1_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM1); }
INLINE void TIM2_OC2FastDisable(void) { TIMx_OC2FastDisable(TIM2); }

// Compare mode: Enable/Disable preload compare register 2 (default disable; register CHCTLR1.OC2PE)
INLINE void TIMx_OC2PreEnable(TIM_t* tim) { tim->CHCTLR1 |= B11; }
INLINE void TIM1_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM1); }
INLINE void TIM2_OC2PreEnable(void) { TIMx_OC2PreEnable(TIM2); }

INLINE void TIMx_OC2PreDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B11; }
INLINE void TIM1_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM1); }
INLINE void TIM2_OC2PreDisable(void) { TIMx_OC2PreDisable(TIM2); }

// Compare mode: Set compare mode of channel 2 TIM_COMP_* (default freeze; register CHCTLR1.OC2M[2:0])
INLINE void TIMx_OC2Mode(TIM_t* tim, int mode) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(7<<12)) | (mode<<12); }
INLINE void TIM1_OC2Mode(int mode) { TIMx_OC2Mode(TIM1, mode); }
INLINE void TIM2_OC2Mode(int mode) { TIMx_OC2Mode(TIM2, mode); }

// Compare mode: Enable/Disable compare channel 2 clear by ETRF input (default disabled; register CHCTLR1.OC2CE)
INLINE void TIMx_OC2ClrEnable(TIM_t* tim) { tim->CHCTLR1 |= B15; }
INLINE void TIM1_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM1); }
INLINE void TIM2_OC2ClrEnable(void) { TIMx_OC2ClrEnable(TIM2); }

INLINE void TIMx_OC2ClrDisable(TIM_t* tim) { tim->CHCTLR1 &= ~B15; }
INLINE void TIM1_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM1); }
INLINE void TIM2_OC2ClrDisable(void) { TIMx_OC2ClrDisable(TIM2); }

// Capture mode: Set prescaler of capture channel 2 TIM_ETRPDIV_* (default prescaler off; register CHCTLR1.IC2PSC[1:0])
INLINE void TIMx_IC2Div(TIM_t* tim, int div) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(3<<10)) | (div<<10); }
INLINE void TIM1_IC2Div(int div) { TIMx_IC2Div(TIM1, div); }
INLINE void TIM2_IC2Div(int div) { TIMx_IC2Div(TIM2, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 2 (record up to N events; default 0; register CHCTLR1.IC2F[3:0])
INLINE void TIMx_IC2Filter(TIM_t* tim, int filt) { tim->CHCTLR1 = (tim->CHCTLR1 & ~(0x0f<<12)) | (filt<<12); }
INLINE void TIM1_IC2Filter(int filt) { TIMx_IC2Filter(TIM1, filt); }
INLINE void TIM2_IC2Filter(int filt) { TIMx_IC2Filter(TIM2, filt); }

// Select input/output mode of compare capture channel 3 TIM_CCSEL_* (default output; register CHCTLR2.CC3S[2:0])
INLINE void TIMx_CC3Sel(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~3) | mode; }
INLINE void TIM1_CC3Sel(int mode) { TIMx_CC3Sel(TIM1, mode); }
INLINE void TIM2_CC3Sel(int mode) { TIMx_CC3Sel(TIM2, mode); }

// Compare mode: Enable/Disable fast compare of channel 3 (default disable; register CHCTLR2.OC3FE)
INLINE void TIMx_OC3FastEnable(TIM_t* tim) { tim->CHCTLR2 |= B2; }
INLINE void TIM1_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM1); }
INLINE void TIM2_OC3FastEnable(void) { TIMx_OC3FastEnable(TIM2); }

INLINE void TIMx_OC3FastDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B2; }
INLINE void TIM1_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM1); }
INLINE void TIM2_OC3FastDisable(void) { TIMx_OC3FastDisable(TIM2); }

// Compare mode: Enable/Disable preload compare register 3 (default disable; register CHCTLR2.OC3PE)
INLINE void TIMx_OC3PreEnable(TIM_t* tim) { tim->CHCTLR2 |= B3; }
INLINE void TIM1_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM1); }
INLINE void TIM2_OC3PreEnable(void) { TIMx_OC3PreEnable(TIM2); }

INLINE void TIMx_OC3PreDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B3; }
INLINE void TIM1_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM1); }
INLINE void TIM2_OC3PreDisable(void) { TIMx_OC3PreDisable(TIM2); }

// Compare mode: Set compare mode of channel 3 TIM_COMP_* (default freeze; register CHCTLR2.OC3M[2:0])
INLINE void TIMx_OC3Mode(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(7<<4)) | (mode<<4); }
INLINE void TIM1_OC3Mode(int mode) { TIMx_OC3Mode(TIM1, mode); }
INLINE void TIM2_OC3Mode(int mode) { TIMx_OC3Mode(TIM2, mode); }

// Compare mode: Enable/Disable compare channel 3 clear by ETRF input (default disabled; register CHCTLR2.OC3CE)
INLINE void TIMx_OC3ClrEnable(TIM_t* tim) { tim->CHCTLR2 |= B7; }
INLINE void TIM1_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM1); }
INLINE void TIM2_OC3ClrEnable(void) { TIMx_OC3ClrEnable(TIM2); }

INLINE void TIMx_OC3ClrDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B7; }
INLINE void TIM1_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM1); }
INLINE void TIM2_OC3ClrDisable(void) { TIMx_OC3ClrDisable(TIM2); }

// Capture mode: Set prescaler of capture channel 3 TIM_ETRPDIV_* (default prescaler off; register CHCTLR2.IC3PSC[1:0])
INLINE void TIMx_IC3Div(TIM_t* tim, int div) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<2)) | (div<<2); }
INLINE void TIM1_IC3Div(int div) { TIMx_IC3Div(TIM1, div); }
INLINE void TIM2_IC3Div(int div) { TIMx_IC3Div(TIM2, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 3 (record up to N events; default 0; register CHCTLR2.IC3F[3:0])
INLINE void TIMx_IC3Filter(TIM_t* tim, int filt) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(0x0f<<4)) | (filt<<4); }
INLINE void TIM1_IC3Filter(int filt) { TIMx_IC3Filter(TIM1, filt); }
INLINE void TIM2_IC3Filter(int filt) { TIMx_IC3Filter(TIM2, filt); }

// Select input/output mode of compare capture channel 4 TIM_CCSEL_* (default output; register CHCTLR2.CCS4S[1:0])
INLINE void TIMx_CC4Sel(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<8)) | (mode<<8); }
INLINE void TIM1_CC4Sel(int mode) { TIMx_CC4Sel(TIM1, mode); }
INLINE void TIM2_CC4Sel(int mode) { TIMx_CC4Sel(TIM2, mode); }

// Compare mode: Enable/Disable fast compare of channel 4 (default disable; register CHCTLR2.OC4FE)
INLINE void TIMx_OC4FastEnable(TIM_t* tim) { tim->CHCTLR2 |= B10; }
INLINE void TIM1_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM1); }
INLINE void TIM2_OC4FastEnable(void) { TIMx_OC4FastEnable(TIM2); }

INLINE void TIMx_OC4FastDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B10; }
INLINE void TIM1_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM1); }
INLINE void TIM2_OC4FastDisable(void) { TIMx_OC4FastDisable(TIM2); }

// Compare mode: Enable/Disable preload compare register 4 (default disable; register CHCTLR2.OC4PE)
INLINE void TIMx_OC4PreEnable(TIM_t* tim) { tim->CHCTLR2 |= B11; }
INLINE void TIM1_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM1); }
INLINE void TIM2_OC4PreEnable(void) { TIMx_OC4PreEnable(TIM2); }

INLINE void TIMx_OC4PreDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B11; }
INLINE void TIM1_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM1); }
INLINE void TIM2_OC4PreDisable(void) { TIMx_OC4PreDisable(TIM2); }

// Compare mode: Set compare mode of channel 4 TIM_COMP_* (default freeze; register CHCTLR2.OC4M[2:0])
INLINE void TIMx_OC4Mode(TIM_t* tim, int mode) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(7<<12)) | (mode<<12); }
INLINE void TIM1_OC4Mode(int mode) { TIMx_OC4Mode(TIM1, mode); }
INLINE void TIM2_OC4Mode(int mode) { TIMx_OC4Mode(TIM2, mode); }

// Compare mode: Enable/Disable compare channel 4 clear by ETRF input (default disabled; register CHCTLR2.OC4CE)
INLINE void TIMx_OC4ClrEnable(TIM_t* tim) { tim->CHCTLR2 |= B15; }
INLINE void TIM1_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM1); }
INLINE void TIM2_OC4ClrEnable(void) { TIMx_OC4ClrEnable(TIM2); }

INLINE void TIMx_OC4ClrDisable(TIM_t* tim) { tim->CHCTLR2 &= ~B15; }
INLINE void TIM1_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM1); }
INLINE void TIM2_OC4ClrDisable(void) { TIMx_OC4ClrDisable(TIM2); }

// Capture mode: Set prescaler of capture channel 4 TIM_ETRPDIV_* (default prescaler off; register CHCTLR2.IC4PSC[1:0])
INLINE void TIMx_IC4Div(TIM_t* tim, int div) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(3<<10)) | (div<<10); }
INLINE void TIM1_IC4Div(int div) { TIMx_IC4Div(TIM1, div); }
INLINE void TIM2_IC4Div(int div) { TIMx_IC4Div(TIM2, div); }

// Capture mode: Select input capture filtering TIM_FILTER_* of channel 4 (record up to N events; default 0; default 0; register CHCTLR2.IC4F[3:0])
INLINE void TIMx_IC4Filter(TIM_t* tim, int filt) { tim->CHCTLR2 = (tim->CHCTLR2 & ~(0x0f<<12)) | (filt<<12); }
INLINE void TIM1_IC4Filter(int filt) { TIMx_IC4Filter(TIM1, filt); }
INLINE void TIM2_IC4Filter(int filt) { TIMx_IC4Filter(TIM2, filt); }

// === Compare capture enable

// Enable/Disable compare capture channel 1 (default disable; register CCER.CC1E)
INLINE void TIMx_CC1Enable(TIM_t* tim) { tim->CCER |= B0; }
INLINE void TIM1_CC1Enable(void) { TIMx_CC1Enable(TIM1); }
INLINE void TIM2_CC1Enable(void) { TIMx_CC1Enable(TIM2); }

INLINE void TIMx_CC1Disable(TIM_t* tim) { tim->CCER &= ~B0; }
INLINE void TIM1_CC1Disable(void) { TIMx_CC1Disable(TIM1); }
INLINE void TIM2_CC1Disable(void) { TIMx_CC1Disable(TIM2); }

// Compare mode: Set compare channel 1 output polarity to active LOW or HIGH (default high; register CCER.CC1P)
INLINE void TIMx_OC1Low(TIM_t* tim) { tim->CCER |= B1; }
INLINE void TIM1_OC1Low(void) { TIMx_OC1Low(TIM1); }
INLINE void TIM2_OC1Low(void) { TIMx_OC1Low(TIM2); }

INLINE void TIMx_OC1High(TIM_t* tim) { tim->CCER &= ~B1; }
INLINE void TIM1_OC1High(void) { TIMx_OC1High(TIM1); }
INLINE void TIM2_OC1High(void) { TIMx_OC1High(TIM2); }

// Capture mode: Set capture channel 1 input polarity to falling or rising edge (default rising; register CCER.CC1P)
INLINE void TIMx_IC1Fall(TIM_t* tim) { tim->CCER |= B1; }
INLINE void TIM1_IC1Fall(void) { TIMx_IC1Fall(TIM1); }
INLINE void TIM2_IC1Fall(void) { TIMx_IC1Fall(TIM2); }

INLINE void TIMx_IC1Rise(TIM_t* tim) { tim->CCER &= ~B1; }
INLINE void TIM1_IC1Rise(void) { TIMx_IC1Rise(TIM1); }
INLINE void TIM2_IC1Rise(void) { TIMx_IC1Rise(TIM2); }

// Enable/Disable compare capture channel 1 complementary (default disable; register CCER.CC1NE)
INLINE void TIMx_CC1NEnable(TIM_t* tim) { tim->CCER |= B2; }
INLINE void TIM1_CC1NEnable(void) { TIMx_CC1NEnable(TIM1); }
INLINE void TIM2_CC1NEnable(void) { TIMx_CC1NEnable(TIM2); }

INLINE void TIMx_CC1NDisable(TIM_t* tim) { tim->CCER &= ~B2; }
INLINE void TIM1_CC1NDisable(void) { TIMx_CC1NDisable(TIM1); }
INLINE void TIM2_CC1NDisable(void) { TIMx_CC1NDisable(TIM2); }

// Compare mode: Set compare channel 1 complementary output polarity to active LOW or HIGH (default high; register CCER.CC1NP)
INLINE void TIMx_OC1NLow(TIM_t* tim) { tim->CCER |= B3; }
INLINE void TIM1_OC1NLow(void) { TIMx_OC1NLow(TIM1); }
INLINE void TIM2_OC1NLow(void) { TIMx_OC1NLow(TIM2); }

INLINE void TIMx_OC1NHigh(TIM_t* tim) { tim->CCER &= ~B3; }
INLINE void TIM1_OC1NHigh(void) { TIMx_OC1NHigh(TIM1); }
INLINE void TIM2_OC1NHigh(void) { TIMx_OC1NHigh(TIM2); }

// Capture mode: Set capture channel 1 complementary input polarity to falling or rising edge (default rising; register CCER.CC1NP)
INLINE void TIMx_IC1NFall(TIM_t* tim) { tim->CCER |= B3; }
INLINE void TIM1_IC1NFall(void) { TIMx_IC1NFall(TIM1); }
INLINE void TIM2_IC1NFall(void) { TIMx_IC1NFall(TIM2); }

INLINE void TIMx_IC1NRise(TIM_t* tim) { tim->CCER &= ~B3; }
INLINE void TIM1_IC1NRise(void) { TIMx_IC1NRise(TIM1); }
INLINE void TIM2_IC1NRise(void) { TIMx_IC1NRise(TIM2); }

// Enable/Disable compare capture channel 2 (default disable; register CCER.CC2E)
INLINE void TIMx_CC2Enable(TIM_t* tim) { tim->CCER |= B4; }
INLINE void TIM1_CC2Enable(void) { TIMx_CC2Enable(TIM1); }
INLINE void TIM2_CC2Enable(void) { TIMx_CC2Enable(TIM2); }

INLINE void TIMx_CC2Disable(TIM_t* tim) { tim->CCER &= ~B4; }
INLINE void TIM1_CC2Disable(void) { TIMx_CC2Disable(TIM1); }
INLINE void TIM2_CC2Disable(void) { TIMx_CC2Disable(TIM2); }

// Compare mode: Set compare channel 2 output polarity to active LOW or HIGH (default high; register CCER.CC2P)
INLINE void TIMx_OC2Low(TIM_t* tim) { tim->CCER |= B5; }
INLINE void TIM1_OC2Low(void) { TIMx_OC2Low(TIM1); }
INLINE void TIM2_OC2Low(void) { TIMx_OC2Low(TIM2); }

INLINE void TIMx_OC2High(TIM_t* tim) { tim->CCER &= ~B5; }
INLINE void TIM1_OC2High(void) { TIMx_OC2High(TIM1); }
INLINE void TIM2_OC2High(void) { TIMx_OC2High(TIM2); }

// Capture mode: Set capture channel 2 input polarity to falling or rising edge (default rising; register CCER.CC2P)
INLINE void TIMx_IC2Fall(TIM_t* tim) { tim->CCER |= B5; }
INLINE void TIM1_IC2Fall(void) { TIMx_IC2Fall(TIM1); }
INLINE void TIM2_IC2Fall(void) { TIMx_IC2Fall(TIM2); }

INLINE void TIMx_IC2Rise(TIM_t* tim) { tim->CCER &= ~B5; }
INLINE void TIM1_IC2Rise(void) { TIMx_IC2Rise(TIM1); }
INLINE void TIM2_IC2Rise(void) { TIMx_IC2Rise(TIM2); }

// Enable/Disable compare capture channel 2 complementary (default disable; register CCER.CC2NE)
INLINE void TIMx_CC2NEnable(TIM_t* tim) { tim->CCER |= B6; }
INLINE void TIM1_CC2NEnable(void) { TIMx_CC2NEnable(TIM1); }
INLINE void TIM2_CC2NEnable(void) { TIMx_CC2NEnable(TIM2); }

INLINE void TIMx_CC2NDisable(TIM_t* tim) { tim->CCER &= ~B6; }
INLINE void TIM1_CC2NDisable(void) { TIMx_CC2NDisable(TIM1); }
INLINE void TIM2_CC2NDisable(void) { TIMx_CC2NDisable(TIM2); }

// Compare mode: Set compare channel 2 complementary output polarity to active LOW or HIGH (default high; register CCER.CC2NP)
INLINE void TIMx_OC2NLow(TIM_t* tim) { tim->CCER |= B7; }
INLINE void TIM1_OC2NLow(void) { TIMx_OC2NLow(TIM1); }
INLINE void TIM2_OC2NLow(void) { TIMx_OC2NLow(TIM2); }

INLINE void TIMx_OC2NHigh(TIM_t* tim) { tim->CCER &= ~B7; }
INLINE void TIM1_OC2NHigh(void) { TIMx_OC2NHigh(TIM1); }
INLINE void TIM2_OC2NHigh(void) { TIMx_OC2NHigh(TIM2); }

// Capture mode: Set capture channel 2 complementary input polarity to falling or rising edge (default rising; register CCER.CC2NP)
INLINE void TIMx_IC2NFall(TIM_t* tim) { tim->CCER |= B7; }
INLINE void TIM1_IC2NFall(void) { TIMx_IC2NFall(TIM1); }
INLINE void TIM2_IC2NFall(void) { TIMx_IC2NFall(TIM2); }

INLINE void TIMx_IC2NRise(TIM_t* tim) { tim->CCER &= ~B7; }
INLINE void TIM1_IC2NRise(void) { TIMx_IC2NRise(TIM1); }
INLINE void TIM2_IC2NRise(void) { TIMx_IC2NRise(TIM2); }

// Enable/Disable compare capture channel 3 (default disable; register CCER.CC3E)
INLINE void TIMx_CC3Enable(TIM_t* tim) { tim->CCER |= B8; }
INLINE void TIM1_CC3Enable(void) { TIMx_CC3Enable(TIM1); }
INLINE void TIM2_CC3Enable(void) { TIMx_CC3Enable(TIM2); }

INLINE void TIMx_CC3Disable(TIM_t* tim) { tim->CCER &= ~B8; }
INLINE void TIM1_CC3Disable(void) { TIMx_CC3Disable(TIM1); }
INLINE void TIM2_CC3Disable(void) { TIMx_CC3Disable(TIM2); }

// Compare mode: Set compare channel 3 output polarity to active LOW or HIGH (default high; register CCER.CC3P)
INLINE void TIMx_OC3Low(TIM_t* tim) { tim->CCER |= B9; }
INLINE void TIM1_OC3Low(void) { TIMx_OC3Low(TIM1); }
INLINE void TIM2_OC3Low(void) { TIMx_OC3Low(TIM2); }

INLINE void TIMx_OC3High(TIM_t* tim) { tim->CCER &= ~B9; }
INLINE void TIM1_OC3High(void) { TIMx_OC3High(TIM1); }
INLINE void TIM2_OC3High(void) { TIMx_OC3High(TIM2); }

// Capture mode: Set capture channel 3 input polarity to falling or rising edge (default rising; register CCER.CC3P)
INLINE void TIMx_IC3Fall(TIM_t* tim) { tim->CCER |= B9; }
INLINE void TIM1_IC3Fall(void) { TIMx_IC3Fall(TIM1); }
INLINE void TIM2_IC3Fall(void) { TIMx_IC3Fall(TIM2); }

INLINE void TIMx_IC3Rise(TIM_t* tim) { tim->CCER &= ~B9; }
INLINE void TIM1_IC3Rise(void) { TIMx_IC3Rise(TIM1); }
INLINE void TIM2_IC3Rise(void) { TIMx_IC3Rise(TIM2); }

// Enable/Disable compare capture channel 3 complementary (default disable; register CCER.CC3NE)
INLINE void TIMx_CC3NEnable(TIM_t* tim) { tim->CCER |= B10; }
INLINE void TIM1_CC3NEnable(void) { TIMx_CC3NEnable(TIM1); }
INLINE void TIM2_CC3NEnable(void) { TIMx_CC3NEnable(TIM2); }

INLINE void TIMx_CC3NDisable(TIM_t* tim) { tim->CCER &= ~B10; }
INLINE void TIM1_CC3NDisable(void) { TIMx_CC3NDisable(TIM1); }
INLINE void TIM2_CC3NDisable(void) { TIMx_CC3NDisable(TIM2); }

// Compare mode: Set compare channel 3 complementary output polarity to active LOW or HIGH (default high; register CCER.CC3NP)
INLINE void TIMx_OC3NLow(TIM_t* tim) { tim->CCER |= B11; }
INLINE void TIM1_OC3NLow(void) { TIMx_OC3NLow(TIM1); }
INLINE void TIM2_OC3NLow(void) { TIMx_OC3NLow(TIM2); }

INLINE void TIMx_OC3NHigh(TIM_t* tim) { tim->CCER &= ~B11; }
INLINE void TIM1_OC3NHigh(void) { TIMx_OC3NHigh(TIM1); }
INLINE void TIM2_OC3NHigh(void) { TIMx_OC3NHigh(TIM2); }

// Capture mode: Set capture channel 3 complementary input polarity to falling or rising edge (default rising; register CCER.CC2NP)
INLINE void TIMx_IC3NFall(TIM_t* tim) { tim->CCER |= B11; }
INLINE void TIM1_IC3NFall(void) { TIMx_IC3NFall(TIM1); }
INLINE void TIM2_IC3NFall(void) { TIMx_IC3NFall(TIM2); }

INLINE void TIMx_IC3NRise(TIM_t* tim) { tim->CCER &= ~B11; }
INLINE void TIM1_IC3NRise(void) { TIMx_IC3NRise(TIM1); }
INLINE void TIM2_IC3NRise(void) { TIMx_IC3NRise(TIM2); }

// Enable/Disable compare capture channel 4 (default disable; register CCER.CC4E)
INLINE void TIMx_CC4Enable(TIM_t* tim) { tim->CCER |= B12; }
INLINE void TIM1_CC4Enable(void) { TIMx_CC4Enable(TIM1); }
INLINE void TIM2_CC4Enable(void) { TIMx_CC4Enable(TIM2); }

INLINE void TIMx_CC4Disable(TIM_t* tim) { tim->CCER &= ~B12; }
INLINE void TIM1_CC4Disable(void) { TIMx_CC4Disable(TIM1); }
INLINE void TIM2_CC4Disable(void) { TIMx_CC4Disable(TIM2); }

// Compare mode: Set compare channel 4 output polarity to active LOW or HIGH (default high; register CCER.CC4P)
INLINE void TIMx_OC4Low(TIM_t* tim) { tim->CCER |= B13; }
INLINE void TIM1_OC4Low(void) { TIMx_OC4Low(TIM1); }
INLINE void TIM2_OC4Low(void) { TIMx_OC4Low(TIM2); }

INLINE void TIMx_OC4High(TIM_t* tim) { tim->CCER &= ~B13; }
INLINE void TIM1_OC4High(void) { TIMx_OC4High(TIM1); }
INLINE void TIM2_OC4High(void) { TIMx_OC4High(TIM2); }

// Capture mode: Set capture channel 4 input polarity to falling or rising edge (default rising; register CCER.CC4P)
INLINE void TIMx_IC4Fall(TIM_t* tim) { tim->CCER |= B13; }
INLINE void TIM1_IC4Fall(void) { TIMx_IC4Fall(TIM1); }
INLINE void TIM2_IC4Fall(void) { TIMx_IC4Fall(TIM2); }

INLINE void TIMx_IC4Rise(TIM_t* tim) { tim->CCER &= ~B13; }
INLINE void TIM1_IC4Rise(void) { TIMx_IC4Rise(TIM1); }
INLINE void TIM2_IC4Rise(void) { TIMx_IC4Rise(TIM2); }

// === Counter

// Set/Get counter 0..65535 (default 0; register CNT)
INLINE void TIMx_Cnt(TIM_t* tim, int cnt) { tim->CNT = cnt; }
INLINE void TIM1_Cnt(int cnt) { TIMx_Cnt(TIM1, cnt); }
INLINE void TIM2_Cnt(int cnt) { TIMx_Cnt(TIM2, cnt); }

INLINE u16 TIMx_GetCnt(TIM_t* tim) { return (u16)tim->CNT; }
INLINE u16 TIM1_GetCnt(void) { return TIMx_GetCnt(TIM1); }
INLINE u16 TIM2_GetCnt(void) { return TIMx_GetCnt(TIM2); }

// Set/Get prescaler 0..65535 (divide ratio = presc+1; default 0 = no divide; register PSC)
INLINE void TIMx_Presc(TIM_t* tim, int presc) { tim->PSC = presc; }
INLINE void TIM1_Presc(int presc) { TIMx_Presc(TIM1, presc); }
INLINE void TIM2_Presc(int presc) { TIMx_Presc(TIM2, presc); }

INLINE u16 TIMx_GetPresc(TIM_t* tim) { return (u16)tim->PSC; }
INLINE u16 TIM1_GetPresc(void) { return TIMx_GetPresc(TIM1); }
INLINE u16 TIM2_GetPresc(void) { return TIMx_GetPresc(TIM2); }

// Set/Get timer divider 1..65536 (divide ratio = prescaler+1; default 1 = no divide)
INLINE void TIMx_Div(TIM_t* tim, int div) { TIMx_Presc(tim, div-1); }
INLINE void TIM1_Div(int div) { TIMx_Div(TIM1, div); }
INLINE void TIM2_Div(int div) { TIMx_Div(TIM2, div); }

INLINE int TIMx_GetDiv(TIM_t* tim) { return TIMx_GetPresc(tim)+1; }
INLINE int TIM1_GetDiv(void) { return TIMx_GetDiv(TIM1); }
INLINE int TIM2_GetDiv(void) { return TIMx_GetDiv(TIM2); }

// Set/Get auto-reload value 0..65535 (timer period = load+1; default 0 = 1 tick; register ATRLR)
INLINE void TIMx_Load(TIM_t* tim, int val) { tim->ATRLR = val; }
INLINE void TIM1_Load(int val) { TIMx_Load(TIM1, val); }
INLINE void TIM2_Load(int val) { TIMx_Load(TIM2, val); }

INLINE u16 TIMx_GetLoad(TIM_t* tim) { return (u16)tim->ATRLR; }
INLINE u16 TIM1_GetLoad(void) { return TIMx_GetLoad(TIM1); }
INLINE u16 TIM2_GetLoad(void) { return TIMx_GetLoad(TIM2); }

// Set/Get repeat count value 0..65535 (default 0; register RPTCR)
INLINE void TIMx_Repeat(TIM_t* tim, int val) { tim->RPTCR = val; }
INLINE void TIM1_Repeat(int val) { TIMx_Repeat(TIM1, val); }
INLINE void TIM2_Repeat(int val) { TIMx_Repeat(TIM2, val); }

INLINE u16 TIMx_GetRepeat(TIM_t* tim) { return (u16)tim->RPTCR; }
INLINE u16 TIM1_GetRepeat(void) { return TIMx_GetRepeat(TIM1); }
INLINE u16 TIM2_GetRepeat(void) { return TIMx_GetRepeat(TIM2); }

// Set/Get compare value of channel 1, 0..65535 (default 0; register CH1CVR.CH1CVR)
INLINE void TIMx_Comp1(TIM_t* tim, int val) { tim->CH1CVR = val; }
INLINE void TIM1_Comp1(int val) { TIMx_Comp1(TIM1, val); }
INLINE void TIM2_Comp1(int val) { TIMx_Comp1(TIM2, val); }

INLINE u16 TIMx_GetComp1(TIM_t* tim) { return (u16)tim->CH1CVR; }
INLINE u16 TIM1_GetComp1(void) { return TIMx_GetComp1(TIM1); }
INLINE u16 TIM2_GetComp1(void) { return TIMx_GetComp1(TIM2); }

// Check level flag of captured value channel 1 (register CH1CVR.LEVEL1)
INLINE Bool TIMx_GetLevel1(TIM_t* tim) { return (tim->CH1CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel1(void) { return TIMx_GetLevel1(TIM1); }
INLINE Bool TIM2_GetLevel1(void) { return TIMx_GetLevel1(TIM2); }

// Set/Get compare value of channel 2, 0..65535 (default 0; register CH2CVR.CH2CVR)
INLINE void TIMx_Comp2(TIM_t* tim, int val) { tim->CH2CVR = val; }
INLINE void TIM1_Comp2(int val) { TIMx_Comp2(TIM1, val); }
INLINE void TIM2_Comp2(int val) { TIMx_Comp2(TIM2, val); }

INLINE u16 TIMx_GetComp2(TIM_t* tim) { return (u16)tim->CH2CVR; }
INLINE u16 TIM1_GetComp2(void) { return TIMx_GetComp2(TIM1); }
INLINE u16 TIM2_GetComp2(void) { return TIMx_GetComp2(TIM2); }

// Check level flag of captured value channel 2 (register CH2CVR.LEVEL2)
INLINE Bool TIMx_GetLevel2(TIM_t* tim) { return (tim->CH2CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel2(void) { return TIMx_GetLevel2(TIM1); }
INLINE Bool TIM2_GetLevel2(void) { return TIMx_GetLevel2(TIM2); }

// Set/Get compare value of channel 3, 0..65535 (default 0; register CH3CVR.CH3CVR)
INLINE void TIMx_Comp3(TIM_t* tim, int val) { tim->CH3CVR = val; }
INLINE void TIM1_Comp3(int val) { TIMx_Comp3(TIM1, val); }
INLINE void TIM2_Comp3(int val) { TIMx_Comp3(TIM2, val); }

INLINE u16 TIMx_GetComp3(TIM_t* tim) { return (u16)tim->CH3CVR; }
INLINE u16 TIM1_GetComp3(void) { return TIMx_GetComp3(TIM1); }
INLINE u16 TIM2_GetComp3(void) { return TIMx_GetComp3(TIM2); }

// Check level flag of captured value channel 3 (register CH3CVR.LEVEL3)
INLINE Bool TIMx_GetLevel3(TIM_t* tim) { return (tim->CH3CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel3(void) { return TIMx_GetLevel3(TIM1); }
INLINE Bool TIM2_GetLevel3(void) { return TIMx_GetLevel3(TIM2); }

// Set/Get compare value of channel 4, 0..65535 (default 0; register CH4CVR.CH4CVR)
INLINE void TIMx_Comp4(TIM_t* tim, int val) { tim->CH4CVR = val; }
INLINE void TIM1_Comp4(int val) { TIMx_Comp4(TIM1, val); }
INLINE void TIM2_Comp4(int val) { TIMx_Comp4(TIM2, val); }

INLINE u16 TIMx_GetComp4(TIM_t* tim) { return (u16)tim->CH4CVR; }
INLINE u16 TIM1_GetComp4(void) { return TIMx_GetComp4(TIM1); }
INLINE u16 TIM2_GetComp4(void) { return TIMx_GetComp4(TIM2); }

// Check level flag of captured value channel 4 (register CH4CVR.LEVEL4)
INLINE Bool TIMx_GetLevel4(TIM_t* tim) { return (tim->CH4CVR & B16) != 0; }
INLINE Bool TIM1_GetLevel4(void) { return TIMx_GetLevel4(TIM1); }
INLINE Bool TIM2_GetLevel4(void) { return TIMx_GetLevel4(TIM2); }

// === Brake and deadtime

// Set/Get deadband time 0..255 (default 0; register BDTR.DTG[7:0])
//  dt=0..127 -> DT=dt*Tdtg, Tdtg=TDTS
//  dt=128..191 -> DT=(64 + dt-128)*Tdtg, Tdtg=2*TDTS
//  dt=192..223 -> DT=(32 + dt-192)*Tdtg, Tdtg=8*TDTS
//  dt=224..255 -> DT=(32 + dt-224)*Tdtg, Tdtg=16*TDTS
INLINE void TIMx_DTG(TIM_t* tim, int dt) { tim->BDTR = (tim->BDTR & ~0xff) | dt; }
INLINE void TIM1_DTG(int dt) { TIMx_DTG(TIM1, dt); }
INLINE void TIM2_DTG(int dt) { TIMx_DTG(TIM2, dt); }

INLINE u8 TIMx_GetDTG(TIM_t* tim) { return (u8)tim->BDTR; }
INLINE u8 TIM1_GetDTG(void) { return TIMx_GetDTG(TIM1); }
INLINE u8 TIM2_GetDTG(void) { return TIMx_GetDTG(TIM2); }

// Set lock function TIM_LOCK_* (default no lock; register BDTR.LOCK[1:0])
// - Lock bits can only be written once and cannot be modified again until reset.
INLINE void TIMx_Lock(TIM_t* tim, int lock) { tim->BDTR = (tim->BDTR & ~(3<<8)) | (lock<<8); }
INLINE void TIM1_Lock(int lock) { TIMx_Lock(TIM1, lock); }
INLINE void TIM2_Lock(int lock) { TIMx_Lock(TIM2, lock); }

// Enable/Disable idle level of OC outputs (default outputs are disabled when timer is off; register BDTR.OSSI)
INLINE void TIMx_OSSIEnable(TIM_t* tim) { tim->BDTR |= B10; }
INLINE void TIM1_OSSIEnable(void) { TIMx_OSSIEnable(TIM1); }
INLINE void TIM2_OSSIEnable(void) { TIMx_OSSIEnable(TIM2); }

INLINE void TIMx_OSSIDisable(TIM_t* tim) { tim->BDTR &= ~B10; }
INLINE void TIM1_OSSIDisable(void) { TIMx_OSSIDisable(TIM1); }
INLINE void TIM2_OSSIDisable(void) { TIMx_OSSIDisable(TIM2); }

// Enable/Disable invalid level of OC outputs (default outputs are disabled when timer is off; register BDTR.OSSR)
INLINE void TIMx_OSSREnable(TIM_t* tim) { tim->BDTR |= B11; }
INLINE void TIM1_OSSREnable(void) { TIMx_OSSREnable(TIM1); }
INLINE void TIM2_OSSREnable(void) { TIMx_OSSREnable(TIM2); }

INLINE void TIMx_OSSRDisable(TIM_t* tim) { tim->BDTR &= ~B11; }
INLINE void TIM1_OSSRDisable(void) { TIMx_OSSRDisable(TIM1); }
INLINE void TIM2_OSSRDisable(void) { TIMx_OSSRDisable(TIM2); }

// Enable/Disable brake input (default disabled; register BDTR.BKE)
INLINE void TIMx_BrkEnable(TIM_t* tim) { tim->BDTR |= B12; }
INLINE void TIM1_BrkEnable(void) { TIMx_BrkEnable(TIM1); }
INLINE void TIM2_BrkEnable(void) { TIMx_BrkEnable(TIM2); }

INLINE void TIMx_BrkDisable(TIM_t* tim) { tim->BDTR &= ~B12; }
INLINE void TIM1_BrkDisable(void) { TIMx_BrkDisable(TIM1); }
INLINE void TIM2_BrkDisable(void) { TIMx_BrkDisable(TIM2); }

// Set brake input polarity to high or low (default low; register BDTR.BKP)
INLINE void TIMx_BrkHigh(TIM_t* tim) { tim->BDTR |= B13; }
INLINE void TIM1_BrkHigh(void) { TIMx_BrkHigh(TIM1); }
INLINE void TIM2_BrkHigh(void) { TIMx_BrkHigh(TIM2); }

INLINE void TIMx_BrkLow(TIM_t* tim) { tim->BDTR &= ~B13; }
INLINE void TIM1_BrkLow(void) { TIMx_BrkLow(TIM1); }
INLINE void TIM2_BrkLow(void) { TIMx_BrkLow(TIM2); }

// Enable/Disable auto OC output (default disable; register BDTR.AOE)
INLINE void TIMx_AutoOCEnable(TIM_t* tim) { tim->BDTR |= B14; }
INLINE void TIM1_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM1); }
INLINE void TIM2_AutoOCEnable(void) { TIMx_AutoOCEnable(TIM2); }

INLINE void TIMx_AutoOCDisable(TIM_t* tim) { tim->BDTR &= ~B14; }
INLINE void TIM1_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM1); }
INLINE void TIM2_AutoOCDisable(void) { TIMx_AutoOCDisable(TIM2); }

// Enable/Disable OC output (default disable; register BDTR.MOE)
INLINE void TIMx_OCEnable(TIM_t* tim) { tim->BDTR |= B15; }
INLINE void TIM1_OCEnable(void) { TIMx_OCEnable(TIM1); }
INLINE void TIM2_OCEnable(void) { TIMx_OCEnable(TIM2); }

INLINE void TIMx_OCDisable(TIM_t* tim) { tim->BDTR &= ~B15; }
INLINE void TIM1_OCDisable(void) { TIMx_OCDisable(TIM1); }
INLINE void TIM2_OCDisable(void) { TIMx_OCDisable(TIM2); }

// === DMA setup

// Set offset address of DMA in continuous mode 0..31 (default 0; register DMACFGR.DBA[4:0])
INLINE void TIMx_DMAOff(TIM_t* tim, int off) { tim->DMACFGR = (tim->DMACFGR & ~0x1f) | off; }
INLINE void TIM1_DMAOff(int off) { TIMx_DMAOff(TIM1, off); }
INLINE void TIM2_DMAOff(int off) { TIMx_DMAOff(TIM2, off); }

// Set length of DMA continuous transmission 0..31 (default 0; register DMACFGR.DBL[4:0])
INLINE void TIMx_DMALen(TIM_t* tim, int len) { tim->DMACFGR = (tim->DMACFGR & ~(0x1f<<8)) | (len<<8); }
INLINE void TIM1_DMALen(int len) { TIMx_DMALen(TIM1, len); }
INLINE void TIM2_DMALen(int len) { TIMx_DMALen(TIM2, len); }

// Set base address of DMA in continuous mode 0..65535 (default 0; register DMAADR.DMAB[15:0])
INLINE void TIMx_DMABase(TIM_t* tim, int base) { tim->DMAADR = base; }
INLINE void TIM1_DMABase(int base) { TIMx_DMABase(TIM1, base); }
INLINE void TIM2_DMABase(int base) { TIMx_DMABase(TIM2, base); }

// === Setup

// Reset timer to default setup
void TIMx_Reset(TIM_t* tim);
INLINE void TIM1_Reset(void) { RCC_TIM1Reset(); }
INLINE void TIM2_Reset(void) { RCC_TIM2Reset(); }

// Enable timer clock source
void TIMx_ClkEnable(TIM_t* tim);
INLINE void TIM1_ClkEnable(void) { RCC_TIM1ClkEnable(); }
INLINE void TIM2_ClkEnable(void) { RCC_TIM2ClkEnable(); }

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

#ifdef __cplusplus
}
#endif

#endif // _SDK_TIM_H

#endif // USE_TIM
