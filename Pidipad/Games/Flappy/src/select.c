
// ****************************************************************************
//
//                                 Select
//
// ****************************************************************************

#include "../include.h"

// colum width: 2 characters + 1 characters separator
// number of columns: 7
#define LEVW	3
#define ROWLEV	7

// print 1 level
void LevSelect1(int inx)
{
	// prepare coordinates on screen
	int y = inx / ROWLEV;		// row
	int x = (inx - y*ROWLEV)*LEVW;	// column

	// prepare cursor
	u8 inv = 0;
	if (inx == SceneInx) inv = 0x80;

	// display number
	inx += SceneBase;
	u8 ch1 = inx / 10;
	u8 ch2 = (inx - ch1*10) + '0';
	ch1 = (ch1 < 10) ? (ch1 + '0') : '*';
	PrintCharAt(ch1 + inv, x, y, COL_WHITE);
	PrintCharAt(ch2 + inv, x+1, y, COL_WHITE);
}

// select level (returns True = OK, False = Esc)
Bool LevSelect()
{
	// clear screen
	DrawClear();

	// print list of levels
	int i;
	for (i = 0; i < SceneSetNum; i++) LevSelect1(i);

	// select
	char c;
	int j;
	for (;;)
	{
		// get character
		c = KeyGet();

		switch (c)
		{
		case KEY_UP:
			i = SceneInx;
			SceneInx = -1;
			LevSelect1(i);
			i -= ROWLEV;
			if (i < 0)
			{
				do i += ROWLEV; while (i < SceneSetNum);
				i -= ROWLEV;
				if (i < 0) i += ROWLEV;
			}
			SceneInx = i;
			LevSelect1(i);
			break;

		case KEY_LEFT:
			i = SceneInx;
			SceneInx = -1;
			LevSelect1(i);
			i--;
			if (i < 0) i = SceneSetNum-1;
			SceneInx = i;
			LevSelect1(i);
			break;

		case KEY_DOWN:
			i = SceneInx;
			SceneInx = -1;
			LevSelect1(i);
			i += ROWLEV;
			if (i >= SceneSetNum)
			{
				do i -= ROWLEV; while (i >= 0);
				i += ROWLEV;
				if (i >= SceneSetNum) i -= ROWLEV;
			}
			SceneInx = i;
			LevSelect1(i);
			break;

		case KEY_RIGHT:
			i = SceneInx;
			SceneInx = -1;
			LevSelect1(i);
			i++;
			if (i > SceneSetNum-1) i = 0;
			SceneInx = i;
			LevSelect1(i);
			break;

		case KEY_A: // select
			return True;

		case KEY_X:
		case KEY_Y:
		case KEY_B: // Esc
			return False;
		}
	}
	return False;
}
