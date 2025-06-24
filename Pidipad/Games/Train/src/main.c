
// ****************************************************************************
//
//                                  Train game
//
// ****************************************************************************

#include "../include.h"

int main(void)
{
	int i;

	// display splash screen
	u8 key;
	KeyWaitNoPressed();
	memcpy(FrameBuf, ImgIntro, sizeof(ImgIntro));
	memcpy(AttrBuf, ImgIntro_Attr, sizeof(ImgIntro_Attr));
	while ((key = KeyGet()) == NOKEY) {}
	if (key == KEY_Y) ResetToBootLoader();
	DrawClear();

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

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
