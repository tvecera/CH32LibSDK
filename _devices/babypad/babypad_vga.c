
// ****************************************************************************
//
//                        BabyPad - VGA display driver
//
// ****************************************************************************

#include "../../includes.h"

#if USE_DISP		// 1=use display support

u8 FrameBuf[FRAMESIZE];		// display graphics buffer
#if VMODE == 2
u8 AttrBuf[ATTRSIZE];		// display attribute buffer (1 bit = high level of white color)
volatile u8* AttrBufAddr;	// current pointer to attribute buffer
#endif
volatile u32 DispLine;		// current display line
volatile u32 DispFrame = 0;	// current frame
volatile u8* FrameBufAddr;	// current pointer to graphics buffer
volatile u32 DispTimTest;	// test - get TIM-CNT value at start of image

// wait for VSync scanline
void WaitVSync()
{
	while (DispIsVSync()) {}
	while (!DispIsVSync()) {}
}

// Initialize videomode
void DispInit()
{
	DispLine = 0;			// current display line
#if VMODE == 2
	AttrBufAddr = AttrBuf;		// current pointer to attribute buffer
#endif
	FrameBufAddr = FrameBuf;	// current pointer to graphics buffer

	// trim HSI oscillator to 25MHz
	RCC_HSITrim(31);

	// initialize GPIO pins (note: clocks are setup at config.h)
	// - PC0 video out, GPIO push-pull output, fast speed
	RCC_AFIClkEnable();
	RCC_PCClkEnable();
	RCC_PDClkEnable();

	// - PC7 VSYNC, GPIO push-pull output, fast speed
	GPIO_Mode(PC7, GPIO_MODE_OUT_FAST);
	// - PD4 HSYNC, TIM2 channel 1 output
	GPIO_Mode(PD4, GPIO_MODE_AF_FAST);

	// SPI setup
	RCC_SPI1ClkEnable();		// enable SPI1 clock
	RCC_SPI1Reset();		// reset SPI1 to default state
	SPI1_Baud(SPI_HDIV);		// horizontal divider
	SPI1_Data8();			// select 8-bit mode
	SPI1_MSB();			// transmit MSB (bit 7 first)
	SPI1_NSSSw();			// control NSS by software
	SPI1_NSSHigh();			// need set NSS to HIGH to enable master output
	SPI1_SSDisable();		// disable SS output
	SPI1_Bidi1();			// set 1-line bidirectional mode
	SPI1_TxBidi();			// transmit only
	SPI1_Master();			// master mode
	SPI1_Enable();			// enable SPI
	// 0 ... PC1:CS, PC5:SCK, PC7:MISO, PC6:MOSI
	GPIO_Remap_SPI1(0);		// remap MOSI to PC6
	GPIO_Mode(PC6, GPIO_MODE_AF_FAST);

	// Remap Timer 2 - mapping 0 (output channel 1 to PD4)
	// 0 ... PD4:CH1/ETR, PD3:CH2, PC0:CH3/CH1N, PD7:CH4/CH2N
	GPIO_Remap_TIM2(0);

	// Setup Timer 2 channel 1 for HSYNC signal:
	//  System frequency 50 MHz
	//  Total period 31.77756 us = 1600 clock cycles
	//  Sync 3.81331 us = 192 clock cycles
	//  Image start at 5.71996 us (= HSYNC + back porch) = 288 clock cycles (visible area is 1280 clock cycles)
// void TIM2_InitPWM(int chan, int div, int reload, int comp, Bool high);
//   chan ... channel 1..4
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
//   comp ... compare value 0..reload
//   high ... direction HIGH->LOW (or LOW->HIGH otherwise)
	TIM2_InitPWM(1, 1, 1600-1, 192, False);

	// Setup interrupt on Timer 2 channel 2 to display image
	//  160x120: 8 clock cycles per pixel, 64 clock cycles per character, active 20 characters, total 25 characters
	//  320x240: 4 clock cycles per pixel, 32 clock cycles per character, active 40 characters, total 50 characters
	//  640x240: 2 clock cycles per pixel, 16 clock cycles per character, active 80 characters, total 100 characters
#if VMODE == 1
	TIM2_Comp2(288 - 60);		// set compare value
#elif VMODE == 2
	TIM2_Comp2(288 - 83);		// set compare value
#elif VMODE == 3
	TIM2_Comp2(288 - 74);		// set compare value
#elif VMODE == 4
	TIM2_Comp2(288 - 74);		// set compare value
#endif
	TIM2_OC2Mode(TIM_COMP_FREEZE);	// set compare mode
	TIM2_OC2PreEnable();		// enable preload compare register
	TIM2_CC2Enable();		// enable compare output
	TIM2_CC2IntClr();		// clear interrupt request
	TIM2_CC2IntEnable();		// enable capture compare of channel 2
	NVIC_IRQEnable(IRQ_TIM2);	// enable interrupt servive
}

// Terminate videomode
void DispTerm()
{
	// disable capture compare interrupt (channel 2)
	NVIC_IRQDisable(IRQ_TIM2);	// disable interrupt service
	TIM2_CC2IntDisable();		// disable capture compare of channel 2
	TIM2_CC2Disable();		// disable compare output
	TIM2_CC2IntClr();		// enable capture compare of channel 2

	// disable timer
	TIM2_Disable();

	// Reset timer to default setup
	TIM2_Reset();

	// reset SPI
	RCC_SPI1Reset();		// reset SPI1 to default state

	// reset GPIO pins
	GPIO_PinReset(PC7);
	GPIO_PinReset(PD4);
	GPIO_PinReset(PC6);

	// reset HSI trim to default value 24 MHz
	RCC_HSITrim(16);
}

#endif // USE_DISP
