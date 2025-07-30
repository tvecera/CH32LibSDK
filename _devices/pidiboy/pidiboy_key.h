
// ****************************************************************************
//
//                             PidiBoy - Keyboard
//
// ****************************************************************************
// The button input uses the following pins:
//  1: PD3 .... RIGHT
//  2: PD6 .... UP
//  3: PD2 .... LEFT
//  4: PD5 .... DOWN
//  5: PD0 .... A
//  6: PD4 .... B
//  7: PD7 .... Y

#if USE_KEY		// 1=use keyboard support

#ifndef _PIDIBOY_KEY_H
#define _PIDIBOY_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

// Button codes (order corresponds to the order of the arrow characters in the font)
#define KEY_RIGHT	1	// right (character code 0x0C = arrow right, or ASCII code 0x1C)
#define KEY_UP		2	// up (character code 0x0D = arrow up, or ASCII code 0x1D)
#define KEY_LEFT	3	// left (character code 0x0E = arrow left, or ASCII code 0x1E)
#define KEY_DOWN	4	// down (character code 0x0F = arrow down, or ASCII code 0x1F)
#define KEY_A		5	// A
#define KEY_B		6	// B
#define KEY_Y		7	// Y

#define NOKEY		0	// no key

#define KEY_NUM		7	// number of buttons

// keyboard buffer
#define KEYBUF_SIZE	8	// size of keyboard buffer
extern volatile Bool KeyPressMap[KEY_NUM]; // keys are currently pressed (index = button code - 1)
extern u8 KeyBuf[KEYBUF_SIZE];	// keyboard buffer
extern volatile u8 KeyWriteOff;	// write offset to keyboard buffer
extern volatile u8 KeyReadOff;	// read offset from keyboard buffer

// check if button KEY_* is currently pressed
INLINE Bool KeyPressed(u8 key) { return KeyPressMap[key-1]; }

// scan keyboard (called from SysTick)
void KeyScan();

// get button from keyboard buffer KEY_* (returns NOKEY if no key)
u8 KeyGet();

// key flush
void KeyFlush();

// check no pressed key
Bool KeyNoPressed();

// wait for no key pressed
void KeyWaitNoPressed();

// Initialize keyboard service (should be called before display init)
void KeyInit();

// terminate keyboard
void KeyTerm();

#ifdef __cplusplus
}
#endif

#endif // _PIDIBOY_KEY_H

#endif // USE_KEY
