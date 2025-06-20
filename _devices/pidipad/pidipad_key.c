
// ****************************************************************************
//
//                             PidiPad - Keyboard
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
	// switch PC7 to GPIO mode, if not already set
	if (((OB->USER >> 3) & 3) != 3)
	{
		OB_t ob;
		Flash_OBRead(&ob);	// read option bytes
		ob.USER |= B3|B4;	// disable RESET function
		Flash_OBWrite(&ob);	// write new option bytes
	}
#endif

	// initialize key ports
//  1: PA5 ... RIGHT
//  2: PA0 ... UP
//  3: PD0 ... LEFT
//  4: PA4 ... DOWN
//  5: PD7 ... A
//  6: PD2 ... B
//  7: PC4 ... X
//  8: PC5 ... Y
	RCC_PAClkEnable();
	RCC_PCClkEnable();
	RCC_PDClkEnable();

	GPIO_Mode(PA5, GPIO_MODE_IN_PU);
	GPIO_Mode(PA0, GPIO_MODE_IN_PU);
	GPIO_Mode(PD0, GPIO_MODE_IN_PU);
	GPIO_Mode(PA4, GPIO_MODE_IN_PU);
	GPIO_Mode(PD7, GPIO_MODE_IN_PU);
	GPIO_Mode(PD2, GPIO_MODE_IN_PU);
	GPIO_Mode(PC4, GPIO_MODE_IN_PU);
	GPIO_Mode(PC5, GPIO_MODE_IN_PU);
}

// terminate keyboard
void KeyTerm()
{
	GPIO_PinReset(PA5);
	GPIO_PinReset(PA0);
	GPIO_PinReset(PD0);
	GPIO_PinReset(PA4);
	GPIO_PinReset(PD7);
	GPIO_PinReset(PD2);
	GPIO_PinReset(PC4);
	GPIO_PinReset(PC5);
}

#endif // USE_KEY
