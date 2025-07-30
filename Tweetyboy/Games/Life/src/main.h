
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//#define DRV	VideoVGA // timings
#define TILESIZE 6	// tile size in number of pixels
#define MAPW0	(26+5)	// hidden width (=31)
#define MAPH0	(13+5)	// hidden height (=18)
#define MAPW	(WIDTH/TILESIZE) // real board width (=26)
#define MAPH	(HEIGHT/TILESIZE) // real board height (=13)
#define MAPSIZE	(MAPW0*MAPH0) // board size (=558)
#define SLOTNUM	1	// number of slots
#define MAPX	((MAPW0-MAPW)/2) // start X
#define MAPY	((MAPH0-MAPH)/2) // start X

#define BOARDW	(TILESIZE*MAPW+1) // board width (= 157)
#define BOARDH	(TILESIZE*MAPH+1) // board height (= 79)
#define BOARDX	1		// board X
#define BOARDY	0		// board Y

#define SPEED	300	// speed, sleep in [ms]

#define TILE_EMPTY	0	// empty tile
#define	TILE_FULL	1	// full tile
#define TILE_SELEMPTY 	2	// selected empty tile
#define TILE_SELFULL	3	// selected full tile

#define TILE_NUM	4	// number of tiles types

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 80 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[12800] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 6 pixels
// image height: 6 lines
// image pitch: 6 bytes
//extern const u16 ImgTileEmpty_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgTileEmpty[36] __attribute__ ((aligned(4)));
extern const u8 ImgTileEmptySel[36] __attribute__ ((aligned(4)));
extern const u8 ImgTileCell[36] __attribute__ ((aligned(4)));
extern const u8 ImgTileCellSel[36] __attribute__ ((aligned(4)));
#define TILE_WB	6

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
