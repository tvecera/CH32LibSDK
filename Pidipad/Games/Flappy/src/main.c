
// ****************************************************************************
//
//                                  Flappy game
//
// ****************************************************************************

#include "../include.h"

#define F_	NOTE_F4
#define G_	NOTE_G4
#define A_	NOTE_A4
#define H_	NOTE_B4
#define C	NOTE_C5
#define D	NOTE_D5
#define E	NOTE_E5
#define F	NOTE_F5
#define FIS	NOTE_FS5
#define G	NOTE_G5
#define A	NOTE_A5
#define H	NOTE_B5
#define _C	NOTE_C6
#define _D	NOTE_D6
#define _E	NOTE_E6
#define _G	NOTE_G6

#define R	NOTE_R

#define L2	NOTE_LEN2
#define L2D	NOTE_LEN2DOT
#define L4	NOTE_LEN4
#define L4D	NOTE_LEN4DOT
#define L8	NOTE_LEN8

#define NOTE(tone, len) { (len), (tone) }

// main melody
const sMelodyNote MusicSound[] = {
	NOTE(R, L2), NOTE(E, L8), NOTE(R, L8),
	NOTE(C, L4), NOTE(R, L4), NOTE(E, L8), NOTE(R, L8),
	NOTE(C, L4), NOTE(R, L4), NOTE(E, L8), NOTE(R, L8), 
	NOTE(G, L8), NOTE(R, L8), NOTE(G, L8), NOTE(E, L8), NOTE(C, L8), NOTE(E, L8),
	NOTE(D, L4D), NOTE(R, L8), NOTE(F, L8), NOTE(R, L8),
	NOTE(H_, L4), NOTE(R, L4), NOTE(D, L8), NOTE(R, L8),
	NOTE(G_, L4), NOTE(R, L4), NOTE(D, L8), NOTE(R, L8),
	NOTE(F, L8), NOTE(R, L8), NOTE(F, L8), NOTE(D, L8), NOTE(H_, L8), NOTE(D, L8),
	NOTE(C, L4D), NOTE(R, L4D),
	NOTE(A, L2D),

	NOTE(R, L4), NOTE(F, L8), NOTE(R, L8), NOTE(F, L8), NOTE(R, L8),


	NOTE(R, L8), NOTE(A, L8), NOTE(F, L8), NOTE(A, L8), NOTE(F, L8), NOTE(A, L8),
	NOTE(_D, L8), NOTE(_C, L8), NOTE(H, L8), NOTE(_D, L8), NOTE(_C, L8), NOTE(A, L8),
	NOTE(G, L4), NOTE(E, L4), NOTE(C, L4),
 	NOTE(R, L4), NOTE(E, L4), NOTE(C, L4),
	NOTE(R, L4), NOTE(E, L4), NOTE(C, L4),
	NOTE(G, L8), NOTE(FIS, L8), NOTE(A, L8), NOTE(G, L8), NOTE(F, L8), NOTE(E, L8),
	NOTE(D, L4), NOTE(H_, L8), NOTE(G, L8), NOTE(F, L4),
	NOTE(D, L4), NOTE(H_, L8), NOTE(G, L8), NOTE(F, L4),
	NOTE(D, L4), NOTE(H_, L8), NOTE(G, L8), NOTE(F, L4),
	NOTE(H, L8), NOTE(A, L8), NOTE(G, L8), NOTE(F, L8), NOTE(E, L8), NOTE(D, L8),
	NOTE(C, L4), NOTE(R, L4), NOTE(H_, L4),
	NOTE(C, L4), NOTE(R, L4), NOTE(H_, L4),
	NOTE(C, L4), NOTE(R, L2),
	{ 0, 0 },
};

int main(void)
{
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

	// select scene (returns True = OK, False = Esc)
	while (LevSelect())
	{
		NewGame(True);

		// game loop
		while (GameLoop())
		{
			// continue to next scene
			SceneInx++;
			if (SceneInx >= SceneSetNum) SceneInx = 0;
			NewGame(False);
		}
	}
	ResetToBootLoader();
}
