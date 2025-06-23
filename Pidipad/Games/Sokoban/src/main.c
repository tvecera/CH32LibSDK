
// ****************************************************************************
//
//                              Sokoban game
//
// ****************************************************************************

#include "../include.h"

// wait for a character
char WaitChar()
{
	char c;
	while ((c = KeyGet()) == NOKEY) {}
	return c;
}

int main(void)
{
	// use custom font
	DrawFont = FontBold8x8Custom; // custom font

	// display splash screen
	u8 key;
	KeyWaitNoPressed();
	memcpy(FrameBuf, ImgIntro, sizeof(ImgIntro));
	memcpy(AttrBuf, ImgIntro_Attr, sizeof(ImgIntro_Attr));
	while ((key = KeyGet()) == NOKEY) {}
	if (key == KEY_Y) ResetToBootLoader();

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	// select level
	while (LevSelect())
	{
		// game loop
		GameLoop();
	}
	ResetToBootLoader();
}
