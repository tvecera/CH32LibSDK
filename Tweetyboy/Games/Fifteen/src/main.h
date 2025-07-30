
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
// image width: 32 pixels
// image height: 320 lines ... 16 tiles of height 20 lines
// image pitch: 32 bytes
//extern const u16 ImgTiles_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgTiles[10240] __attribute__ ((aligned(4)));

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
