
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 1-bit pixel graphics
// image width: 144 pixels
// image height: 32 lines
// image pitch: 18 bytes
extern const u8 ImgTitle[576] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 46 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[7360] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 18 lines
// image pitch: 160 bytes
// - Every tile is 8 pixels width, 8 pixels is X distance, 6 lines height.
//extern const u16 ImgTiles_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgTiles[2880] __attribute__ ((aligned(4)));

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
