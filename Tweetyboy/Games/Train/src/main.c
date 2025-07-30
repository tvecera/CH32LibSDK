
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
	DrawClear();
	DrawMonoImg(ImgTitle, (WIDTH-144)/2, 0, 144, 32, 18, COL_CYAN);
	DrawImg(ImgIntro, 0, HEIGHT-46, 0, 0, WIDTH, 46, WIDTHBYTE);
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
