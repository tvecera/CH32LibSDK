
// ****************************************************************************
//
//                        BabyPad - VGA display driver
//
// ****************************************************************************

// project configuration
#include "config.h"		// project configuration
#include "_config.h"		// default configuration

#if USE_DISP		// 1=use display support

#define	PERIPH_BASE		0x40000000	// Peripheral base address in the alias region
#define APB1PERIPH_BASE		PERIPH_BASE			// 0x40000000
#define APB2PERIPH_BASE		(PERIPH_BASE + 0x10000)		// 0x40010000

#define GPIOC_BASE		(APB2PERIPH_BASE + 0x1000)	// PC port base (0x40011000)
#define GPIOD_BASE		(APB2PERIPH_BASE + 0x1400)	// PD port 0x40011400
#define SPI1_BASE		(APB2PERIPH_BASE + 0x3000)	// SPI1 base (0x40013000)
#define TIM1_BASE		(APB2PERIPH_BASE + 0x2C00)	// TIM1 0x40012C00
#define TIM2_BASE		(APB1PERIPH_BASE + 0x0000)	// TIM2 0x40000000

#define SPI_DATAR_OFF		0x0C		// data register offset
#define GPIO_INDR_OFF		0x08		// GPIO input register offset
#define GPIO_BSHR_OFF		0x10		// GPIO bit set output data register
#define GPIO_BCR_OFF		0x14		// GPIO bit clear output data register
#define TIM_INTFR_OFF		0x10		// timer interrupt register offset
#define TIM_CCER_OFF		0x20		// timer compare register
#define TIM_CNT_OFF		0x24		// timer counter register offset
#define TIM_ATRLR_OFF		0x2C		// ATRLR timer auto-reload value register
#define TIM_CH4CVR_OFF		0x40		// CH4CVR timer compare/capture register 4

.global FrameBuf			// (u8[]) display graphics buffer
#if VMODE == 2
.global AttrBuf				// (u8[]) display attribute buffer (color of 2 pixels: 1st pixels in bits 1..3, 2nd pixel in bits 5..7)
.global AttrBufAddr			// (u8*) current pointer to attribute buffer
#endif
.global DispLine			// (u32) current display line
.global DispFrame			// (u32) current frame
.global FrameBufAddr			// (u8*) current pointer to graphics buffer
.global DispTimTest			// test - get TIM-CNT value at start of image
.global DrawFont			// (u8*) current pointer to font
#define KEY_NUM			7	// number of buttons

#define KEYBUF_SIZE		8	// size of keyboard buffer
// If you change the settings, also check this in babypad_key.h.
.global KeyReleaseCnt			// (u8[]) key release counter + key repeat counter; >0 if key is pressed (without NOKEY)
.global KeyBuf				// (u8[]) keyboard buffer
.global KeyWriteOff			// (u8) write offset to keyboard buffer
.global KeyReadOff			// (u8) read offset from keyboard buffer

.global SoundMelodyPtr			// (sMelodyNote*) pointer to current melody
.global SoundMelodyNext			// (sMelodyNote*) pointer to next melody
.global SoundMelodyLen			// (s16) remaining length of current tone (0 = no melody, -1 = start next melody)

// 32-bit NOP
.macro nopw
	.word	0x13
.endm

// multiply 16-bit NOP
.macro nop_rep num
.rept \num
	nop
.endr
.endm

// multiply 32-bit NOP
.macro nopw_rep num
.rept \num
	nopw
.endr
.endm

	.section .time_critical, "ax"

// ============================================================================
//         1: graphics mode 160x120 pixels, required memory 2400 B
// ============================================================================

#if VMODE == 1

// Registers:
//  A0-A3, T0-T1 =
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current scanline
//  RA = return address
// - continue to TIM2_IRQHandler4

.macro vga_draw

	// test - get TIM-CNT value at start of image (should be 288-5=283)
//	li	a0,TIM2_BASE		// A0 <- Timer 2 base
//	lw	a0,TIM_CNT_OFF(a0)	// load timer counter
//	la	a1,DispTimTest
//	sw	a0,0(a1)		// save value

	// prepare number of samples -> A3
	li	a3,20			// -5: [1]

	.align	2,,			// -4: [1]

// [64 loop] send sample ... This address must be aligned

// Registers:
//  A0 = temporary with sample
//  A1, T0-T1 =
//  A2 = delay counter
//  A3 = loop counter
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current scanline
//  RA = return address

	// [5] send sample to SPI
2:	lbu	a0,0(a4)		// -3: [2] A0 <- load sample
	addi	a4,a4,1			// -1: [1] increase sample address
	sw	a0,SPI_DATAR_OFF(a5)	// 0: [2] send sample to SPI

	// [55] delay
	li	a2,14			// +2: [1]
	.align	2,,			// +3: [0]
	// [13*4+2=54] loop
1:	addi	a2,a2,-1		// [1]
	bnez	a2,1b			// [1 pass, 3 loop] loop ... 4 clock cycles per loop

	// [2,4] loop counter
	addi	a3,a3,-1		// +57: [1]
	bnez	a3,2b			// +58: [1,3] 
					// +59 (pass), +61 (loop)

	// send black color
	sw	x0,SPI_DATAR_OFF(a5)	// send black sample to SPI

	// save new sample pointer only every 4th scanline (divide line by 4)
	li	a1,3			// to compare
	and	a0,t2,a1		// get lowest 2 bits
	bne	a0,a1,1f		// skip if not correct scanline
	la	a2,FrameBufAddr
	sw	a4,0(a2)		// save new pointer

1:	j	TIM2_IRQHandler4

.endm

#endif // VMODE == 1

// ============================================================================
//    2: graphics mode 160x120 pixels with gray 1-bit attributes 2x2 pixels
// ============================================================================

#if VMODE == 2

// Registers:
//  A0-A2, T0-T1 =
//  A3 = pointer to attribute buffer
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current scanline
//  RA = return address
// - continue to TIM2_IRQHandler4

.macro vga_draw

	// test - get TIM-CNT value at start of image (should be 288-24=264)
//	li	a0,TIM2_BASE		// A0 <- Timer 2 base
//	lw	a0,TIM_CNT_OFF(a0)	// load timer counter
//	la	a1,DispTimTest
//	sw	a0,0(a1)		// save value

	// prepare pointer to odd/even table -> T0
	la	t0,DispFrame		// -24: [2]
	lw	t0,0(t0)		// -22: [2] T0 <- current frame counter
	la	a0,VMode2OddEven	// -20: [2]
	add	t0,t0,t2		// -18: [1] T0 <- current frame + current scanline
	andi	t0,t0,1			// -17: [2] T0 <- odd/even flag
	slli	t0,t0,4			// -15: [1] T0 <- odd/even flag * 16 (= offset in odd/even table)
	add	t0,t0,a0		// -14: [1] T0 <- pointer to odd/even table

	// prepare number of samples -> T1
	li	t1,10			// -13: [1]
	.align	2,,			// -12: [1]

// [128 loop] send 2 samples ... This address must be aligned

// Registers:
//  A0 = temporary with sample
//  A1 = temporary with attribute
//  A2 = temporary, delay counter
//  A3 = pointer to attribute buffer
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T0 = pointer to odd/even table
//  T1 = loop counter
//  T2 = current scanline
//  RA = return address

	// [4] load 1st sample -> A0, load attribute -> A1
2:	lbu	a0,0(a4)		// -11: [2] A0 <- load sample
	lbu	a1,0(a3)		// -9: [2] A1 <- load attribute

	// [7] mask by odd/even table for 1st pixel
	srli	a2,a1,4			// -7: [2] A2 <- high 4 bits of the attributes
	add	a2,a2,t0		// -5: [1] A2 <- pointer to odd/even table
	lbu	a2,0(a2)		// -4: [2] A2 <- odd/even mask
	addi	a3,a3,1			// -2: [1] increase attribute address
	and	a0,a0,a2		// -1: [1] A0 <- mask sample with odd/even	

	// [2] send sample of 1st pixel to SPI
	sw	a0,SPI_DATAR_OFF(a5)	// 0: [2] send sample to SPI

	// [2] load 2nd sample -> A0, load attribute -> A1
	lbu	a0,1(a4)		// +2: [2] A0 <- load sample

	// [6] mask by odd/even table for 2nd pixel
	andi	a1,a1,0x0f		// +4: [1] A1 <- low 4 bits of the attributes
	add	a1,a1,t0		// +5: [1] A1 <- pointer to odd/even table
	lbu	a1,0(a1)		// +6: [2] A1 <- odd/even mask
	addi	a4,a4,2			// +8: [1] increase sample address
	and	a0,a0,a1		// +9: [1] A0 <- mask sample with odd/even	

	// [52] delay
	li	a2,13			// +10: [1]
	.align	2,,			// +11: [0]
	// [12*4+2=50] loop
1:	addi	a2,a2,-1		// +11: [1]
	bnez	a2,1b			// [1 pass, 3 loop] loop ... 4 clock cycles per loop
	j	1f			// +61: [3]

	// [2] send sample of 2nd pixel to SPI
1:	sw	a0,SPI_DATAR_OFF(a5)	// +64: [2] send sample to SPI

	// [47] delay
	li	a2,11			// +66: [1]
	.align	2,,			// +67: [1]
	// [10*4+2=42] loop
1:	addi	a2,a2,-1		// +68: [1]
	bnez	a2,1b			// [1 pass, 3 loop] loop ... 4 clock cycles per loop
	j	1f			// +110: [3]

	// [2,4] loop counter
1:	addi	t1,t1,-1		// +113: [1]
	bnez	t1,2b			// +114: [1,3] 
					// +115 (pass), +117 (loop)

	// send black color
	sw	x0,SPI_DATAR_OFF(a5)	// send black sample to SPI

	// save new sample pointer only every 4th scanline (divide line by 4)
	li	a1,3			// to compare
	and	a0,t2,a1		// get lowest 2 bits
	bne	a0,a1,1f		// skip if not correct scanline
	la	a2,FrameBufAddr
	sw	a4,0(a2)		// save new pointer

	// save new attribute pointer only every 8th scanline (divide line by 2*4=8)
1:	li	a1,0x07			// to compare
	and	a0,t2,a1		// get lowest 3 bits
	bne	a0,a1,1f		// skip if not correct scanline
	la	a2,AttrBufAddr
	sw	a3,0(a2)		// save new pointer

1:	j	TIM2_IRQHandler4

.endm

#endif // VMODE == 2

// ============================================================================
// 3: text mode 40x30 characters of 8x8 pixels (resolution 320x240 pixels, pseudographics
//    80x60 pixels), required memory 1200 bytes, font 8x8 pixels 2048 bytes in Flash
// ============================================================================

#if VMODE == 3

// Registers:
//  A0-A3, T0-T1 =
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current scanline
//  RA = return address
// - continue to TIM2_IRQHandler4

.macro vga_draw

	// test - get TIM-CNT value at start of image (should be 288-19=269)
//	li	a0,TIM2_BASE		// A0 <- Timer 2 base
//	lw	a0,TIM_CNT_OFF(a0)	// load timer counter
//	la	a1,DispTimTest
//	sw	a0,0(a1)		// save value

	// prepare pointer to font -> A1
	la	a1,DrawFont		// -19: [2] address of pointer
	lw	a1,0(a1)		// -17: [2] A1 <- get pointer to font

	// shift font to current font line -> A1
	andi	a0,t2,0x0e		// -15: [2] A0 <- lowest 3 bits of the line * 2 (ignore bit 0, we use 2 scanlines per 1 graphics line)
	slli	a0,a0,7			// -13: [1] A0 <- offset of the line (1 line = 256 bytes)
	add	a1,a1,a0		// -12: [1] A1 <- shift pointer to current line of the font

	// prepare number of samples -> A3
	li	a3,40			// -11: [1]

	.align	2,,			// -10: [1]

// [32 loop] send sample ... This address must be aligned

// Registers:
//  A0 = temporary with sample
//  A1 = pointer to font
//  A2 = delay counter
//  A3 = loop counter
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T0-T1 =
//  T2 = current scanline
//  RA = return address

	// [11] load next character, load font sample and send sample to SPI
2:	lbu	a0,0(a4)		// -9: [2] A0 <- load text character
	addi	a4,a4,1			// -7: [1] increase sample address
	add	a0,a0,a1		// -6: [1] A0 <- address of the sample in the font
	lbu	a0,0(a0)		// -5: [4] A0 <- load font sample
	addi	a3,a3,-1		// -1: [1] decrease loop counter
	sw	a0,SPI_DATAR_OFF(a5)	// 0: [2] send sample to SPI

	// [18] delay
	li	a2,4			// +2: [1]
	.align	2,,			// +3: [1]
	// [3*4+2=14] loop
1:	addi	a2,a2,-1		// +4: [1]
	bnez	a2,1b			// [1 pass, 3 loop] loop ... 4 clock cycles per loop
	nop_rep	2			// +18: [2]

	// [1,3] loop counter
	bnez	a3,2b			// +20: [1,3] 
					// +21 (pass), +23 (loop)

	// send black color
	sw	x0,SPI_DATAR_OFF(a5)	// send black sample to SPI

	// save new sample pointer only every 16th scanline (divide line by 2)
	li	a1,0x0f			// to compare
	and	a0,t2,a1		// get lowest 4 bits
	bne	a0,a1,1f		// skip if not correct scanline
	la	a2,FrameBufAddr
	sw	a4,0(a2)		// save new pointer

1:	j	TIM2_IRQHandler4

.endm

#endif // VMODE == 3

// ============================================================================
// 4: text mode 80x30 characters of 8x8 pixels (resolution 640x240 pixels, pseudographics
//    160x60 pixels), required memory 2400 bytes, font 8x8 pixels 2048 bytes in Flash
// ============================================================================

#if VMODE == 4

// Registers:
//  A0-A3, T0-T1 =
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current scanline
//  RA = return address
// - continue to TIM2_IRQHandler4

.macro vga_draw

	// test - get TIM-CNT value at start of image (should be 288-19=269)
//	li	a0,TIM2_BASE		// A0 <- Timer 2 base
//	lw	a0,TIM_CNT_OFF(a0)	// load timer counter
//	la	a1,DispTimTest
//	sw	a0,0(a1)		// save value

	// prepare pointer to font -> A1
	la	a1,DrawFont		// -19: [2] address of pointer
	lw	a1,0(a1)		// -17: [2] A1 <- get pointer to font

	// shift font to current font line -> A1
	andi	a0,t2,0x0e		// -15: [2] A0 <- lowest 3 bits of the line * 2 (ignore bit 0, we use 2 scanlines per 1 graphics line)
	slli	a0,a0,7			// -13: [1] A0 <- offset of the line (1 line = 256 bytes)
	add	a1,a1,a0		// -12: [1] A1 <- shift pointer to current line of the font

	// prepare number of samples -> A3
	li	a3,80			// -11: [1]

	.align	2,,			// -10: [1]

// [32 loop] send sample ... This address must be aligned

// Registers:
//  A0 = temporary with sample
//  A1 = pointer to font
//  A2 = delay counter
//  A3 = loop counter
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T0-T1 =
//  T2 = current scanline
//  RA = return address

	// [11] load next character, load font sample and send sample to SPI
2:	lbu	a0,0(a4)		// -9: [2] A0 <- load text character
	addi	a4,a4,1			// -7: [1] increase sample address
	add	a0,a0,a1		// -6: [1] A0 <- address of the sample in the font
	lbu	a0,0(a0)		// -5: [4] A0 <- load font sample
	addi	a3,a3,-1		// -1: [1] decrease loop counter
	sw	a0,SPI_DATAR_OFF(a5)	// 0: [2] send sample to SPI

	// [2] delay
	nop_rep	2			// +2: [2]

	// [1,3] loop counter
	bnez	a3,2b			// +4: [1,3] 
					// +5 (pass), +7 (loop)

	// send black color
	sw	x0,SPI_DATAR_OFF(a5)	// send black sample to SPI

	// save new sample pointer only every 16th scanline (divide line by 2)
	li	a1,0x0f			// to compare
	and	a0,t2,a1		// get lowest 4 bits
	bne	a0,a1,1f		// skip if not correct scanline
	la	a2,FrameBufAddr
	sw	a4,0(a2)		// save new pointer

1:	j	TIM2_IRQHandler4

.endm

#endif // VMODE == 4

// ============================================================================
//                   Timer handler - draw VGA graphics line
// ============================================================================
// Hardware prologie/epilogue HPE (2 levels):
//  Interrupt saves working registers: x1 (ra), x5-x7 (t0-t2), x10-x15 (a0-a5)

	.align  4,,
.global TIM2_IRQHandler
TIM2_IRQHandler:

// ==== Clear interrupt flag
// Registers:
//  A0-A5, T0-T2 =
//  RA = return address

	// clear update interrupt flag (Timer 2, channel 2)
	li	a2,TIM2_BASE		// A2 <- Timer 2 base
	lw	a1,TIM_INTFR_OFF(a2)	// get INTFR register
	lw	a0,TIM_CNT_OFF(a2)	// A0 <- load timer counter
	andi	a1,a1,~(1<<2)		// clear interrupt flag
	sw	a1,TIM_INTFR_OFF(a2)	// set INTFR register

// ==== Time synchronization
// Registers:
//  A0 = Timer 2 counter value
//  A1-A5,T0-T2 =
//  RA = return address

	//  The exact interruption time may vary within a range of 1-7 processor clock
	//  cycles due to the varying length of instructions in the main program.

	// >>>>>>>> SETUP
	//  If you change Timer 2 channel 2 compare value, setup this
	//  correction - select 0 to 7 to minimise display noise.
#if VMODE == 1
	addi	a0,a0,0			// time correction: select 0 to 7
#elif VMODE == 2
	addi	a0,a0,7			// time correction: select 0 to 7
#elif VMODE == 3
	addi	a0,a0,6			// time correction: select 0 to 7
#elif VMODE == 4
	addi	a0,a0,6			// time correction: select 0 to 7
#endif
	// <<<<<<<<

	andi	a0,a0,7			// mask lower 3 bits of the timer counter
	slli	a0,a0,1			// timer * 2 (because jump to NOPs is multiply of 2 bytes)
	la	a2,TIM2_IRQHandler_sync	// table of 7x NOP instruction
	add	a0,a0,a2		// A0 <- jump address
	
	.align  2,,
	jr	a0			// jump to NOP and delay additional 'x' clock cycles
	.align	2,,

TIM2_IRQHandler_sync:
	nop_rep	7			// 0..7: +7..+0 clk

// ==== Get current scanline

	// load current scanline -> T2
	la	a0,DispLine
	lw	t2,0(a0)

// Registers:
//  A0-A5, T0-T1 =
//  T2 = current scanline
//  RA = return address

	// check active image
	li	a0,480
	bge	t2,a0,TIM2_IRQHandler2 // not active image

// ==== active image

	// load pointer to graphics buffer -> A4
	la	a4,FrameBufAddr
	lw	a4,0(a4)

#if VMODE == 2
	// load pointer to attribute buffer
	la	a3,AttrBufAddr
	lw	a3,0(a3)
#endif

	// pointer to SPI1 base -> A5
	li	a5,SPI1_BASE

// Registers:
//  A0-A2, T0-T1 =
//  A3 = pointer to attribute buffer
//  A4 = pointer to source graphics buffer
//  A5 = SPI
//  T2 = current line
//  RA = return address

	// draw VGA line (continue to TIM2_IRQHandler4)
	vga_draw

// ==== VSYNC
// Registers:
//  A0-A5, T0-T1 =
//  T2 = current scanline
//  RA = return address

	.align  2,,

TIM2_IRQHandler2:

	// check front porch
	li	a1,480+10
	blt	t2,a1,TIM2_IRQHandler4	// front porch - black line

	// VSYNC
	li	a3,GPIOC_BASE		// A3 <- port C
	li	a0,1<<7			// A0 <- pin 7 of port C

// Registers:
//  A1-A2,A4,A5, T0-T1 =
//  A0 = mask of pin 7
//  A3 = GPIOC base
//  T2 = current scanline
//  RA = return address

	li	a1,480+10+2 - 1		// A1 <- index of last VSYNC scanline
	bgt	t2,a1,TIM2_IRQHandler3	// not VSYNC, serve back porch

	// start VSYNC pulse (send 0 to PC7)
	sw	a0,GPIO_BCR_OFF(a3)	// send 0 to PC7
	bne	t2,a1,TIM2_IRQHandler5	// skip if not last line of VSYNC

// ==== Buttons
// Registers:
//  A0-A5, T0-T1 =
//  T2 = current scanline
//  RA = return address

#if USE_KEY		// 1=use keyboard support

	// prepare to scan buttons (decreasing from 6 to 1)
	la	a1,ButKeyTab		// A1 <- pointer to button table
	li	a2,7			// A2 <- button index
	la	a3,KeyReleaseCnt+6	// A3 <- pointer to key release counter table + key repeat counter

	// prepare port base and pin mask
2:	lw	a4,0(a1)		// A4 <- port base
	lw	a5,4(a1)		// A5 <- pin mask

// Registers:
//  A0 =
//  A1 = pointer to button table
//  A2 = button index
//  A3 = pointer to key release counter + key repeat counter
//  A4 = ...port base
//  A5 = ...pin mask
//  T0 =
//  T1 =
//  T2 = current scanline
//  RA = return address

	// get input, check if button is pressed
	lw	a0,GPIO_INDR_OFF(a4)	// A0 <- get input port
	and	a0,a0,a5		// A0 <- check input pin, 0=pressed
	bnez	a0,4f			// button is not pressed

	// Button is pressed: release counter -> A0; is pressed for the first time?
	lbu	a0,0(a3)		// A0 <- get release counter
	li	a4,KEYCNT_PRESS		// A4 <- first repeat is 0.3 sec
	beqz	a0,6f			// release counter is 0, pressed for the first time

	// already pressed - count repeat counter
	lbu	a0,KEY_NUM(a3)		// A0 <- get repeat counter
	addi	a0,a0,-1		// decrement repeat counter
	sb	a0,KEY_NUM(a3)		// save new repeat counter
	bnez	a0,3f			// no repeat
	li	a4,KEYCNT_REPEAT	// A4 <- repeat is 0.1 sec

// Registers:
//  A0 =
//  A1 = pointer to button table
//  A2 = button index
//  A3 = pointer to key release counter + key repeat counter
//  A4 = ...repeat counter
//  A5 =
//  T0 =
//  T1 =
//  T2 = current scanline
//  RA = return address

	// set repeat counter to "first repeat"
6:	sb	a4,KEY_NUM(a3)		// setup repeat counter

	// Button is pressed for the first time: send button to keyboard buffer
7:	la	t1,KeyWriteOff		// T1 <- keyboard buffer write offset variable
	lbu	a5,0(t1)		// A5 <- get write offset to keyboard buffer
	addi	a0,a5,1			// A0 <- shift write offset
	li	t0,KEYBUF_SIZE		// T0 <- max. buffer size
	bne	a0,t0,1f		// not end of the buffer
	li	a0,0			// A0 <- reset offset to the start of the buffer

// Registers:
//  A0 = ... new write offset to keyboard buffer
//  A1 = pointer to button table
//  A2 = button index
//  A3 = pointer to key release counter + key repeat counter
//  A4 =
//  A5 = ... old write offset to keyboard buffer
//  T0 = ... max. buffer size, read offset from keyboard buffer, keyboard buffer
//  T1 = ... keyboard buffer write offset variable
//  T2 = current scanline
//  RA = return address

1:	la	t0,KeyReadOff		// T0 <- keyboard buffer read offset
	lbu	t0,0(t0)		// T0 <- get read offset from keyboard buffer
	beq	a0,t0,3f		// skip if buffer is full

	la	t0,KeyBuf		// T0 <- keyboard buffer
	add	a5,a5,t0		// T2 <- write pointer to keyboard buffer
	sb	a2,0(a5)		// save button to keyboard buffer

	sb	a0,0(t1)		// save new write offset

	// initialize press counter (update interval = 1/60 = 17ms, release time = 50 ms = 3 steps)
3:	li	a0,KEYCNT_REL		// release interval = (4-1)*17 = 50ms
	sb	a0,0(a3)		// save new release counter

	// Button is not pressed: decrease release counter
4:	lbu	a0,0(a3)		// A0 <- get release counter
	beqz	a0,5f			// counter is already 0
	addi	a0,a0,-1		// decrease counter
	sb	a0,0(a3)		// save new release counter

// Registers:
//  A0 =
//  A1 = pointer to button table
//  A2 = button index
//  A3 = pointer to key release counter + key repeat counter
//  A4 =
//  A5 =
//  T0 =
//  T1 =
//  T2 = current scanline
//  RA = return address

	// shift to next button
5:	addi	a1,a1,8			// shift table pointer
	addi	a2,a2,-1		// shift button index
	addi	a3,a3,-1		// shift key release counter table
	bnez	a2,2b			// next button

#endif // USE_KEY

	j	TIM2_IRQHandler4

// ==== Music
// Registers:
//  A0-A5, T0-T1 =
//  T2 = current scanline
//  RA = return address

TIM2_IRQHandler5:

#if USE_SOUND		// 1=use sound support

// Registers:
//  A0 = remaining length
//  A1 = SoundMelodyLen variable
//  A2 = SoundMelodyPtr variable
//  A3 = melody pointer
//  A4 = TIM1 base
//  A5,T0-T1 =
//  T2 = current scanline
//  RA = return address

	// get tone counter
	la	a1,SoundMelodyLen
	lh	a0,0(a1)		// A0 <- remaining length
	beqz	a0,TIM2_IRQHandler4	// no melody, quit
	la	a2,SoundMelodyPtr
	li	a4,TIM1_BASE		// A4 <- Timer 1 base
	bgtz	a0,4f			// valid counter

// The audio output is via PC4 (pin 4), Timer 1 channel 4, mapping 0.

	// request to start new melody
	la	a3,SoundMelodyNext
	lw	a3,0(a3)		// A3 <- pointer to next melody
	sw	a3,0(a2)		// save new pointer
	j	5f			// start new note

	// decrease counter
4:	addi	a0,a0,-1		// shift length counter
	sh	a0,0(a1)		// save new counter
	bnez	a0,TIM2_IRQHandler4	// counter not 0, continue with this tone

	// shift current melody pointer
	lw	a3,0(a2)		// A3 <- current melody pointer
	addi	a3,a3,4			// shift melody pointer
	sw	a3,0(a2)		// save new pointer

	// start new note
5:	lhu	a0,0(a3)		// A0 <- note length
	sh	a0,0(a1)		// save counter
	beqz	a0,8f			// stop melody

	// get note divider
	lhu	a3,2(a3)		// A3 <- note divider
	beqz	a3,8f			// pause

	// set timer load
	sw	a3,TIM_ATRLR_OFF(a4)	// save auto-reload register
	srli	a3,a3,1			// divider / 2
	sw	a3,TIM_CH4CVR_OFF(a4)	// save compare register

	// enable melody
	lw	a0,TIM_CCER_OFF(a4)	// load compare register
	li	a1,1<<12
	or	a0,a0,a1		// enable channel 4
	sw	a0,TIM_CCER_OFF(a4)	// save compare register
	j	TIM2_IRQHandler4

	// stop melody
8:	lw	a0,TIM_CCER_OFF(a4)	// load compare register
	li	a1,~(1<<12)
	and	a0,a0,a1		// disable channel 4
	sw	a0,TIM_CCER_OFF(a4)	// save compare register

#endif // USE_SOUND

	j	TIM2_IRQHandler4

// ==== Back porch
// Registers:
//  A1-A2,A4,A5,T0-T1 =
//  A0 = mask of pin 7
//  A3 = GPIOC base
//  T2 = current scanline
//  RA = return address

TIM2_IRQHandler3:

	// back porch - stop VSYNC pulse (1 to PC7)
	sw	a0,GPIO_BSHR_OFF(a3)	// send 1 to PC7

// ==== Increase scanline
// Registers:
//  A0-A5, T0-T1 =
//  T2 = current scanline
//  RA = return address

TIM2_IRQHandler4:

	// inrease current scanline
	addi	t2,t2,1			// T2 <- next scanline
	li	a2,525			// A2 <- total number of scanlines
	blt	t2,a2,8f		// not total line yet

	// reset to start of image
	li	t2,0			// T2 <- new scanline = 0

	// increase current frame
	la	a0,DispFrame		// A0 <- current frame
	lw	a1,0(a0)		// A1 <- get current frame 
	addi	a1,a1,1			// increase current frame
	sw	a1,0(a0)		// save new current frame

	// reset pointers
	la	a0,FrameBuf		// A0 <- frame buffer
	la	a1,FrameBufAddr		// A1 <- frame buffer address
	sw	a0,0(a1)		// save new pointer

#if VMODE == 2
	la	a0,AttrBuf
	la	a1,AttrBufAddr
	sw	a0,0(a1)		// save new pointer
#endif

	// save new scanline
8:	la	a0,DispLine
	sw	t2,0(a0)		// save new scanline
	mret

#if VMODE == 2
	.align  2,,
// odd/even mask (0=low level flip, 1=high level not flip)
// - this table must be in faster RAM section
VMode2OddEven:
	// Odd table
	.byte	0b01010101	//  0: 0000
	.byte	0b01010111	//  1: 0001
	.byte	0b01011101	//  2: 0010
	.byte	0b01011111	//  3: 0011
	.byte	0b01110101	//  4: 0100
	.byte	0b01110111	//  5: 0101
	.byte	0b01111101	//  6: 0110
	.byte	0b01111111	//  7: 0111
	.byte	0b11010101	//  8: 1000
	.byte	0b11010111	//  9: 1001
	.byte	0b11011101	// 10: 1010
	.byte	0b11011111	// 11: 1011
	.byte	0b11110101	// 12: 1100
	.byte	0b11110111	// 13: 1101
	.byte	0b11111101	// 14: 1110
	.byte	0b11111111	// 15: 1111

	// Even table
	.byte	0b10101010	//  0: 0000
	.byte	0b10101011	//  1: 0001
	.byte	0b10101110	//  2: 0010
	.byte	0b10101111	//  3: 0011
	.byte	0b10111010	//  4: 0100
	.byte	0b10111011	//  5: 0101
	.byte	0b10111110	//  6: 0110
	.byte	0b10111111	//  7: 0111
	.byte	0b11101010	//  8: 1000
	.byte	0b11101011	//  9: 1001
	.byte	0b11101110	// 10: 1010
	.byte	0b11101111	// 11: 1011
	.byte	0b11111010	// 12: 1100
	.byte	0b11111011	// 13: 1101
	.byte	0b11111110	// 14: 1110
	.byte	0b11111111	// 15: 1111

#endif // VMODE == 2

	.section .text
	.align  2,,

// Buttons port and pin mask
ButKeyTab:
	.word	GPIOD_BASE, 1 << 7	// 7: PD7 ... X
	.word	GPIOC_BASE, 1 << 0	// 6: PC0 ... B
	.word	GPIOC_BASE, 1 << 3	// 5: PC3 ... A
	.word	GPIOD_BASE, 1 << 5	// 4: PD5 ... DOWN
	.word	GPIOC_BASE, 1 << 1	// 3: PC1 ... LEFT
	.word	GPIOD_BASE, 1 << 6	// 2: PD6 ... UP
	.word	GPIOC_BASE, 1 << 2	// 1: PC2 ... RIGHT

#endif // USE_DISP
