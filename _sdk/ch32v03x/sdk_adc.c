
// ****************************************************************************
//
//                         ADC Analog-to-digital converter
//
// ****************************************************************************

#include "../_include.h"

#if USE_ADC		// 1=use ADC peripheral

// Set sampling time 0..7 for channel 0-15 (default 0 = 4 cycles; registers SAMPTR1, SAMPTR2)
//  0 ... 4 cycles
//  1 ... 5 cycles
//  2 ... 6 cycles
//  3 ... 7 cycles
//  4 ... 8 cycles
//  5 ... 9 cycles
//  6 ... 10 cycles
//  7 ... 11 cycles
void ADCx_SampTime(ADC_t* adc, int chan, int samp)
{
	if (chan < 10)
	{
		chan *= 3;
		adc->SAMPTR2 = (adc->SAMPTR2 & ~(7 << chan)) | (samp << chan);
	}
	else
	{
		chan = (chan - 10) * 3;
		adc->SAMPTR1 = (adc->SAMPTR1 & ~(7 << chan)) | (samp << chan);
	}
}

// Select ADC channel chan=0..15 to be converted on the rule sequence seq=1..16 (default 0; register RSQR1/RSQR2/RSQR3)
void ADCx_RSeq(ADC_t* adc, int seq, int chan)
{
	if (seq <= 6)
	{
		seq = (seq - 1) * 5;
		adc->RSQR3 = (adc->RSQR3 & ~(0x1f << seq)) | (chan << seq);
	}
	else if (seq <= 12)
	{
		seq = (seq - 7) * 5;
		adc->RSQR2 = (adc->RSQR2 & ~(0x1f << seq)) | (chan << seq);
	}
	else
	{
		seq = (seq - 13) * 5;
		adc->RSQR1 = (adc->RSQR1 & ~(0x1f << seq)) | (chan << seq);
	}
}

// Initialize ADC single conversion mode
// - Get conversion result with ADCx_GetSingle().
void ADCx_InitSingle(ADC_t* adc)
{
	// reset ADC module
	if (adc == ADC1)
	{
		// enable clock for ADC
		RCC_ADC1ClkEnable();

		// reset ADC module
		RCC_ADC1Reset();
	}

	// set clock prescaler (ADCCLK_DIV = 1..16)
	ADCx_ClkDiv(adc, ADCCLK_DIV);

	// select software trigger of rule channel conversion
	ADCx_ExtSel(adc, ADC_EXT_SWSTART);

	// enable externally triggered conversion for rule channels
	ADCx_ExtTrigEnable(adc);

	// set sampling time 4-11 cycles of all channels
	int n = 4 + (ADCCLK_DIV-1)/2; // 3 bits
	n = n | (n << 3); // 6 bits
	n = n | (n << 6); // 12 bits
	n = n | (n << 12); // 24 bits
	n = n | (n << 24); // 30 bits
	adc->SAMPTR1 = n;
	adc->SAMPTR2 = n;

	// set number of channels to be converted in a regular channel conversion sequence
	ADCx_RNum(adc, 1);

	// wake-up ADC
	ADCx_Enable(adc);
	WaitMs(1);
}

// Get single conversion (chan = channel 0..15)
// - Conversion must be initialized using ADCx_InitSingle().
// - Before use, set the appropriate GPIO pin to analog input mode GPIO_MODE_AIN.
u16 ADCx_GetSingle(ADC_t* adc, int chan)
{
	// select channel to be converted
	ADCx_RSeq(adc, 1, chan);

	// software start conversion
	ADCx_SwStart(adc);

	// wait end of conversion
	while (!ADCx_EndGet(adc)) {}

	// get conversion result (and clear "end of conversion" flag)
	return ADCx_Data(adc);
}

// Get sum of multiple single conversions (chan = channel 0..15, num = number of samples)
// - Conversion must be initialized using ADCx_InitSingle().
// - Before use, set the appropriate GPIO pin to analog input mode GPIO_MODE_AIN.
int ADCx_GetSingleMul(ADC_t* adc, int chan, int num)
{
	int sum = 0;
	for (; num > 0; num--) sum += ADCx_GetSingle(adc, chan);
	return sum;
}

#endif // USE_ADC
