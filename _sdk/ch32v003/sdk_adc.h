
// ****************************************************************************
//
//                         ADC Analog-to-digital converter
//
// ****************************************************************************
// 10-bit conversion, max. 24MHz input clock, 8+2 channels
// Conversion time Tconv = sanpling_time + 11Tadcclk
// - Rule conversion group: conversion order of the channels is set in the ADC_RSQRx register (up to 16 conversions).
// - Injection conversion group: conversion order is set in the ADC_ISQR register (up to 4 conversions).

#if USE_ADC		// 1=use ADC peripheral

#ifndef _SDK_ADC_H
#define _SDK_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

// ADC registers
typedef struct {
	io32	STATR;		// 0x00: status register
	io32	CTLR1;		// 0x04: control register 1
	io32	CTLR2;		// 0x08: control register 2
	io32	SAMPTR1;	// 0x0C: sample time register 1
	io32	SAMPTR2;	// 0x10: sample time register 2
	io32	IOFR1;		// 0x14: inject channel data offset register 1
	io32	IOFR2;		// 0x18: inject channel data offset register 2
	io32	IOFR3;		// 0x1C: inject channel data offset register 3
	io32	IOFR4;		// 0x20: inject channel data offset register 4
	io32	WDHTR;		// 0x24: watchdog high threshold register
	io32	WDLTR;		// 0x28: watchdog low threshold register
	io32	RSQR1;		// 0x2C: regular sequence register 1
	io32	RSQR2;		// 0x30: regular sequence register 2
	io32	RSQR3;		// 0x34: regular sequence register 3
	io32	ISQR;		// 0x38: injected sequence register
	io32	IDATAR1;	// 0x3C: injected data register 1
	io32	IDATAR2;	// 0x40: injected data register 2
	io32	IDATAR3;	// 0x44: injected data register 3
	io32	IDATAR4;	// 0x48: injected data register 4
	io32	RDATAR;		// 0x4C: regular data register
	io32	DLYR;		// 0x50: delayed data register
} ADC_t;
STATIC_ASSERT(sizeof(ADC_t) == 0x54, "Incorrect ADC_t!");
#define ADC1	((ADC_t*)ADC1_BASE)	// 0x40012400

// ADC calibration coltage selection
#define ADC_CAL_24	1	// calibration voltage 2/4 AVDD
#define ADC_CAL_34	2	// calibration voltage 3/4 AVDD

// external trigger event selection for initiating injection channel conversion
#define ADC_JEXT_T1_CC3		0	// CC3 event of timer 1
#define ADC_JEXT_T1_CC4		1	// CC4 event of timer 1
#define ADC_JEXT_T2_CC3		2	// CC3 event of timer 2
#define ADC_JEXT_T2_CC4		3	// CC4 event of timer 2
#define ADC_JEXT_PD1_PA2	6	// PD1/PA2
#define ADC_JEXT_JSWSTART	7	// JSWSTART software trigger

// === ADC status

// Check/Clear analog watchdog event (register STATR.AWD)
INLINE Bool ADCx_AWD(ADC_t* adc) { return (adc->STATR & B0) != 0; }
INLINE Bool ADC1_AWD(void) { return ADCx_AWD(ADC1); }

INLINE void ADCx_AWDClr(ADC_t* adc) { adc->STATR &= ~B0; }
INLINE void ADC1_AWDClr(void) { ADCx_AWDClr(ADC1); }

// Check/Clear end of conversion (register STATR.EOC)
INLINE Bool ADCx_End(ADC_t* adc) { return (adc->STATR & B1) != 0; }
INLINE Bool ADC1_End(void) { return ADCx_End(ADC1); }

INLINE void ADCx_EndClr(ADC_t* adc) { adc->STATR &= ~B1; }
INLINE void ADC1_EndClr(void) { ADCx_EndClr(ADC1); }

// Check/Clear end of conversion of injection group (register STATR.JEOC)
INLINE Bool ADCx_JEnd(ADC_t* adc) { return (adc->STATR & B2) != 0; }
INLINE Bool ADC1_JEnd(void) { return ADCx_JEnd(ADC1); }

INLINE void ADCx_JEndClr(ADC_t* adc) { adc->STATR &= ~B2; }
INLINE void ADC1_JEndClr(void) { ADCx_JEndClr(ADC1); }

// Check/Clear start of conversion of injection group (register STATR.JSTRT)
INLINE Bool ADCx_JStart(ADC_t* adc) { return (adc->STATR & B3) != 0; }
INLINE Bool ADC1_JStart(void) { return ADCx_JStart(ADC1); }

INLINE void ADCx_JStartClr(ADC_t* adc) { adc->STATR &= ~B3; }
INLINE void ADC1_JStartClr(void) { ADCx_JStartClr(ADC1); }

// Check/Clear start of conversion of rule group (register STATR.STRT)
INLINE Bool ADCx_Start(ADC_t* adc) { return (adc->STATR & B4) != 0; }
INLINE Bool ADC1_Start(void) { return ADCx_Start(ADC1); }

INLINE void ADCx_StartClr(ADC_t* adc) { adc->STATR &= ~B4; }
INLINE void ADC1_StartClr(void) { ADCx_StartClr(ADC1); }

// === ADC setup

// Enable/Disable ADC (default disable; register CTLR2.ADON)
// - Disable will turn off ADC and enter power-down mode.
// - Enable from disable state will wake up ADC from power-down mode. Delay Tstab is required to stabilize.
// - Enable from enable state will start the conversion.
INLINE void ADCx_Enable(ADC_t* adc) { adc->CTLR2 |= B0; }
INLINE void ADC1_Enable(void) { ADCx_Enable(ADC1); }

INLINE void ADCx_Disable(ADC_t* adc) { adc->CTLR2 &= ~B0; }
INLINE void ADC1_Disable(void) { ADCx_Disable(ADC1); }

// Set single or continuous conversion mode (default single; register CTLR2.CONT)
// - In continuous mode, the conversion will continue until switch to single mode.
INLINE void ADCx_Single(ADC_t* adc) { adc->CTLR2 &= ~B1; }
INLINE void ADC1_Single(void) { ADCx_Single(ADC1); }

INLINE void ADCx_Cont(ADC_t* adc) { adc->CTLR2 |= B1; }
INLINE void ADC1_Cont(void) { ADCx_Cont(ADC1); }

// Start/Check calibration (register CTLR2.CAL)
// - Calibration is initiated by software and terminated by hardware.
INLINE void ADCx_CalibStart(ADC_t* adc) { adc->CTLR2 |= B2; }
INLINE void ADC1_CalibStart(void) { ADCx_CalibStart(ADC1); }

INLINE Bool ADCx_CalibBusy(ADC_t* adc) { return (adc->CTLR2 & B2) != 0; }
INLINE Bool ADC1_CalibBusy(void) { return ADCx_CalibBusy(ADC1); }

// Start/Check reset calibration (register CTLR2.RSTCAL)
// - Reset calibration is initiated by software and terminated by hardware.
INLINE void ADCx_RstCalibStart(ADC_t* adc) { adc->CTLR2 |= B3; }
INLINE void ADC1_RstCalibStart(void) { ADCx_RstCalibStart(ADC1); }

INLINE Bool ADCx_RstCalibBusy(ADC_t* adc) { return (adc->CTLR2 & B3) != 0; }
INLINE Bool ADC1_RstCalibBusy(void) { return ADCx_RstCalibBusy(ADC1); }

// Enable/Disable DMA mode (register CTLR2.DMA)
INLINE void ADCx_DMAEnable(ADC_t* adc) { adc->CTLR2 |= B8; }
INLINE void ADC1_DMAEnable(void) { ADCx_DMAEnable(ADC1); }

INLINE void ADCx_DMADisable(ADC_t* adc) { adc->CTLR2 &= ~B8; }
INLINE void ADC1_DMADisable(void) { ADCx_DMADisable(ADC1); }

// Set data alignment to left or right (default right-aligned; register CTLR2.ALIGN)
INLINE void ADCx_AlignLeft(ADC_t* adc) { adc->CTLR2 |= B11; }
INLINE void ADC1_AlignLeft(void) { ADCx_AlignLeft(ADC1); }

INLINE void ADCx_AlignRight(ADC_t* adc) { adc->CTLR2 &= ~B11; }
INLINE void ADC1_AlignRight(void) { ADCx_AlignRight(ADC1); }

// Set external trigger event selection for initiating injection channel conversion ADC_JEXT_* (default CC3 of timer 1; register CTLR2.JEXTSEL[2:0])
INLINE 


#define ADC_JEXT_T1_CC3		0	// CC3 event of timer 1

!!!!!!!!!!!!!!!!! str. 80












// Select analog watchdog channel 0..15 (default 0; register CTLR1.AWDCH[4:0])
INLINE void ADCx_AWDChan(ADC_t* adc, int ch) { adc->CTLR1 = (adc->CTLR1 & ~0x1f) | ch; }
INLINE void ADC1_AWDChan(int ch) { ADCx_AWDChan(ADC1, ch); }

// Enable/Disable scan mode (default disabled; register CTLR1.SCAN)
INLINE void ADCx_ScanEnable(ADC_t* adc) { adc->CTLR1 |= B8; }
INLINE void ADC1_ScanEnable(void) { ADCx_ScanEnable(ADC1); }

INLINE void ADCx_ScanDisable(ADC_t* adc) { adc->CTLR1 &= ~B8; }
INLINE void ADC1_ScanDisable(void) { ADCx_ScanDisable(ADC1); }

// Select analog watchdog in scan mode to single channel or all channels (default all channels; register CTLR1.AWDSGL)
INLINE void ADCx_AWDSingle(ADC_t* adc) { adc->CTLR1 |= B9; }
INLINE void ADC1_AWDSingle(void) { ADCx_AWDSingle(ADC1); }

INLINE void ADCx_AWDAll(ADC_t* adc) { adc->CTLR1 &= ~B9; }
INLINE void ADC1_AWDAll(void) { ADCx_AWDAll(ADC1); }

// Enable/Disable automatic injection switching (default disabled; register CTLR1.JAUTO)
INLINE void ADCx_JAutoEnable(ADC_t* adc) { adc->CTLR1 |= B10; }
INLINE void ADC1_JAutoEnable(void) { ADCx_JAutoEnable(ADC1); }

INLINE void ADCx_JAutoDisable(ADC_t* adc) { adc->CTLR1 &= ~B10; }
INLINE void ADC1_JAutoDisable(void) { ADCx_JAutoDisable(ADC1); }

// Enable/Disable intermittent mode on rule channel (default disabled; register CTLR1.DISCEN)
INLINE void ADCx_IntermEnable(ADC_t* adc) { adc->CTLR1 |= B11; }
INLINE void ADC1_IntermEnable(void) { ADCx_IntermEnable(ADC1); }

INLINE void ADCx_IntermDisable(ADC_t* adc) { adc->CTLR1 &= ~B11; }
INLINE void ADC1_IntermDisable(void) { ADCx_IntermDisable(ADC1); }

// Enable/Disable intermittent mode on injection channel (default disabled; register CTLR1.JDISCEN)
INLINE void ADCx_JIntermEnable(ADC_t* adc) { adc->CTLR1 |= B12; }
INLINE void ADC1_JIntermEnable(void) { ADCx_JIntermEnable(ADC1); }

INLINE void ADCx_JIntermDisable(ADC_t* adc) { adc->CTLR1 &= ~B12; }
INLINE void ADC1_JIntermDisable(void) { ADCx_JIntermDisable(ADC1); }

// Set number of rule channels 1..8 to be converted after exernal triggering in intermittent mode (default 1 channel; register CTLR1.DISCNUM[2:0])
INLINE void ADCx_DiscNum(ADC_t* adc, int num) { adc->CTLR1 = (adc->CTLR1 & ~(7<<13)) | ((num-1)<<13); }
INLINE void ADC1_DiscNum(int num) { ADCx_DiscNum(ADC1, num); }

// Enable/Disable analog watchdog on injection channel (default disable; register CTLR1.JAWDEN)
INLINE void ADCx_JAWDEnable(ADC_t* adc) { adc->CTLR1 |= B22; }
INLINE void ADC1_JAWDEnable(void) { ADCx_JAWDEnable(ADC1); }

INLINE void ADCx_JAWDDisable(ADC_t* adc) { adc->CTLR1 &= ~B22; }
INLINE void ADC1_JAWDDisable(void) { ADCx_JAWDDisable(ADC1); }

// Enable/Disable analog watchdog on rule channel (default disable; register CTLR1.AWDEN)
INLINE void ADCx_AWDEnable(ADC_t* adc) { adc->CTLR1 |= B23; }
INLINE void ADC1_AWDEnable(void) { ADCx_AWDEnable(ADC1); }

INLINE void ADCx_AWDDisable(ADC_t* adc) { adc->CTLR1 &= ~B23; }
INLINE void ADC1_AWDDisable(void) { ADCx_AWDDisable(ADC1); }

// Set calibration voltage selection ADC_CAL_* (default 2/4; register CTLR1.CALVOL[1:0])
INLINE void ADCx_CalVol(ADC_t* adc, int calvol) { adc->CTLR1 = (adc->CTLR1 & ~(3<<25)) | (calvol<<25); }
INLINE void ADC1_CalVol(int calvol) { ADCx_CalVol(ADC1, calvol); }

// === Interrupt

// Enable/Disable interrupt on end of conversion (rule or injection; default disabled; register CTLR1.EOCIE)
INLINE void ADCx_EndIntEnable(ADC_t* adc) { adc->CTLR1 |= B5; }
INLINE void ADC1_EndIntEnable(void) { ADCx_EndIntEnable(ADC1); }

INLINE void ADCx_EndIntDisable(ADC_t* adc) { adc->CTLR1 &= ~B5; }
INLINE void ADC1_EndIntDisable(void) { ADCx_EndIntDisable(ADC1); }

// Enable/Disable analog watchdog interrupt (default disabled; register CTLR1.AWDIE)
INLINE void ADCx_AWDIntEnable(ADC_t* adc) { adc->CTLR1 |= B6; }
INLINE void ADC1_AWDIntEnable(void) { ADCx_AWDIntEnable(ADC1); }

INLINE void ADCx_AWDIntDisable(ADC_t* adc) { adc->CTLR1 &= ~B6; }
INLINE void ADC1_AWDIntDisable(void) { ADCx_AWDIntDisable(ADC1); }

// Enable/Disable interrupt on end of conversion of injection group (default disabled; register CTLR1.JEOCIE)
INLINE void ADCx_JEndIntEnable(ADC_t* adc) { adc->CTLR1 |= B7; }
INLINE void ADC1_JEndIntEnable(void) { ADCx_JEndIntEnable(ADC1); }

INLINE void ADCx_JEndIntDisable(ADC_t* adc) { adc->CTLR1 &= ~B7; }
INLINE void ADC1_JEndIntDisable(void) { ADCx_JEndIntDisable(ADC1); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_ADC_H

#endif // USE_ADC
