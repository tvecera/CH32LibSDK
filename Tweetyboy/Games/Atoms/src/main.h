
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 80 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[12800] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 8 pixels
// image height: 152 lines
// image pitch: 8 bytes
//extern const u16 ImgTiles_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgTiles[1216] __attribute__ ((aligned(4)));

// tile indices
enum {
	TILE_0N = 0,	// empty tile, normal
	TILE_0I,	// empty tile, inverse
	TILE_11N,	// tile 1, player 1, normal
	TILE_11I,	// tile 1, player 1, inverse
	TILE_12N,	// tile 1, player 2, normal
	TILE_12I,	// tile 1, player 2, inverse
	TILE_21N,	// tile 2, player 1, normal
	TILE_21I,	// tile 2, player 1, inverse
	TILE_22N,	// tile 2, player 2, normal
	TILE_22I,	// tile 2, player 2, inverse
	TILE_31N,	// tile 3, player 1, normal
	TILE_31I,	// tile 3, player 1, inverse
	TILE_32N,	// tile 3, player 2, normal
	TILE_32I,	// tile 3, player 2, inverse
	TILE_41N,	// tile 4, player 1, normal
	TILE_41I,	// tile 4, player 1, inverse
	TILE_42N,	// tile 4, player 2, normal
	TILE_42I,	// tile 4, player 2, inverse
	TILE_COMP,	// computer
};

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
