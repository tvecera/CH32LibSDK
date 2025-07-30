
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
	char ch;

restart:

	// display splash screen
	DrawImg(ImgIntro, 0, 0, 0, 0, WIDTH, HEIGHT, WIDTHBYTE);
	DispUpdate();
	WaitMs(200);
	KeyFlush();
	while ((ch = KeyGet()) == NOKEY) {}
#if USE_SCREENSHOT	// 1=use screen shot
	if (ch == KEY_Y) ScreenShot();
#endif
	if (ch == KEY_Y) ResetToBootLoader();

	// randomize
	RandSeed += Time();

	// clear display
	DrawClear();

	while (True)
	{
		// select level
		if (!LevSelect()) goto restart;

		// game loop
		GameLoop();
	}
}
