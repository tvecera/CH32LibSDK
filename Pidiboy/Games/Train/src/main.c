
// ****************************************************************************
//
//                                  Train game
//
// ****************************************************************************

#include "../include.h"

int main(void)
{
	char ch;
	int i;

	// display splash screen
	DrawImgBg(ImgIntro, 0, 0, WIDTH, HEIGHT, WIDTHBYTE);
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

	// set first scene
	Level = LEVFIRST;
	Score = 0;

	while (True)
	{
		// demo
		i = Level; // save current level

		while (True)
		{
			// play demo scene 0
			Level = 0;
			if (HelpLevel()) break; // break

#ifdef AUTOMODE // automode - run levels to check solutions (help completes scene)
			break;
#endif

			// play random scene
			Level = RandU8MinMax(1, LEVNUM);
			if (HelpLevel()) break; // break
		}

		if (KeyPressed(KEY_Y)) ResetToBootLoader();

		// return current level
		Level = i;
		InitLevel();

		// main loop
		while (True)
		{
			// game loop - called every single game step
			if (GameLoop()) break;

			// wait step
			WaitStep();
		}
	}
}
