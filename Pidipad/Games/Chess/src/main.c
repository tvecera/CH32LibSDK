
// ****************************************************************************
//
//                              Chess game
//
// ****************************************************************************
// In case of insufficient RAM, decrease the size of STACK_MAX.

#include "../include.h"

// text console
#define TEXTX	104	// text X coordinate
#define TEXTW	9	// text width (9 characters of width 6 pixels, 54 pixels)
#define TEXTH	15	// text height (15 rows, 120 pixels)
#define TEXTS (TEXTW*TEXTH) // size of text buffer

// field tile
#define TILEW	12	// tile width
#define TILEH	12	// tile height

// board map in fields
#define MAP0W	8	// board visible width in fields, without border
#define MAP0H	8	// board visible height in fields, without border
#define MAPW	(MAP0W+2) // board internal width in fields, with border 1+1 columns (= 10)
#define MAPH	(MAP0H+4) // board internal height in fields, with border 2+2 rows (= 12)
#define MAPSIZE (MAPW*MAPH) // board internal size in fields (= 120)

// visible board
#define FRAME	8	// frame width
#define BOARDX	FRAME	// board X coordinate
#define BOARDY	8	// board Y coordinate
#define BOARDW (FRAME + MAP0W*TILEW) // board total width (= 104)
#define BOARDH (FRAME + MAP0H*TILEH) // board total height (= 104)

#define FRAMECOL	COL_RED	// border color

// piece indices
#define EMPTY	0	// empty field (or black pawn)
#define PAWN	1	// pawn (or white pawn)
#define KNIGHT	2	// knight
#define BISHOP	3	// bishop
#define ROOK	4	// rook
#define QUEEN	5	// queen
#define KING	6	// king
#define BORDER	7	// invalid field
#define PIECEMASK 7	// mask of piece value

#define MOVEMASK B4	// flag of moving piece
#define MOVING	0	// piece has been moving
#define NOMOVING MOVEMASK // piece has not been moving yet

#define COLORMASK B5	// mask of piece color
#define WHITE	0	 // white color of piece
#define BLACK	COLORMASK // black color of piece

// player
typedef struct {
	Bool	comp;		// player is computer
	u8	curpos;		// current cursor position
	u8	king;		// king position
} sPlayer;

#define WHITE_PLAYER	0	// index of white player
#define BLACK_PLAYER	1	// index of black player

// move stack entry (8 bytes)
typedef struct {
	u8	src;		// source position
	u8	dst;		// destination position
	u8	srcpiece;	// source piece (including original NOMOVING flag)
	u8	dstpiece;	// old content of destination field (with color)
	u8	flags;		// flags MOVEFLAG_*
	u8	extra;		// additional data: new piece to change from pawn, En Passant position
	s16	val;		// move value
} sMove;

#define VAL_WIN		10000	// winning value

// move flags
#define MOVEFLAG_NORMAL		0	// normal move
#define MOVEFLAG_QUEEN		1	// change pawn to queen (= piecenew)
#define MOVEFLAG_SMALL		2	// make small castle
#define MOVEFLAG_BIG		3	// make big castle
#define MOVEFLAG_EP		4	// en passant privilege

#define STACK_MAX	420	// max. size of move stack
#define DEEP_MAX	5	// max. deep in move stack

// text console
char TextBuf[TEXTS+1];
int TextRows = 0;

// view to board (WHITE_PLAYER or BLACK_PLAYER)
u8 PlayerView;

// game board with border (order of fields: from field A1 to field H8)
u8 Board[MAPSIZE];

// available moves
Bool BoardEnable[MAPSIZE];

// move stack
sMove MoveStack[STACK_MAX];
int MoveStackTop; // number of entries
const sMove* LastMove; // last move (NULL = check-mat)

// start entries and number of entries in move stack
int MoveStackStart[DEEP_MAX]; // start indices
int MoveStackNum[DEEP_MAX]; // number of moves in one deep level
int MoveStackInx[DEEP_MAX]; // current index in move stack
u8 MovePassantPiece[DEEP_MAX+1]; // position of piece to En Passant (0 = invalid)
u8 MovePassantPos[DEEP_MAX+1]; // capture position of piece to En Passant (0 = invalid)
int MoveDeep; // current deep in move stack
int DeepMax;

// player
u8 Player;	// current player (WHITE_PLAYER or BLACK_PLAYER)
//u8 Enemy;	// color of other player (WHITE_PLAYER or BLACK_PLAYER)
sPlayer Players[2]; // players

int MoveNum;	// move counter (current move 1..)
u32 BlinkTime; // cursor blinking time

// board template with border (from A1 to H8)
const u8 BoardTemp[MAPSIZE] = {
//	0		1		2		3		4		5		6		7		8		9
/*00*/	BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		// 00
/*10*/	BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		// 10
/*20*/	BORDER,		ROOK+WHITE,	KNIGHT+WHITE,	BISHOP+WHITE,	QUEEN+WHITE,	KING+WHITE,	BISHOP+WHITE,	KNIGHT+WHITE,	ROOK+WHITE,	BORDER,		// 20
/*30*/	BORDER,		PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	PAWN+WHITE,	BORDER,		// 30
/*40*/	BORDER,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		BORDER,		// 40
/*50*/	BORDER,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		BORDER,		// 50
/*60*/	BORDER,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		BORDER,		// 60
/*70*/	BORDER,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		EMPTY,		BORDER,		// 70
/*80*/	BORDER,		PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	PAWN+BLACK,	BORDER,		// 80
/*90*/	BORDER,		ROOK+BLACK,	KNIGHT+BLACK,	BISHOP+BLACK,	QUEEN+BLACK,	KING+BLACK,	BISHOP+BLACK,	KNIGHT+BLACK,	ROOK+BLACK,	BORDER,		// 90
/*100*/	BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		// 100
/*110*/	BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		BORDER,		// 110
};

// template of possible moves (one row is 10 fields width)
const s8 MovesTemp[24] = {
	9,	11,	-11,	-9,	// 0: diagonally
	10,	-10,	1,	-1,	// 4: perpendicularly
	-21,	-12,	8,	19,	// 8: knight to the left
	21,	12,	-8,	-19,	// 12: knight to the right
	10,	20,	11,	9,	// 16: white pawn
	-10,	-20,	-11,	-9,	// 20: black pawn
};

// possible moves of the pieces (offset, number of moves)
const u8 PieceMoves[14] = {
	20, 4,		// 0: black pawn
	16, 4,		// 1: white pawn
	8, 8,		// 2: knight
	0, 4,		// 3: bishop
	4, 4,		// 4: rook
	0, 8,		// 5: queen
	0, 8,		// 6: king
};

// rating of the pieces
const u8 PieceRate[7] = {
	0,		// 0: empty
	1,		// 1: pawn
	3,		// 2: knight
	3,		// 3: bishop
	5,		// 4: rook
	9,		// 5: queen
	15,		// 6: king
};

// white player's opening moves table
const sMove OpenMovesTab[4] = {
	{ 35, 55, PAWN+WHITE+NOMOVING, EMPTY, MOVEFLAG_NORMAL, 0, 0 },		// pawn from B5 to D5
	{ 34, 54, PAWN+WHITE+NOMOVING, EMPTY, MOVEFLAG_NORMAL, 0, 0 },		// pawn from B4 to D4
	{ 22, 43, KNIGHT+WHITE+NOMOVING, EMPTY, MOVEFLAG_NORMAL, 0, 0 },	// knight from A2 to C3
	{ 27, 46, KNIGHT+WHITE+NOMOVING, EMPTY, MOVEFLAG_NORMAL, 0, 0 },	// knight from A7 to C6
};

// print text row to text console
void OutText(const char* txt)
{
	// scroll buffer
	if (TextRows >= TEXTH)
	{
		memmove(&TextBuf[0], &TextBuf[TEXTW], TEXTS - TEXTW);
		memset(&TextBuf[TEXTS - TEXTW], ' ', TEXTW);
		TextRows--;
	}

	// print text
	int len = StrLen(txt);
	if (len > TEXTW) len = TEXTW;
	memcpy(&TextBuf[TextRows*TEXTW], txt, len);

	// print text buffer
	int x, y;
	char ch;
	for (y = 0; y < TEXTH; y++)
	{
		ch = TextBuf[(y+1)*TEXTW];
		TextBuf[(y+1)*TEXTW] = 0;
		DrawTextCond(&TextBuf[y*TEXTW], TEXTX, y*8, COL_WHITE);
		TextBuf[(y+1)*TEXTW] = ch;
	}
	TextRows++;
}

// get piece from board (returns BORDER on invalid coordinates)
INLINE u8 GetPiece(u8 inx) { return Board[inx]; }

// set piece to board
INLINE void SetPiece(u8 inx, u8 piece) { Board[inx] = piece; }

// display one field (respects board rotation)
//   usecolor ..... use custom color
//   color ... custom color
//   usemove ... use "move" color
//   usecatch ... use "catch" color
void DispField(u8 inx, Bool usecolor, u16 color, Bool usemove, Bool usecatch)
{
	// check if field is valid
	u8 piece = GetPiece(inx);
	if (piece == BORDER) return;

	// split index to column and row
	u8 row = inx/MAPW;
	u8 col = inx - row*MAPW - 1; // 1 column is border
	row -= 2; // 2 rows are border

	// field destination coordinate
	u8 x, y;
	if (PlayerView)
	{
		x = BOARDX + (MAP0W - 1 - col)*TILEW;
		y = BOARDY + row*TILEH;
	}
	else
	{
		x = BOARDX + col*TILEW;
		y = BOARDY + (MAP0H - 1 - row)*TILEH;
	}

	// field source offset
	int xs = (piece & PIECEMASK) * 2; // piece type, without color
	int ys = ((piece & COLORMASK) == WHITE) ? 0 : PIECESIMGWB*TILEH; // piece color
	xs += ys;

	// prepare default color
	if (!usecolor)
	{
		color = COL_WHITE;
		if (BoardEnable[inx]) color = COL_GREEN;
		if (usemove) color = COL_CYAN;
		if (usecatch) color = COL_MAGENTA;
	}

	// use grid instead of empty black field
	if ((color != COL_WHITE) && ((piece & PIECEMASK) == 0)) xs += 14;

	// display field
	if (((row + col) & 1) != 0) // white field
		DrawImgBg(ImgPiecesWhite + xs, x, y, TILEW, TILEW, PIECESIMGWB, color);
	else
		DrawImgBg(ImgPiecesBlack + xs, x, y, TILEW, TILEW, PIECESIMGWB, color);
}

// display board frame
void DispBoardFrame()
{
	int i, j;
	u16 col;
	char ch;

	// draw frame
	DrawRect(0, HEIGHT-FRAME-8, BOARDW, FRAME, FRAMECOL); // bottom border
	DrawRect(0, 8, FRAME, HEIGHT-2*FRAME, FRAMECOL); // left border

	// horizontal legend
	for (i = 0; i < MAP0W; i++)
	{
		ch = PlayerView ? ('H' - i) : ('A' + i);
		DrawCharClr(ch, FRAME + TILEW/2 - 4 + i*TILEW, HEIGHT-FRAME-8, FRAMECOL); // bottom legend
	}

	// vertical legend
	for (i = 0; i < MAP0H; i++)
	{
		ch = PlayerView ? ('1' + i) : ('8' - i);
		DrawCharClr(ch, 0, 8 + TILEH/2 - 4 + i*TILEH, FRAMECOL); // left legend
	}
}

// display base board
void DispBoard()
{
	int i;
	for (i = 0; i < MAPSIZE; i++) DispField(i, False, 0, False, False);
}

// start new game
void NewGame()
{
	DrawClear();

	PlayerView = False; // view direction
	MoveNum = 1; // move counter
	Player = WHITE_PLAYER; // white is playing first
	MoveStackTop = 0; // clear move stack
//#ifdef DEBUG_STACK	// debug flag - display stack max. depth
//	MoveStackTopMax = 0; // number of entries
//#endif
	MovePassantPiece[0] = 0; // last position of piece to En Passant (0 = invalid)
	MovePassantPos[0] = 0; // last capture position of piece to En Passant (0 = invalid)

	// clear text buffer
	memset(TextBuf, ' ', TEXTS);
	TextRows = 0;

	// initialize board
	u8 b;
	int i;
	for (i = 0; i < MAPSIZE; i++)
	{
		BoardEnable[i] = False;
		b = BoardTemp[i];
		if (b == KING+WHITE) Players[WHITE_PLAYER].king = i; // white king
		if (b == KING+BLACK) Players[BLACK_PLAYER].king = i; // black king
		if ((b != BORDER) && (b != EMPTY)) b |= NOMOVING; // not moving yet
		Board[i] = b;
	}

	// initialize players
	sPlayer* p = Players;
	for (i = 0; i < 2; i++)
	{
		p->curpos = p->king; // current cursor position
		p++;
	}
}

// do move (without visualization)
void DoMove(const sMove* m)
{
	// update king position
	if ((m->srcpiece & PIECEMASK) == KING) Players[Player].king = m->dst;

	// do normal move
	if (m->flags == MOVEFLAG_NORMAL)
	{
		SetPiece(m->src, EMPTY); // empty old position
		SetPiece(m->dst, m->srcpiece & ~MOVEMASK); // move piece, clear move flag
	}

	// change pawn to queen
	else if (m->flags == MOVEFLAG_QUEEN)
	{
		SetPiece(m->src, EMPTY); // empty old position
		SetPiece(m->dst, m->extra); // place new piece
	}

	// En Passant
	else if (m->flags == MOVEFLAG_EP)
	{
		SetPiece(m->src, EMPTY); // empty old position
		SetPiece(m->dst, m->srcpiece & ~MOVEMASK); // move piece, clear move flag
		SetPiece(m->extra, EMPTY); // capture enemy pawn
	}

	// small castle
	else if (m->flags == MOVEFLAG_SMALL)
	{
		SetPiece(m->src, EMPTY); // empty king
		SetPiece(m->src+1, m->dstpiece & ~MOVEMASK); // put rook
		SetPiece(m->dst, m->srcpiece & ~MOVEMASK); // put king
		SetPiece(m->src+3, EMPTY); // empty rook
	}

	// big castle
	else if (m->flags == MOVEFLAG_BIG)
	{
		SetPiece(m->src, EMPTY); // empty king
		SetPiece(m->src-1, m->dstpiece & ~MOVEMASK); // put rook
		SetPiece(m->dst, m->srcpiece & ~MOVEMASK); // put king
		SetPiece(m->src-4, EMPTY); // empty rook
	}
}

// undo move (without visualization)
void UndoMove(const sMove* m)
{
	if ((m->srcpiece & PIECEMASK) == KING) Players[Player].king = m->src;

	// En Passant
	if (m->flags == MOVEFLAG_EP)
	{
		SetPiece(m->src, m->srcpiece);
		SetPiece(m->dst, EMPTY);
		SetPiece(m->extra, m->dstpiece);
	}

	// small castle
	else if (m->flags == MOVEFLAG_SMALL)
	{
		SetPiece(m->src, m->srcpiece | NOMOVING);
		SetPiece(m->src+1, EMPTY);
		SetPiece(m->dst, EMPTY);
		SetPiece(m->src+3, m->dstpiece | NOMOVING);
	}

	// big castle
	else if (m->flags == MOVEFLAG_BIG)
	{
		SetPiece(m->src, m->srcpiece | NOMOVING);
		SetPiece(m->src-1, EMPTY);
		SetPiece(m->dst, EMPTY);
		SetPiece(m->src-4, m->dstpiece | NOMOVING);
	}
	else
	{
		SetPiece(m->src, m->srcpiece);
		SetPiece(m->dst, m->dstpiece);
	}
}

// do move with visualization
void DoMoveDisp(const sMove* m)
{
	LastMove = m;

	// save position of En Passant
	MovePassantPiece[0] = 0;
	MovePassantPos[0] = 0;
	if ((m->srcpiece & (PIECEMASK | MOVEMASK)) == (PAWN | NOMOVING))
	{
		int dif = m->dst - m->src;
		if ((dif == 20) || (dif == -20))
		{
			MovePassantPiece[0] = m->dst; // destination position of pawn
			MovePassantPos[0] = m->src + dif/2; // capture position
		}
	}

	// prepare destination color
	Bool usemove = True;
	Bool usecatch = False;
	if ((m->dstpiece != EMPTY) && (m->flags != MOVEFLAG_SMALL) && (m->flags != MOVEFLAG_BIG))
	{
		usemove = False;
		usecatch = True;
	}

	// draw destination
	DispField(m->dst, False, 0, usemove, usecatch);

	// animate source field
	int i;
	for (i = 0; i < 4; i++)
	{
		SetPiece(m->src, m->srcpiece);
		DispField(m->src, False, 0, True, False);
		WaitMs(100);

		SetPiece(m->src, EMPTY);
		DispField(m->src, False, 0, True, False);
		WaitMs(100);
	}
	SetPiece(m->src, m->srcpiece);

	// do move
	DoMove(m);
	u8 piece = GetPiece(m->dst);

	// display source field
	DispField(m->src, False, 0, True, False);

	// animate destination field
	for (i = 0; i < 4; i++)
	{
		SetPiece(m->dst, piece);
		DispField(m->dst, False, 0, usemove, usecatch);
		WaitMs(100);

		SetPiece(m->dst, EMPTY);
		DispField(m->dst, False, 0, usemove, usecatch);
		WaitMs(100);
	}
	SetPiece(m->dst, piece);

	DispBoard();
}

// test whether king of current player is in check
Bool TestCheck()
{
	int dir, jump, pos, pos0, steps;
	u8 piece, king;

	// prepare king current position
	pos0 = Players[Player].king;
	king = GetPiece(pos0);

	// check all possible directions from direction table (including knight jump)
	for (dir = 0; dir < 16; dir++)
	{
		// load next jump offset (directions: 0..3 diagonally, 4..7 perpendicularly, 8..15 knight)
		jump = MovesTemp[dir];

		// prepare starting position
		pos = pos0;

		// check steps
		for (steps = 0; ;)
		{
			// increase steps
			steps++;

			// shift position
			pos += jump;

			// get board content
			piece = GetPiece(pos);

			// border
			if (piece == BORDER) break;

			// empty field - continue to next field, except knight direction
			if (piece == EMPTY)
			{
				if (dir >= 8) break;
				continue;
			}

			// check color - piece of same color covers the king
			if (((piece ^ king) & COLORMASK) == 0) break;
			piece &= PIECEMASK;

			// enemy knight (1 step, knight directions)
			if (piece == KNIGHT)
			{
				if (dir >= 8) return True; // king is in check
				break; // not in check, no other steps
			}

			// no other pieces can check in knight directions
			if (dir >= 8) break;

			// enemy king (1 step, not knight directions)
			if (piece == KING)
			{
				if (steps == 1) return True; // king is in check
				break; // not in check, no other steps
			}

			// enemy queen
			if (piece == QUEEN) return True;

			// bishop
			if ((piece == BISHOP) && (dir <= 3)) return True;

			// rook
			if ((piece == ROOK) && (dir >= 4)) return True;
			
			// pawn
			if ((piece == PAWN) && (dir <= 3) && (steps == 1))
			{
				// black pawn attacking white king
				if ((king & COLORMASK) == WHITE)
				{
					if ((jump == 11) || (jump == 9)) return True;
				}

				// white pawn attacking black king
				else
				{
					if ((jump == -11) || (jump == -9)) return True;
				}
			}

			// others are safe
			break;
		}
	}
	return False;
}

// search all possible moves in current deep level
void SearchMoves(s16 val)
{
	int level = MoveDeep;	// current deep level
	int movenum = MoveStackTop; // number of entries
	MoveStackStart[level] = movenum; // save index of start move
	sMove* move = &MoveStack[movenum]; // pointer to current move
	u8 piece, piece2, piece3, piece4, flags;
	u8 col = (Player == WHITE_PLAYER) ? WHITE : BLACK; // prepare piece color
	u8 enemycol = col ^ COLORMASK; // prepare enemy piece color
	int dir, dirnum, jump, pos, steps;
	s16 val2;
	Bool possiblecheck, check;
	u8 extra = 0;

	// loop all fields
	u8 pos0 = 21; // first field
	for (; pos0 <= 98; pos0++)
	{
		// get piece
		piece = GetPiece(pos0);

		// check piece color (must be player's own piece)
		if ((piece == BORDER) || (piece == EMPTY) || ((piece & COLORMASK) != col)) continue;

		// fast check to see if removing a piece can put the king in check
		piece2 = piece & PIECEMASK;
		if (piece2 == KING)
			possiblecheck = True;
		else
		{
			SetPiece(pos0, EMPTY);
			possiblecheck = TestCheck();
			SetPiece(pos0, piece);
		}

		// prepare piece directions
		if ((piece2 == PAWN) && (col == BLACK)) piece2 = 0; // black and white pawn differentiation (black pawn will have index 0)
		dir = PieceMoves[piece2*2];
		dirnum = PieceMoves[piece2*2+1];

		// loop all directions
		for (; dirnum > 0; dirnum--)
		{
			// get jump offset
			jump = MovesTemp[dir];
			dir++;

			// prepare start position
			pos = pos0;

			// check steps
			for (steps = 0; ;)
			{
				// increase steps
				steps++;

				// shift position
				pos += jump;

				// get board content
				piece3 = GetPiece(pos);

				// border
				if (piece3 == BORDER) break;

				// same color, break
				if ((piece3 != EMPTY) && ((piece3 & COLORMASK) == col)) break;

				// pawn
				flags = MOVEFLAG_NORMAL;
				if (piece2 <= 1)
				{
					// 2 fields: must be first move and to empty field
					if ((jump == 20) || (jump == -20))
					{
						if (((piece & MOVEMASK) == MOVING) || (piece3 != EMPTY)) break;
					}

					// 1 step in straight direction: must be to empty field
					if ((jump == 10) || (jump == -10))
					{
						if (piece3 != EMPTY) break;
					}

					// 1 step diagonally: must capture enemy
					if ((jump == -11) || (jump == -9) || (jump == 11) || (jump == 9))
					{
						if (piece3 == EMPTY)
						{
							// check En Passant Privilege
							if (pos != MovePassantPos[level]) break;
							extra = MovePassantPiece[level];
							piece3 = GetPiece(extra);
							flags = MOVEFLAG_EP;
						}
					}

					// change pawn to something else
					if (((pos >= 21) && (pos <= 28)) || ((pos >= 91) && (pos <= 98)))
					{
						// test check condition
						check = False;
						if (possiblecheck)
						{
							SetPiece(pos0, EMPTY);
							SetPiece(pos, piece);
							check = TestCheck();
							SetPiece(pos0, piece);
							SetPiece(pos, piece3);	
						}

						if (!check)
						{
							// add 4 moves
							for (piece4 = KNIGHT; piece4 <= QUEEN; piece4++)
							{
								val2 = val;
								if (piece3 != EMPTY) val2 += PieceRate[piece3 & PIECEMASK];
								val2 += PieceRate[piece4] - 1;

								if (movenum < STACK_MAX)
								{
									move->src = pos0;
									move->dst = pos;
									move->srcpiece = piece;
									move->dstpiece = piece3;
									move->flags = MOVEFLAG_QUEEN;
									move->val = val2;
									move->extra = piece4 | col;
									move++;
									movenum++;
								}
							}
						}
						break;
					}
				}

				// test check condition
				check = False;
				if (piece2 == KING)
				{
					// check king
					SetPiece(pos0, EMPTY);
					SetPiece(pos, piece);
					Players[Player].king = pos;
					check = TestCheck();
					Players[Player].king = pos0;
					SetPiece(pos0, piece);
					SetPiece(pos, piece3);	
				}
				else if (possiblecheck)
				{
					SetPiece(pos0, EMPTY);
					SetPiece(pos, piece);
					check = TestCheck();
					SetPiece(pos0, piece);
					SetPiece(pos, piece3);	
				}

				if (!check)
				{
					// empty field or enemy
					val2 = val;
					if (piece3 != EMPTY) val2 += PieceRate[piece3 & PIECEMASK];

					// add move
					if (movenum < STACK_MAX)
					{
						move->src = pos0;
						move->dst = pos;
						move->srcpiece = piece;
						move->dstpiece = piece3;
						move->flags = flags;
						move->extra = extra;
						move->val = val2;
						move++;
						movenum++;
					}
				}

				// found enemy, stop steps
				if (piece3 != EMPTY) break;

				// king, pawn and knight can go only 1 step
				if ((piece2 == KNIGHT) || (piece2 <= 1) || (piece2 == KING)) break;
			}
		}
	}

	// test castle
	pos0 = Players[Player].king; // king position
	piece = GetPiece(pos0); // get king piece
	if (((piece & MOVEMASK) == NOMOVING) && !TestCheck())
	{
		// small castle
		pos = pos0 + 3;
		piece3 = GetPiece(pos);
		if ( ((piece3 & (PIECEMASK|MOVEMASK)) == (ROOK|NOMOVING)) &&
			(GetPiece(pos0 + 1) == EMPTY) &&
			(GetPiece(pos0 + 2) == EMPTY))
		{
			Players[Player].king = pos0+1;
			check = TestCheck();
			Players[Player].king = pos0+2;
			check |= TestCheck();
			Players[Player].king = pos0;

			if (!check && (movenum < STACK_MAX))
			{
				move->src = pos0;
				move->dst = pos0+2;
				move->srcpiece = piece;
				move->dstpiece = piece3;
				move->flags = MOVEFLAG_SMALL;
				move->val = val;
				move++;
				movenum++;
			}
		}

		// big castle
		pos = pos0 - 4;
		piece3 = GetPiece(pos);
		if ( ((piece3 & (PIECEMASK|MOVEMASK)) == (ROOK|NOMOVING)) &&
			(GetPiece(pos0 - 1) == EMPTY) &&
			(GetPiece(pos0 - 2) == EMPTY) &&
			(GetPiece(pos0 - 3) == EMPTY))
		{
			Players[Player].king = pos0-1;
			check = TestCheck();
			Players[Player].king = pos0-2;
			check |= TestCheck();
			Players[Player].king = pos0;

			if (!check && (movenum < STACK_MAX))
			{
				move->src = pos0;
				move->dst = pos0-2;
				move->srcpiece = piece;
				move->dstpiece = piece3;
				move->flags = MOVEFLAG_BIG;
				move->val = val;
				move++;
				movenum++;
			}
		}
	}

	// store number of moves
	MoveStackNum[level] = movenum - MoveStackTop;
	MoveStackTop = movenum;

//#ifdef DEBUG_STACK	// debug flag - display stack max. depth
//	if (MoveStackTopMax < movenum) MoveStackTopMax = movenum; // number of entries
//#endif

}

// find computer move on one level (returns best move, NULL = not found)
sMove* MoveCompLevel()
{
	int deep = MoveDeep;
	int top;
	sMove *m, *m2;

	// search moves at this level
	SearchMoves(deep);

	// number of moves
	int i = MoveStackNum[deep];
	if (i == 0) return NULL;

	// max. level (initial moves are with less depth)
	int deepmax = MoveNum;
	if (deepmax > DeepMax-1) deepmax = DeepMax-1;

	// loop moves
	if (deep < deepmax)
	{
		// shift level
		MoveDeep = deep + 1;

		// check moves
		m = &MoveStack[MoveStackStart[deep]];
		for (; i > 0; i--)
		{
			// save En Passant position
			MovePassantPiece[deep+1] = 0;
			MovePassantPos[deep+1] = 0;
			if ((m->srcpiece & (PIECEMASK | MOVEMASK)) == (PAWN | NOMOVING))
			{
				int dif = m->dst - m->src;
				if ((dif == 20) || (dif == -20))
				{
					MovePassantPiece[deep+1] = m->dst; // destination position of pawn
					MovePassantPos[deep+1] = m->src + dif/2; // capture position
				}
			}

			// do this move
			DoMove(m);

			// change player
			Player ^= 1;
			top = MoveStackTop;

			// search sub-moves
			m2 = MoveCompLevel();

			// no move, checkmat or pat
			if (m2 == NULL)
				m->val += VAL_WIN;
			else
				// add move value
				m->val -= m2->val;

			// restore state
			MoveStackTop = top;

			// change player
			Player ^= 1;

			// undo this move
			UndoMove(m);

			// next move
			m++;
		}

		// restore state
		MoveDeep = deep;
	}

	// use first move
	m = &MoveStack[MoveStackStart[deep]];
	int bestnum = 1;
	s16 bestval = m->val;
	m++;
	i = MoveStackNum[deep] - 1;
	
	// check moves
	for (; i > 0; i--)
	{
		// check value
		if (m->val == bestval)
			bestnum++;
		else if (m->val > bestval)
		{
			bestnum = 1;
			bestval = m->val;
		}

		// next move
		m++;
	}

	// randomize selection
	i = RandS16Max(bestnum - 1);

	// search best move
	m = &MoveStack[MoveStackStart[deep]];
	for (;;)
	{
		if (m->val == bestval)
		{
			i--;
			if (i < 0) break;
		}		
		m++;
	}
		
	return m;
}

// move computer
void MoveComp()
{
	// white player is opening game
	if ((MoveNum == 1) && (Player == WHITE_PLAYER))
	{
		const sMove* m  = &OpenMovesTab[RandU8Max(3)];
		DoMoveDisp(m);
		return;
	}

	// prepare all possible moves
	MoveDeep = 0;
	MoveStackTop = 0;

//#ifdef DEBUG_STACK	// debug flag - display stack max. depth
//	MoveStackTopMax = 0; // number of entries
//#endif

	// search move
	sMove* m = MoveCompLevel();
	LastMove = m;

	if (m != NULL)
	{
		DoMoveDisp(m);
	}
}

// move human player (returns True = quit game)
Bool MoveHuman()
{
	u32 t;
	char ch;
	u16 col;
	u8 piece, piece2;
	Bool ok, sel, cancel;
	u8 oldpos;
	sMove* m;
	int i, j;

	// current player
	sPlayer* p = &Players[Player];

	// select view
	PlayerView = Player;

	// display board frame
	DispBoardFrame();

	// prepare all possible moves
	MoveDeep = 0;
	MoveStackTop = 0;
//#ifdef DEBUG_STACK	// debug flag - display stack max. depth
//	MoveStackTopMax = 0; // number of entries
//#endif
	SearchMoves(0);

	// check-mat
	if (MoveStackNum[0] == 0)
	{
		LastMove = NULL;
		return False;
	}

MOVE_AGAIN:

	// check start position
	for (i = 0; i < MAPSIZE; i++) BoardEnable[i] = False;
	m = &MoveStack[MoveStackStart[0]];
	for (i = MoveStackNum[0]; i > 0; i--)
	{
		BoardEnable[m->src] = True;
		m++;
	}

	// display board
	DispBoard();

	// start blinking cursor
	BlinkTime = Time();

	KeyFlush();

	// select piece
	while (True)
	{
		// prepare cursor color
		piece = GetPiece(p->curpos);
		col = COL_RED; // red cursor
		ok = False;
		if (BoardEnable[p->curpos])
		{
			col = COL_BLUE;
			ok = True; // can move
		}

		// wait
		WaitMs(15);

		// display cursor
		t = Time();
		DispField(p->curpos, (((t - BlinkTime) >> 22) & 3) < 3, col, False, False);

		// get keyboard
		ch = KeyGet();

		// turn board
		if (PlayerView)
		{
			if ((ch == KEY_LEFT) || (ch == KEY_RIGHT)) ch ^= KEY_LEFT ^ KEY_RIGHT;
			if ((ch == KEY_UP) || (ch == KEY_DOWN)) ch ^= KEY_UP ^ KEY_DOWN;
		}

		// cursor movements
		sel = False;
		switch (ch)
		{
		case KEY_A:
			if (ok) sel = True;
			break;

		case KEY_LEFT:
			DispField(p->curpos, False, 0, False, False);
			p->curpos--;
			if (GetPiece(p->curpos) == BORDER) p->curpos += MAP0W;
			BlinkTime = Time();
			break;

		case KEY_RIGHT:
			DispField(p->curpos, False, 0, False, False);
			p->curpos++;
			if (GetPiece(p->curpos) == BORDER) p->curpos -= MAP0W;
			BlinkTime = Time();
			break;

		case KEY_DOWN:
			DispField(p->curpos, False, 0, False, False);
			p->curpos -= MAPW;
			if (GetPiece(p->curpos) == BORDER) p->curpos += MAP0H*MAPW;
			BlinkTime = Time();
			break;

		case KEY_UP:
			DispField(p->curpos, False, 0, False, False);
			p->curpos += MAPW;
			if (GetPiece(p->curpos) == BORDER) p->curpos -= MAP0H*MAPW;
			BlinkTime = Time();
			break;

		case KEY_X:
			for (i = 0; i < MAPSIZE; i++) BoardEnable[i] = False;
			DispBoard();
#if USE_SCREENSHOT		// use screen shots
			ScreenShot();
#endif
			MoveComp();
			return False;

		case KEY_Y:
			return True; // quit game
		}
		if (sel) break;
	}

	// save start position
	oldpos = p->curpos;

	// prepare enable map
	for (i = 0; i < MAPSIZE; i++) BoardEnable[i] = False;
	BoardEnable[oldpos] = True; // return to old position
	m = &MoveStack[MoveStackStart[0]];
	for (i = MoveStackNum[0]; i > 0; i--)
	{
		if (m->src == oldpos) BoardEnable[m->dst] = True;
		m++;
	}

	// display board with enable flags
	DispBoard();

	// pick up piece from chessboard
	SetPiece(oldpos, EMPTY);
	piece2 = EMPTY;

	// start blinking cursor
	BlinkTime = Time();

	KeyFlush();

	// move piece
	while (True)
	{
		// prepare cursor color
		col = COL_RED; // red cursor
		ok = False;
		if (BoardEnable[p->curpos])
		{
			col = COL_BLUE;
			ok = True; // can move
		}

		// wait
		WaitMs(15);

		// display cursor
		t = Time();
		if ((((t - BlinkTime) >> 23) & 1) == 1) // display original field
		{
			SetPiece(p->curpos, piece2);
			DispField(p->curpos, False, 0, False, False);
		}
		else // display moved piece
		{
			SetPiece(p->curpos, piece);
			DispField(p->curpos, True, col, False, False);
		}

		// get keyboard
		ch = KeyGet();

		// turn board
		if (PlayerView == BLACK_PLAYER)
		{
			if ((ch == KEY_LEFT) || (ch == KEY_RIGHT)) ch ^= KEY_LEFT ^ KEY_RIGHT;
			if ((ch == KEY_UP) || (ch == KEY_DOWN)) ch ^= KEY_UP ^ KEY_DOWN;
		}

		// cursor movements
		sel = False;
		cancel = False;
		switch (ch)
		{
		case KEY_A:
			if (ok) sel = True;
			break;

		case KEY_B:
			cancel = True;
			break;

		case KEY_LEFT:
			SetPiece(p->curpos, piece2);
			DispField(p->curpos, False, 0, False, False);
			p->curpos--;
			if (GetPiece(p->curpos) == BORDER) p->curpos += MAP0W;
			BlinkTime = Time();
			piece2 = GetPiece(p->curpos);
			break;

		case KEY_RIGHT:
			SetPiece(p->curpos, piece2);
			DispField(p->curpos, False, 0, False, False);
			p->curpos++;
			if (GetPiece(p->curpos) == BORDER) p->curpos -= MAP0W;
			BlinkTime = Time();
			piece2 = GetPiece(p->curpos);
			break;

		case KEY_DOWN:
			SetPiece(p->curpos, piece2);
			DispField(p->curpos, False, 0, False, False);
			p->curpos -= MAPW;
			if (GetPiece(p->curpos) == BORDER) p->curpos += MAP0H*MAPW;
			BlinkTime = Time();
			piece2 = GetPiece(p->curpos);
			break;

		case KEY_UP:
			SetPiece(p->curpos, piece2);
			DispField(p->curpos, False, 0, False, False);
			p->curpos += MAPW;
			if (GetPiece(p->curpos) == BORDER) p->curpos -= MAP0H*MAPW;
			BlinkTime = Time();
			piece2 = GetPiece(p->curpos);
			break;

		case KEY_X:
			for (i = 0; i < MAPSIZE; i++) BoardEnable[i] = False;
			DispBoard();
			MoveComp();
			return False;

		case KEY_Y:
			return True; // quit game
		}
		if (sel || cancel) break;
	}

	// return piece
	SetPiece(p->curpos, piece2);
	SetPiece(oldpos, piece);

	// clear enable map
	for (i = 0; i < MAPSIZE; i++) BoardEnable[i] = False;
	DispBoard();

	// cancel
	if (cancel)
	{
		p->curpos = oldpos;
		goto MOVE_AGAIN;
	}

	// return piece on original position
	if (p->curpos == oldpos) goto MOVE_AGAIN;

	// search move
	m = &MoveStack[MoveStackStart[0]];
	for (i = MoveStackNum[0]; i > 0; i--)
	{
		if ((m->src == oldpos) && (m->dst == p->curpos) &&
			((m->flags != MOVEFLAG_QUEEN) || ((m->extra & PIECEMASK) == QUEEN)))
		{
			DoMoveDisp(m);
			break;
		}
		m++;
	}

	return False;
}

// open screen
void Open()
{
	char ch;

	DrawClear();
	DrawText2("CHESS", (WIDTH - 5*16)/2, 8, COL_YELLOW);

	DrawText("Select:", 0, 4*8, COL_GREEN);

	DrawText("LEFT ... Play White", 0, 6*8, COL_WHITE);
	DrawText("RIGHT .. Play Black", 0, 7*8, COL_WHITE);
	DrawText("DOWN ... 2 Players", 0, 8*8, COL_WHITE);
	DrawText("UP ..... Demo", 0, 9*8, COL_WHITE);
	DrawText("Y ...... Quit", 0, 10*8, COL_WHITE);
//	DrawText("A ... Level:", 0, 12*8, COL_WHITE);

	KeyFlush();
	for (;;)
	{
		ch = KeyGet();
		switch (ch)
		{
		case KEY_LEFT:
			Players[0].comp = False;
			Players[1].comp = True;
			return;

		case KEY_RIGHT:
			Players[0].comp = True;
			Players[1].comp = False;
			return;
	
		case KEY_DOWN:
			Players[0].comp = False;
			Players[1].comp = False;
			return;

		case KEY_UP:
			Players[0].comp = True;
			Players[1].comp = True;
			return;

//		case KEY_A:
//			DeepMax++;
//			if (DeepMax > DEEP_MAX) DeepMax = 2;
//			break;

		case KEY_Y: ResetToBootLoader();
		}
/*
		if (DeepMax == 2)
			DrawText("BEGINNER", 12*8, 12*8, COL_RED);
		else if (DeepMax == 3)
			DrawText("ADVANCED", 12*8, 12*8, COL_RED);
		else if (DeepMax == 4)
			DrawText("PRO     ", 12*8, 12*8, COL_RED);
		else
			DrawText("SUPERPRO", 12*8, 12*8, COL_RED);
*/
	}
}

int main(void)
{
	char buf[20];
	int bufn;
	char ch;

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

	int i, row, col;
	DeepMax = 3;

	while (True)
	{
		// open screen
		Open();

		// new game
		NewGame();

		// display board frame
		DispBoardFrame();

		// display board
		DispBoard();

		while (True)
		{
			// move computer or player
			if (Players[Player].comp)
				MoveComp();
			else
				if (MoveHuman()) break;

			if (KeyGet() == KEY_Y) break;

			// checkmat (9 chars)
			if (LastMove == NULL)
			{
				OutText("CHECKMATE");

				WaitMs(500);
				KeyFlush();
				while (KeyGet() == NOKEY) {}
				break;
			}

			// prepare move counter (3 chars)
			i = MoveNum/10;
			ch = MoveNum - i*10;
			buf[1] = ch + '0';
			ch = (i == 0) ? ' ' : ((i > 9) ? '*' : (i + '0'));
			buf[0] = ch;
			bufn = 2;
			buf[bufn++] = ':';

			if (Player == BLACK_PLAYER)
			{
				buf[0] = ' ';
				buf[1] = ' ';
				buf[2] = ' ';
			}

			// special cases
			if (LastMove->flags == MOVEFLAG_SMALL)
			{
				buf[bufn++] = 'O';
				buf[bufn++] = '-';
				buf[bufn++] = 'O';
			}
			else if (LastMove->flags == MOVEFLAG_BIG)
			{
				buf[bufn++] = 'O';
				buf[bufn++] = '-';
				buf[bufn++] = 'O';
				buf[bufn++] = '-';
				buf[bufn++] = 'O';
			}
			else
			{
				// set source field (2 chars)
				row = LastMove->src / 10;
				col = LastMove->src - row*10;
				buf[bufn++] = col+'a'-1;
				buf[bufn++] = row+'1'-2;

				// separator (1 char)
				buf[bufn++] = '-';

				// set destination field (2 chars)
				row = LastMove->dst / 10;
				col = LastMove->dst - row*10;
				buf[bufn++] = col+'a'-1;
				buf[bufn++] = row+'1'-2;

				// take piece
				if (LastMove->dstpiece != EMPTY) buf[bufn++] = 'x';

				// En Passant
				if (LastMove->flags == MOVEFLAG_EP) buf[bufn++] = 'e';
			}	

			// display move
			buf[bufn] = 0;
			OutText(buf);

//#ifdef DEBUG_STACK	// debug flag - display stack max. depth
//			TextSetUInt(&txt, MoveStackTopMax, 0);
//			OutText(&txt);
//#endif

			// change player
			Player ^= 1;

			// increase move counter
			if (Player == WHITE_PLAYER) MoveNum++;
		}
	}
}
