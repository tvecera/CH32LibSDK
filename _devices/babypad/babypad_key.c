
// ****************************************************************************
//
//                             BabyPad - Keyboard
//
// ****************************************************************************

#include "../../includes.h"

#if USE_KEY		// 1=use keyboard support

// keyboard buffer
volatile u8 KeyReleaseCnt[KEY_NUM*2]; // key release counter + key repeat counter; >0 if key is pressed (without NOKEY)
u8 KeyBuf[KEYBUF_SIZE];		// keyboard buffer
volatile u8 KeyWriteOff = 0;	// write offset to keyboard buffer
volatile u8 KeyReadOff = 0;	// read offset from keyboard buffer

// get button from keyboard buffer KEY_* (returns NOKEY if no key)
u8 KeyGet()
{
	// check if keyboard buffer is empty
	u8 r = KeyReadOff;
	cb();
	if (r == KeyWriteOff) return NOKEY;

	// get key from keyboard buffer
	u8 ch = KeyBuf[r];

	// write new read offset
	r++;
	if (r >= KEYBUF_SIZE) r = 0;
	KeyReadOff = r;

	return ch;
}

// key flush
void KeyFlush()
{
	while (KeyGet() != NOKEY) {}
}

// check no pressed key
Bool KeyNoPressed()
{
	int i;
	for (i = 0; i < KEY_NUM; i++) if (KeyReleaseCnt[i] > 0) return False;
	return True;
}

// wait for no key pressed
void KeyWaitNoPressed()
{
	while (!KeyNoPressed()) {}
}

// Initialize keyboard service (should be called before display init)
void KeyInit()
{
#if USE_FLASH		// 1=use Flash programming
	// switch PD7 to GPIO mode, if not already set
	if (((OB->USER >> 3) & 3) != 3)
	{
		OB_t ob;
		Flash_OBRead(&ob);	// read option bytes
		ob.USER |= B3|B4;	// disable RESET function
		Flash_OBWrite(&ob);	// write new option bytes
	}
#endif

	// initialize key ports
//  1: PC2 ... RIGHT
//  2: PD6 ... UP
//  3: PC1 ... LEFT
//  4: PD5 ... DOWN
//  5: PC3 ... A
//  6: PC0 ... B
//  7: PD7 ... X
	RCC_PCClkEnable();
	RCC_PDClkEnable();

	GPIO_Mode(PC2, GPIO_MODE_IN_PU);
	GPIO_Mode(PD6, GPIO_MODE_IN_PU);
	GPIO_Mode(PC1, GPIO_MODE_IN_PU);
	GPIO_Mode(PD5, GPIO_MODE_IN_PU);
	GPIO_Mode(PC3, GPIO_MODE_IN_PU);
	GPIO_Mode(PC0, GPIO_MODE_IN_PU);
	GPIO_Mode(PD7, GPIO_MODE_IN_PU);
}

// terminate keyboard
void KeyTerm()
{
	GPIO_PinReset(PC2);
	GPIO_PinReset(PD6);
	GPIO_PinReset(PC1);
	GPIO_PinReset(PD5);
	GPIO_PinReset(PC3);
	GPIO_PinReset(PC0);
	GPIO_PinReset(PD7);
}

#endif // USE_KEY
