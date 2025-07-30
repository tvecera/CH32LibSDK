
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define TILESIZE 6	// tile size in number of pixels
#define MAPW	25	// board width
#define MAPH	13	// board height
#define MAPSIZE	(MAPW*MAPH) // board size

#define MAZEW	(((MAPW+1)/2*2)-1) // maze width (=25, must be odd number)
#define MAZEH	(((MAPH+1)/2*2)-1) // maze height (=13, must be odd number)

#define BOARDW	(MAZEW*TILESIZE)	// board width in pixels (= 150)
#define BOARDH	(MAZEH*TILESIZE)	// board height in pixels (= 78)

#define BOARDX	5	// board X coordinate
#define BOARDY	1	// board Y coordinate

#define SPEED	1 //50	// step speed (delay in [ms])

// tile indices
enum {
	S_BLACK = 0,		// 0: black tile
	S_EMPTY_HIDE,		// 1: hidden empty field (grass)
	S_WALL_HIDE,		// 2: hidden wall
	S_DOOR_HIDE,		// 3: hidden door
	S_EMPTY,		// 4: empty field (grass)
	S_WALL,			// 5: wall
	S_DOOR,			// 6: door
	S_FACE,			// 7: face on empty field

	TILE_NUM		// number of tiles
};

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 80 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[12800] __attribute__ ((aligned(4)));

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
