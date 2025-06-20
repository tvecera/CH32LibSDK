
// ****************************************************************************
//
//                              Ants game
//
// ****************************************************************************

#include "../include.h"

// draw open screen
void DrawOpen()
{
	PrintClear();

	memcpy(FrameBuf, ImgIntro, sizeof(ImgIntro));
	memcpy(AttrBuf, ImgIntro_Attr, sizeof(ImgIntro_Attr));

	PrintCol = COL_WHITE;
	PrintText(
		"\n\n\26None of the black or red ants remember\n"
		"who and when started their age-old war\n"
		"for the anthill \"At the Two Spruces\".\n"
		"In order to put an end to the eternal\n"
		"battles, they decided to build castles,\n"
		"with the help of which they would gain\n"
		"dominance over the whole territory.\n"
		"The aim of the game is to build a\n"
		"castle with a height of 100 or destroy\n"
		"the opponent's castle. Each card\n"
		"consumes certain amount of resources\n"
		"(number at the top right). The rate\n"
		"of resource generation depends on\n"
		"the amount of workers.\n\n\n"

		"\22Press key to start:\n\n"
		"\23LEFT\27 .... Play for black ants\n"
		"\23RIGHT\27 ... Play for red ants\n"
		"\23DOWN\27 .... 2 players\n"
		"\23UP\27 ...... Demo\n"
		"\23Y\27 ....... Quit\n");
}

int main(void)
{
	KeyWaitNoPressed();

	// draw open screen
	DrawOpen();

	while (True)
	{
		switch (KeyGet())
		{
		// play for black ants
		case KEY_LEFT:
			Game(False, True);
			DrawOpen();
			break;

		// play for red ants
		case KEY_RIGHT:
			Game(True, False);
			DrawOpen();
			break;

		// play 2 players
		case KEY_DOWN:
			Game(False, False);
			DrawOpen();
			break;

		// play demo
		case KEY_UP:
			Game(True, True);
			DrawOpen();
			break;

		// quit
		case KEY_Y:
			ResetToBootLoader();
			break;
		}
	}
}
