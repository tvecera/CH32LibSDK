
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 8-bit paletted pixel graphics
// image width: 10 pixels
// image height: 150 lines
// image pitch: 10 bytes
//extern const u16 ImgTiles_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgTiles[1500] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 80 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[12800] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 80 pixels
// image height: 80 lines
// image pitch: 80 bytes
//extern const u16 ImgFrame_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgFrame[6400] __attribute__ ((aligned(4)));

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
