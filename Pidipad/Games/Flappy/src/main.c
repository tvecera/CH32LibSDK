
// ****************************************************************************
//
//                                  Flappy game
//
// ****************************************************************************

#include "../include.h"

#define F_	NOTE_F2
#define G_	NOTE_G2
#define A_	NOTE_A2
#define H_	NOTE_B2
#define C	NOTE_C3
#define D	NOTE_D3
#define E	NOTE_E3
#define F	NOTE_F3
#define FIS	NOTE_FS3
#define G	NOTE_G3
#define A	NOTE_A3
#define H	NOTE_B3
#define _C	NOTE_C4
#define _D	NOTE_D4
#define _E	NOTE_E4
#define _G	NOTE_G4

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
