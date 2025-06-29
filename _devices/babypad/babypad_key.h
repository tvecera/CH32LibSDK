
// ****************************************************************************
//
//                             BabyPad - Keyboard
//
// ****************************************************************************
// The button input uses the following pins:
//  1: PC2 ... RIGHT
//  2: PD6 ... UP
//  3: PC1 ... LEFT
//  4: PD5 ... DOWN
//  5: PC3 ... A
//  6: PC0 ... B
//  7: PD7 ... X
//
// Input from the buttons is handled during the VGA rendering service,
// in the last VSYNC line.
//
// The order of the KEY_* button codes corresponds to the order of the arrow 
// characters in the font.

#if USE_KEY		// 1=use keyboard support

#ifndef _BABYPAD_KEY_H
#define _BABYPAD_KEY_H

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
#define KEY_X		7	// X

#define NOKEY		0	// no key

#define KEY_NUM		7	// number of buttons

// keyboard buffer
// If you change the settings, also check the code for reading the buttons in babypad_vga_asm.S.
#define KEYBUF_SIZE	8	// size of keyboard buffer
extern volatile u8 KeyReleaseCnt[KEY_NUM*2]; // key release counter + key repeat counter; >0 if key is pressed (without NOKEY)
extern u8 KeyBuf[KEYBUF_SIZE];	// keyboard buffer
extern volatile u8 KeyWriteOff;	// write offset to keyboard buffer
extern volatile u8 KeyReadOff;	// read offset from keyboard buffer

// check if button KEY_* is currently pressed
INLINE Bool KeyPressed(int key) { return KeyReleaseCnt[key-1] > 0; }

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

#endif // _BABYPAD_KEY_H

#endif // USE_KEY
